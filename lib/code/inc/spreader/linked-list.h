// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_LINKED_LIST_H_INCLUDED
#define SPR_HEADER_LINKED_LIST_H_INCLUDED

#include <concepts>
#include <iterator>

namespace Spreader {

    template<class T, class Node>
    concept LinkedListNodeTraits = requires(Node * node) {
        { T::prev(node) } noexcept -> std::same_as<Node *>;
        { T::next(node) } noexcept -> std::same_as<Node *>;
        { T::setPrev(node, node) } noexcept;
        { T::setNext(node, node) } noexcept;
    };

    template<class Node, LinkedListNodeTraits<Node> NodeTraits> 
    class LinkedList {
    public:
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using value_type = std::remove_cv_t<Node>;
        using reference = Node &;
        using pointer = Node *;

    private:
        template<bool Direction>
        class IteratorImpl {
        
        friend LinkedList;
        
        public:
            using difference_type = typename LinkedList::difference_type;
            using value_type = typename LinkedList::value_type;
            using reference = typename LinkedList::reference;
            using pointer = typename LinkedList::pointer;
            using iterator_category = std::bidirectional_iterator_tag;
            using iterator_concept = std::bidirectional_iterator_tag;
        public:
            IteratorImpl() noexcept = default;

            auto operator*() const noexcept -> reference
                { return *this->m_ptr; }
            
            auto operator->() const noexcept -> pointer
                { return this->m_ptr; }

            auto operator++() noexcept -> IteratorImpl & {
                if constexpr (Direction)
                    this->m_ptr = NodeTraits::next(this->m_ptr);
                else
                    this->m_ptr = NodeTraits::prev(this->m_ptr);
                return *this;
            }
            auto operator++(int) noexcept -> IteratorImpl {
                iterator ret(*this);
                ++*this;
                return ret;
            }

            auto operator--() noexcept -> IteratorImpl &{
                if constexpr (Direction)
                    this->m_ptr = NodeTraits::prev(this->m_ptr);
                else
                    this->m_ptr = NodeTraits::next(this->m_ptr);
                return *this;
            }

            auto operator--(int) noexcept -> IteratorImpl {
                iterator ret(*this);
                --*this;
                return ret;
            }
            friend bool operator==(IteratorImpl lhs, IteratorImpl rhs) noexcept 
                { return lhs.m_ptr == rhs.m_ptr; }
            friend bool operator!=(IteratorImpl lhs, IteratorImpl rhs) noexcept 
                { return lhs.m_ptr != rhs.m_ptr; }

        private:
            IteratorImpl(Node * node) noexcept:
                m_ptr(node) 
            {}
        private:
            Node * m_ptr = nullptr;
        };

    public:
        using iterator = IteratorImpl<true>;
        using reverse_iterator = IteratorImpl<false>;
        
    public:
        constexpr LinkedList() noexcept = default;
        LinkedList(const LinkedList &) = delete;
        LinkedList & operator=(const LinkedList &) = delete;

        constexpr LinkedList(LinkedList && src) noexcept:
            m_first(src.m_first),
            m_last(src.m_last) {
            src.m_first = nullptr;
            src.m_last = nullptr;
        }

        constexpr auto operator=(LinkedList && src) noexcept  -> LinkedList & {
            this->m_first = src.m_first;
            this->m_last = src.m_last;
            src.m_first = nullptr;
            src.m_last = nullptr;
        }

        friend void swap(LinkedList & lhs, LinkedList & rhs) noexcept {
            std::swap(lhs.m_first, rhs.m_first);
            std::swap(lhs.m_last, rhs.m_last);
        }

        auto empty() const noexcept -> bool {
            return m_first == nullptr;
        }

        constexpr auto begin() const noexcept -> iterator { return iterator{m_first}; }
        constexpr auto end() const noexcept -> iterator { return iterator{}; }
        constexpr auto rbegin() const noexcept -> reverse_iterator { return reverse_iterator{m_last}; }
        constexpr auto rend() const noexcept -> reverse_iterator { return reverse_iterator{}; }
        
        auto front() noexcept -> reference
            { return *this->m_first; }
        auto back() noexcept -> reference
            { return *this->m_last; }

        void push_back(Node & node) noexcept {
            if (!this->m_first) {
                SPR_ASSERT_LOGIC(!this->m_last);
                NodeTraits::setPrev(&node, nullptr);
                NodeTraits::setNext(&node, nullptr);
                this->m_first = &node;
            } else {
                SPR_ASSERT_LOGIC(this->m_last);
                this->linkAfter(&node, this->m_last);
            }
            this->m_last = &node;
        }

        void push_front(Node & node) noexcept {
            if (!this->m_last) {
                SPR_ASSERT_LOGIC(!this->m_first);
                NodeTraits::setPrev(&node, nullptr);
                NodeTraits::setNext(&node, nullptr);
                this->m_last = &node;
            } else {
                SPR_ASSERT_LOGIC(this->m_first);
                this->linkBefore(&node, this->m_first);
            }
            this->m_first = &node;
        }

        void pop_back() noexcept {
            SPR_ASSERT_INPUT(this->m_last);
            if (auto prev = this->unlink(this->m_last).first) {
                this->m_last = prev;
            } else {
                this->m_first = nullptr;
                this->m_last = nullptr;
            }
        }

        void pop_front() noexcept {
            SPR_ASSERT_INPUT(this->m_first);
            if (auto next = this->unlink(this->m_first).second) {
                this->m_first = next;
            } else {
                this->m_first = nullptr;
                this->m_last = nullptr;
            }
        }

        auto insert(iterator pos, Node & node) noexcept -> iterator {
            if (pos.m_ptr) {
                this->linkBefore(&node, pos.m_ptr);
                if (pos.m_ptr == this->m_first)
                    this->m_first = &node;
            } else {
                if (!this->m_first) {
                    SPR_ASSERT_LOGIC(!this->m_last);
                    this->m_first = &node;
                } else {
                    SPR_ASSERT_LOGIC(this->m_last);
                    this->linkAfter(&node, this->m_last);
                }
                this->m_last = &node;
            }
            return iterator{&node};
        }

        template<class It>
        requires(std::is_same_v<It, iterator> || std::is_same_v<It, reverse_iterator>)
        auto erase(It it) noexcept -> It {
            auto [prev, next] = this->unlink(it.m_ptr);
            if (it.m_ptr == this->m_first)
                this->m_first = next;
            if (it.m_ptr == this->m_last)
                this->m_last = prev;
            if constexpr (std::is_same_v<It, iterator>)
                return iterator{next};
            else
                return reverse_iterator{prev};
        }

        void erase(Node & node) {
            this->erase(iterator{&node});
        }

    private:
        static auto unlink(Node * node) noexcept -> std::pair<Node *, Node *> {
            auto prev = NodeTraits::prev(node);
            if (prev)
                NodeTraits::setNext(prev, NodeTraits::next(node));
            auto next = NodeTraits::next(node);
            if (next)
                NodeTraits::setPrev(next, NodeTraits::prev(node));
            return {prev, next};
        }

        auto linkBefore(Node * node, Node * next) noexcept {
            auto prevOfNext = NodeTraits::prev(next);
            if (prevOfNext) 
                NodeTraits::setNext(prevOfNext, node);
            NodeTraits::setPrev(node, prevOfNext);
            NodeTraits::setPrev(next, node);
            NodeTraits::setNext(node, next);
        }
        
        auto linkAfter(Node * node, Node * prev) noexcept {
            auto nextOfPrev = NodeTraits::next(prev);
            if (nextOfPrev)
                NodeTraits::setPrev(nextOfPrev, node);
            NodeTraits::setNext(node, nextOfPrev);
            NodeTraits::setNext(prev, node);
            NodeTraits::setPrev(node, prev);
        }
    private:
        Node * m_first = nullptr;
        Node * m_last = nullptr;
    };
}

#endif 
