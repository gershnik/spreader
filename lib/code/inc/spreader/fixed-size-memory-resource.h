// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_FIXED_SIZE_MEMORY_RESOURCE_H_INCLUDED
#define SPR_HEADER_FIXED_SIZE_MEMORY_RESOURCE_H_INCLUDED

#include <spreader/error-handling.h>
#include <spreader/linked-list.h>
#include <spreader/util.h>

#include <list>
#include <mutex>

namespace Spreader {

    template<size_t Size, size_t Alignment, size_t PageCount>
    class FixedSizeMemoryResource {

    private:
        static constexpr size_t AlignedSize = alignSize(Size, Alignment);

        class Page;
        using PageList = std::list<Page>;

        struct Unit {
            typename PageList::iterator pageIt;
            union Data {
                struct {
                    Unit * prevFree;  
                    Unit * nextFree;    
                } link;
                alignas(Alignment) std::byte bytes[AlignedSize];
            } data;
        };

        struct LinkedUnitTraits {
            static auto next(Unit * unit) noexcept -> Unit *
                { return unit->data.link.nextFree; }
            static void setNext(Unit * unit, Unit * next) noexcept 
                { unit->data.link.nextFree = next; }
            static auto prev(Unit * unit) noexcept -> Unit *
                { return unit->data.link.prevFree; } 
            static void setPrev(Unit * unit, Unit * prev) noexcept 
                { unit->data.link.prevFree = prev; }
        };

        using FreeList = LinkedList<Unit, LinkedUnitTraits>;

    private:
        class Page {
        public:
            auto init(typename PageList::iterator it, FreeList & freeList) noexcept -> Unit * {
                for(size_t i = 1; i < std::size(m_units); ++i) {
                    auto & unit = m_units[i];
                    freeList.push_back(unit);
                    unit.pageIt = it;
                }
                m_fillCount = 1;
                m_units[0].pageIt = it;
                return &m_units[0];
            }
            void term(FreeList & freeList) noexcept {
                for(auto & unit: m_units) {
                    freeList.erase(unit);
                }
            }

            void allocUnit() noexcept {
                ++m_fillCount;
            }
            auto deallocUnit() noexcept -> bool {
                return --m_fillCount == 0;
            }
        private:
            Unit m_units[PageCount];
            size_t m_fillCount = 0;
        };

    public:
        auto allocate() -> void * {
        #if !SPR_SINGLE_THREADED
            auto lock = std::lock_guard(m_mutex);
        #endif
            Unit * ret;
            if (this->m_freeList.empty()) {
                this->m_pages.emplace_back();
                auto pageIt = --this->m_pages.end();
                ret = pageIt->init(pageIt, this->m_freeList);
            } else {
                ret = &this->m_freeList.front();
                this->m_freeList.pop_front();
                ret->pageIt->allocUnit();
            }
            return ret->data.bytes;
        }

        void deallocate(void * p) noexcept {
        #if !SPR_SINGLE_THREADED
            auto lock = std::lock_guard(m_mutex);
        #endif
            Unit * unit = reinterpret_cast<Unit *>(static_cast<std::byte *>(p) - offsetof(typename Unit::Data, bytes) - offsetof(Unit, data));
            this->m_freeList.push_front(*unit);
            auto & page = *unit->pageIt;
            if (page.deallocUnit()) {
                page.term(this->m_freeList);
                this->m_pages.erase(unit->pageIt);
            }
        }

    private:
        std::list<Page> m_pages;
        LinkedList<Unit, LinkedUnitTraits> m_freeList;
    #if !SPR_SINGLE_THREADED
        std::mutex m_mutex;
    #endif
    };
}

#endif
