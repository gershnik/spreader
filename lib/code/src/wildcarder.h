// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_WILDCARDER_H_INCLUDED
#define SPR_HEADER_WILDCARDER_H_INCLUDED

#include <spreader/error-handling.h>
#include <spreader/compiler.h>

#include <vector>


namespace Spreader {


    template<class Char, class PatternIt = const Char *, class TextIt = const Char *>
    requires(std::is_same_v<std::remove_const_t<typename std::iterator_traits<PatternIt>::value_type>, Char> &&
            std::is_same_v<std::remove_const_t<typename std::iterator_traits<TextIt>::value_type>, Char>)
    class Wildcarder {
        
    private:
        struct SearchEntry {
            PatternIt pattern;
            TextIt lastFound;
        };
        
    public:
        Wildcarder(Char escape) :
            m_escape(escape)
        {}
        
        template<class PCont>
        SPR_ALWAYS_INLINE auto match(const PCont & pattern, TextIt first, TextIt last) -> bool
        requires(std::is_convertible_v<decltype(std::begin(pattern)), PatternIt> &&
                std::is_convertible_v<decltype(std::end(pattern)), PatternIt>) {
            
            return match(std::begin(pattern), std::end(pattern), first, last);
        }
        
        template<class PCont, class TCont>
        SPR_ALWAYS_INLINE auto match(const PCont & pattern, const TCont & text) -> bool
        requires(std::is_convertible_v<decltype(std::begin(pattern)), PatternIt> &&
                std::is_convertible_v<decltype(std::end(pattern)), PatternIt> &&
                std::is_convertible_v<decltype(std::begin(text)), TextIt> &&
                std::is_convertible_v<decltype(std::end(text)), TextIt>){
            
            return match(std::begin(pattern), std::end(pattern), std::begin(text), std::end(text));
        }
        
        auto match(PatternIt patternFirst, PatternIt patternLast, TextIt first, TextIt last) -> bool {
            
            bool inEscape = false;
            m_searchNext = false;
            m_patternFirst = patternFirst;
            m_patternLast = patternLast;
            m_first = first;
            m_last = last;
            m_searchStack.clear();

            for( ; ; ) {
                
                if (m_patternFirst == m_patternLast) [[unlikely]] {
                    
                    if (inEscape) {
                        inEscape = false;
                        
                        if (!matchChar(m_escape)) {
                            if (!retry())
                                return false;
                            continue;
                        }
                    }
                    
                    if (!matchTail()) {
                        if (!retry())
                            return false;
                        continue;
                    }
                    return true;
                }
                
                
                Char c = *m_patternFirst;
                
                
                if (inEscape) {
                    
                    inEscape = false;
                    
                    if (c != Char('*') && c != Char('?') && c != m_escape) {
                        
                        if (!matchChar(m_escape)) {
                            if (!retry())
                                return false;
                            continue;
                        }
                    }
                    
                    if (!matchChar(c)) {
                        if (!retry())
                            return false;
                        continue;
                    }
                
                } else {
                    
                    if (c == Char('*')) {
                        m_searchNext = true;
                    } else if (c == m_escape) {
                        inEscape = true;
                    } else if (c == Char('?')) {
                        if (!matchAny()) {
                            if (!retry())
                                return false;
                            continue;
                        }
                    } else {
                        if (!matchChar(c)) {
                            if (!retry())
                                return false;
                            continue;
                        }
                    }
                }
                ++m_patternFirst;
            }
        }
        
    private:
        auto matchChar(Char c) -> bool {
            if (m_searchNext) {
                if constexpr (std::is_pointer_v<TextIt> && std::is_same_v<std::remove_const_t<decltype(*m_first)>, char>) {
                    auto found = (const char *)memchr(m_first, c, m_last - m_first);
                    if (!found)
                        return false;
                    m_searchStack.push_back({m_patternFirst, found});
                    m_first = found + 1;
                    
                } else {
                    auto found = std::find(m_first, m_last, c);
                    if (found == m_last)
                        return false;
                    m_searchStack.push_back({m_patternFirst, found});
                    m_first = found;
                    ++m_first;
                }
                m_searchNext = false;
            } else {
                if (m_first == m_last || *m_first != c)
                    return false;
                ++m_first;
            }
            return true;
        }
        
        auto matchAny() -> bool {
            if (m_first == m_last)
                return false;
            
            if (m_searchNext) {
                m_searchStack.push_back({m_patternFirst, m_first});
                m_searchNext = false;
            }
            
            ++m_first;
            return true;
        }
        
        SPR_ALWAYS_INLINE auto matchTail() noexcept -> bool {
            return m_searchNext ? true : m_first == m_last;
        }

        auto retry() -> bool {
            if (m_searchStack.empty())
                return false;

            auto & fallback = m_searchStack.back();
            m_patternFirst = fallback.pattern;
            m_first = fallback.lastFound + 1;
            m_searchStack.pop_back();
            m_searchNext = true;
            return true;
        }

    private:
        bool m_searchNext;
        Char m_escape;
        PatternIt m_patternFirst;
        PatternIt m_patternLast;
        TextIt m_first;
        TextIt m_last;
        std::vector<SearchEntry> m_searchStack;
    };


}

#endif
