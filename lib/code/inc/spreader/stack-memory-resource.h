// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_STACK_MEMORY_RESOURCE_H_INCLUDED
#define SPR_HEADER_STACK_MEMORY_RESOURCE_H_INCLUDED

#include <spreader/error-handling.h>
#include <spreader/util.h>

#include <list>

namespace Spreader {

    template<size_t PageSize>
    class StackMemoryResource {
    private:
        struct Page {
            alignas(alignof(std::max_align_t)) std::byte bytes[PageSize];
        };
        using PageList = std::list<Page>;

    public:
        StackMemoryResource() noexcept = default;
        StackMemoryResource(const StackMemoryResource &) = delete;
        StackMemoryResource & operator=(const StackMemoryResource &) = delete;

        auto allocate(size_t size, size_t alignment = alignof(std::max_align_t)) -> void * {

            if (size > sizeof(Page))
                throw std::bad_alloc();

            size_t alignedOffset = alignSize(this->m_offset, alignment);
            Page * currentPage;
            if (this->m_pages.empty() || alignedOffset > sizeof(Page) - size) {
                currentPage = &this->m_pages.emplace_back();
                alignedOffset = 0;
            } else {
                currentPage = &this->m_pages.back();
            }
            this->m_offset = alignedOffset + size;
            void * ret = static_cast<void *>(currentPage->bytes + alignedOffset);
            return ret;
        }

        void deallocate(void * p, size_t /*size*/, size_t /*alignment*/ = alignof(std::max_align_t)) {

            std::byte * pb = static_cast<std::byte *>(p);
            while(!this->m_pages.empty()) {
                Page * currentPage = &this->m_pages.back();
                if (pb >= currentPage->bytes && pb < currentPage->bytes + sizeof(Page)) {
                    SPR_ASSERT_LOGIC(pb < currentPage->bytes + this->m_offset);
                    this->m_offset = pb - currentPage->bytes;
                    return;
                }
                m_pages.pop_back();
                this->m_offset = sizeof(Page);
            }
            SPR_FATAL_ERROR("invalid address passed to deallocate");
        }

    private:
        PageList m_pages;
        size_t m_offset = 0;
    };

}


#endif
