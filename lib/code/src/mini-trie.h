// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_MINI_TRIE_H_INCLUDED
#define SPR_HEADER_MINI_TRIE_H_INCLUDED

#include <spreader/error-handling.h>
#include <spreader/util.h>

#include <vector>
#include <concepts>
#include <algorithm>
#include <string.h>
#include <stdint.h>

#ifdef SPR_TESTING
    #include <iostream>
#endif

namespace Spreader {

    template<class Char, std::unsigned_integral LengthType = uint8_t, std::unsigned_integral PayloadType = uint8_t>
    class MiniTrie {

    private:
        struct Node {
            LengthType length;
            LengthType firstChildOffset;
            LengthType nextSiblingOffset;
            PayloadType payload;
            Char chars[1];
        };

        struct BuildEntry {
            const Char * str;
            LengthType len;
            PayloadType index;

            friend auto operator<(const BuildEntry & lhs, const BuildEntry & rhs) noexcept {
                auto len = std::min(lhs.len, rhs.len);
                if (auto res = memcmp(lhs.str, rhs.str, len * sizeof(Char)); res != 0)
                    return res < 0;
                return lhs.len < rhs.len;
            }
        };

        using BuildVector = std::vector<BuildEntry>;
        using BuildIterator = typename BuildVector::iterator;
        
        enum TraversalState {
            Blank,
            AfterFirstChild,
            AfterNextSibling
        };
        struct BuildStackEntry {
            BuildStackEntry(LengthType parOff, bool ch, BuildIterator f, BuildIterator l):
                first(f),
                lastMatching(f),
                last(l),
                parentOffset(parOff),
                child(ch),
                state(Blank)
            {}
            
            BuildIterator first;
            BuildIterator lastMatching;
            BuildIterator last;
            LengthType parentOffset;
            LengthType offset;
            bool child;
            TraversalState state;
        };

    public:
        static inline constexpr PayloadType noMatch = PayloadType(-1);

    public:
        template<class ItF, class ItL>
        requires(std::is_convertible_v<typename std::iterator_traits<ItF>::iterator_category, std::input_iterator_tag> &&
                std::is_same_v<typename std::iterator_traits<ItF>::value_type, const Char *> &&
                std::equality_comparable_with<ItF, ItL>)
        MiniTrie(ItF first, ItL last) {
            if (first == last) {
                m_minSize = 0;
                return;
            }

            PayloadType emptyPayload = noMatch;
            
            BuildVector buildVector;
            if constexpr (std::is_convertible_v<typename std::iterator_traits<ItF>::iterator_category, std::random_access_iterator_tag>)
                buildVector.reserve(last - first);
            
            for(PayloadType i = 0; first != last; ++i, ++first) {
                auto str = *first;
                auto len = std::char_traits<Char>::length(str);
                if (!len) {
                    m_minSize = 0;
                    emptyPayload = i;
                    continue;
                }
                SPR_ASSERT_INPUT(len < noMatch);
                BuildEntry entry{str, LengthType(len), i};
                if (entry.len > m_maxSize)
                    m_maxSize = entry.len;
                if (entry.len < m_minSize)
                    m_minSize = entry.len;

                auto it = std::lower_bound(buildVector.begin(), buildVector.end(), entry);
                SPR_ASSERT_INPUT(it == buildVector.end() || it->len != entry.len || std::char_traits<Char>::compare(it->str, entry.str, it->len) != 0);
                buildVector.insert(it, entry);
            }

            m_data.resize(sizeof(Node));
            auto * rootNode = reinterpret_cast<Node *>(m_data.data());
            rootNode->payload = emptyPayload;
            if (!buildVector.empty()) {
                std::vector<BuildStackEntry> stack;
                stack.emplace_back(LengthType(0), true, buildVector.begin(), buildVector.end());
                build(stack);
            }
        }
        
        template<class Cont>
        MiniTrie(const Cont & cont):
            MiniTrie(cont.begin(), cont.end())
        {}
        
        MiniTrie(std::initializer_list<const Char *> strings):
            MiniTrie(strings.begin(), strings.end())
        {}

        template<class ItF, class ItL>
        requires(std::is_convertible_v<typename std::iterator_traits<ItF>::iterator_category, std::forward_iterator_tag> &&
                std::is_same_v<typename std::iterator_traits<ItF>::value_type, Char> &&
                std::equality_comparable_with<ItF, ItL>)
        auto prefixMatch(ItF & first, ItL last) const noexcept -> PayloadType {
            
            auto base = m_data.data();
            auto current = reinterpret_cast<const Node *>(base);
            
            if (!current)
                return noMatch;
            
            PayloadType ret = current->payload;
            ItF start = first;
            if (!current->firstChildOffset)
                return ret;
            current = reinterpret_cast<const Node *>(base + current->firstChildOffset);
                    
            for ( ; ; ) {

                if (doesntMatch(current, start, last)) {
                    if (!current->nextSiblingOffset)
                        break;
                    current = reinterpret_cast<const Node *>(base + current->nextSiblingOffset);
                    continue;
                }

                std::advance(start, current->length);
                ret = current->payload;
                if (!current->firstChildOffset)
                    break;
                current = reinterpret_cast<const Node *>(base + current->firstChildOffset);
            }

            if (ret != noMatch) {
                first = start;
            }

            return ret;
        }
        

        #ifdef SPR_TESTING
            friend auto operator<<(std::ostream & str, const MiniTrie & rhs) -> std::ostream & {
                
                auto root = reinterpret_cast<const Node *>(rhs.m_data.data());
                str << "MiniTrie{\n";
                if (root)
                    rhs.dump(str, 0, root);
                str << "}\n";
                return str;
            }

        #endif
        
        

    private:
        template<class ItF, class ItL>
        static auto doesntMatch(const Node * node, ItF first, ItL last) noexcept -> bool {
            
            //memcmp is slower than std::mismatch!
            //if constexpr (std::is_convertible_v<typename std::iterator_traits<ItF>::iterator_category, std::contiguous_iterator_tag>)
            //    return node->length > last - first || memcmp(node->chars, first, node->length * sizeof(Char)) != 0;
            //else {
                return std::mismatch(node->chars, node->chars + node->length, first, last).first != (node->chars + node->length);
            //}
        }

        void build(std::vector<BuildStackEntry> & stack) {
            
            while(!stack.empty()) {
                
                auto & stackEntry = stack.back();
                
                switch(stackEntry.state) {
                    case Blank: {
                        
                        LengthType prefixSize = stackEntry.first->len;
                        for(++stackEntry.lastMatching; stackEntry.lastMatching != stackEntry.last; ++stackEntry.lastMatching) {
                            LengthType matchingSize = 0;
                            while(matchingSize < prefixSize) {
                                
                                if (stackEntry.lastMatching->str[matchingSize] != stackEntry.first->str[matchingSize])
                                    break;
                                
                                ++matchingSize;
                            }
                            if (matchingSize == 0)
                                break;
                            prefixSize = matchingSize;
                        }
                        
                        stackEntry.offset = (decltype(stackEntry.offset))m_data.size();
                        auto newSize = alignSize(stackEntry.offset + sizeof(Node) + (prefixSize - 1) * sizeof(Char), alignof(Node));
                        m_data.resize(newSize);
                        auto * newNode = reinterpret_cast<Node *>(&m_data[stackEntry.offset]);
                        auto * parent = reinterpret_cast<Node *>(&m_data[stackEntry.parentOffset]);
                        
                        newNode->length = prefixSize;
                        newNode->payload = (prefixSize == stackEntry.first->len ? stackEntry.first->index: noMatch);
                        memcpy(newNode->chars, stackEntry.first->str, prefixSize * sizeof(Char));
                        if (stackEntry.child)
                            parent->firstChildOffset = stackEntry.offset;
                        else
                            parent->nextSiblingOffset = stackEntry.offset;
                        
                        for(auto it = stackEntry.first; it != stackEntry.lastMatching; ++it) {
                            it->str += prefixSize;
                            it->len -= prefixSize;
                        }
                        if (!stackEntry.first->len)
                            ++stackEntry.first;
                    }
                    stackEntry.state = AfterFirstChild;
                    if (stackEntry.first != stackEntry.lastMatching) {
                        stack.emplace_back(stackEntry.offset, true, stackEntry.first, stackEntry.lastMatching);
                        continue;
                    }
                    [[fallthrough]];
                case AfterFirstChild:
                    stackEntry.state = AfterNextSibling;
                    
                    if (stackEntry.lastMatching != stackEntry.last) {
                        stack.emplace_back(stackEntry.offset, false, stackEntry.lastMatching, stackEntry.last);
                        continue;
                    }
                    [[fallthrough]];
                case AfterNextSibling:
                    stack.pop_back();
                }
            }
        }

        #ifdef SPR_TESTING
            void dump(std::ostream & str, size_t offset, const Node * node) const {
                str << std::string(offset, ' ') << '"' << std::basic_string<Char>(node->chars, node->length) << "\" " << int(node->payload) << '\n';
                if (node->firstChildOffset)
                    dump(str, offset + 2, reinterpret_cast<const Node *>(m_data.data() + node->firstChildOffset));
                if (node->nextSiblingOffset)
                    dump(str, offset, reinterpret_cast<const Node *>(m_data.data() + node->nextSiblingOffset));
            };
        #endif

    private:
        std::vector<std::byte> m_data;
        LengthType m_minSize = LengthType(-1);
        LengthType m_maxSize = 0;
    };
}


#endif 
