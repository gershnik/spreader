// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_MRU_CACHE_H_INCLUDED
#define SPR_HEADER_MRU_CACHE_H_INCLUDED

#include <spreader/linked-list.h>
#include <spreader/error-handling.h>

#include <vector>
#include <utility>
#include <compare>

namespace Spreader {

    template<class Key, class Value, class Compare = std::less<Key>>
    class MRUCache {

    private:
        struct Entry {
            template<class... Args>
            Entry(const Key & k, Args && ...args):
                value(std::piecewise_construct, 
                      std::forward_as_tuple(k),
                      std::forward_as_tuple(std::forward<Args>(args)...)) 
            {}

            std::pair<Key, Value> value;
            mutable const Entry * prev = nullptr;
            mutable const Entry * next = nullptr;

            friend auto operator<(const Entry & lhs, const Entry & rhs) -> bool 
                { return Compare()(lhs.value.first, rhs.value.first); }
            friend auto operator<(const Entry & lhs, const Key & rhs) -> bool 
                { return Compare()(lhs.value.first, rhs); }
            friend auto operator<(const Key & lhs, const Entry & rhs) -> bool
                { return Compare()(lhs, rhs.value.first); }

            struct LinkedListTraits {
                static auto next(const Entry * entry) noexcept -> const Entry *
                    { return entry->next; }
                static void setNext(const Entry * entry, const Entry * next) noexcept 
                    { entry->next = next; }
                static auto prev(const Entry * entry) noexcept -> const Entry *
                    { return entry->prev; } 
                static void setPrev(const Entry * entry, const Entry * prev) noexcept 
                    { entry->prev = prev; }
            };
        };
        using SetType = std::vector<Entry>;

    public:
        class iterator {
            friend MRUCache;
        public:
            using difference_type = typename SetType::iterator::difference_type;
            using value_type = decltype(Entry::value);
            using reference = value_type &;
            using pointer = value_type *;
            using iterator_category = std::bidirectional_iterator_tag;
            using iterator_concept = std::bidirectional_iterator_tag;
        public:
            iterator() = default;

            auto operator*() const noexcept -> reference
                { return const_cast<reference>(this->m_impl->value); }
            
            auto operator->() const noexcept -> pointer
                { return const_cast<pointer>(&(this->m_impl->value)); }

            auto operator++() noexcept -> iterator & {
                ++this->m_impl;
                return *this;
            }
            auto operator++(int) noexcept -> iterator {
                iterator ret(*this);
                ++*this;
                return ret;
            }

            auto operator--() noexcept -> iterator &{
                --this->m_impl;
                return *this;
            }

            auto operator--(int) noexcept -> iterator {
                iterator ret(*this);
                --*this;
                return ret;
            }
            friend bool operator==(iterator lhs, iterator rhs) noexcept 
                { return lhs.m_impl == rhs.m_impl; }
            friend bool operator!=(iterator lhs, iterator rhs) noexcept 
                { return lhs.m_impl != rhs.m_impl; }

        private:
            iterator(typename SetType::iterator impl): m_impl(impl) 
            {}
        private:
            typename SetType::iterator m_impl;
        };

        class const_iterator {
            friend MRUCache;
        public:
            using difference_type = typename SetType::iterator::difference_type;
            using value_type = decltype(Entry::value);
            using reference = const value_type &;
            using pointer = const value_type *;
            using iterator_category = std::bidirectional_iterator_tag;
            using iterator_concept = std::bidirectional_iterator_tag;
        public:
            const_iterator() = default;
            const_iterator(const iterator & it): m_impl(it.m_impl)
            {}

            auto operator*() const noexcept -> reference
                { return this->m_impl->value; }
            
            auto operator->() const noexcept -> pointer
                { return &(this->m_impl->value); }

            auto operator++() noexcept -> iterator & {
                ++this->m_impl;
                return *this;
            }
            auto operator++(int) noexcept -> iterator {
                iterator ret(*this);
                ++*this;
                return ret;
            }

            auto operator--() noexcept -> iterator &{
                --this->m_impl;
                return *this;
            }

            auto operator--(int) noexcept -> iterator {
                iterator ret(*this);
                --*this;
                return ret;
            }
            friend bool operator==(iterator lhs, iterator rhs) noexcept 
                { return lhs.m_impl == rhs.m_impl; }
            friend bool operator!=(iterator lhs, iterator rhs) noexcept 
                { return lhs.m_impl != rhs.m_impl; }

        private:
            const_iterator(typename SetType::const_iterator impl): m_impl(impl) 
            {}
        private:
            typename SetType::const_iterator m_impl;
        };

    public:
        enum LimitType {
            MaxMemory,
            MaxItemCount
        };

        MRUCache(LimitType type, size_t maxSize) {
            if (type == MaxMemory) {
                maxSize = maxSize / sizeof(Entry);
            }
            this->m_maxSize = (maxSize ? maxSize : 1);
            this->m_set.reserve(this->m_maxSize);
        }

        auto empty() const noexcept -> bool 
            { return this->m_set.empty(); }

        template<class... Args>
        auto emplace(const Key & k, Args && ...args) -> std::pair<iterator, bool> {
            
            auto innerIt = std::lower_bound(this->m_set.begin(), this->m_set.end(), k);
            bool inserted;
            if (innerIt == this->m_set.end() || (k < *innerIt)) {
                //we don't have have an identical element, let's insert it
                //but frst remove oldest if we are up to the limit
                if (this->m_set.size() == this->m_maxSize) {
                    const Entry & last = this->m_insertOrder.back();
                    this->m_insertOrder.pop_back();
                    auto lastIt = std::lower_bound(this->m_set.begin(), this->m_set.end(), last.value.first);
                    SPR_ASSERT_LOGIC(lastIt != this->m_set.end() && &last == &*lastIt);
                    this->m_set.erase(lastIt);
                }
                innerIt = this->m_set.emplace(innerIt, k, std::forward<Args>(args)...);
                inserted = true;
            } else {
                this->m_insertOrder.erase(*innerIt);
                inserted = false;
            }
            this->m_insertOrder.push_front(*innerIt);
            return {iterator{innerIt}, inserted};
        }

        auto erase(iterator it) -> iterator {
            this->m_insertOrder.erase(*it.m_impl);
            return iterator{this->m_set.erase(it.m_impl)};
        }

    private:
        SetType m_set;
        LinkedList<const Entry, typename Entry::LinkedListTraits> m_insertOrder;
        size_t m_maxSize;
    };

}

#endif
