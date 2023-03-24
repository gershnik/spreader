// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_ITERATION_H_INCLUDED
#define SPR_HEADER_ITERATION_H_INCLUDED

#include <spreader/geometry.h>


namespace Spreader {


    class SizeCursor {
    public:
        using size_type = SizeType;
        using difference_type = DistanceType;
        using value_type = Point;
        using reference = const Point;
        using pointer = void;
        
        using iterator_category = std::forward_iterator_tag;
    public:
        constexpr SizeCursor(Size size) noexcept:
            m_size(size), m_current{0, 0}
        {}

        constexpr auto operator*() const noexcept -> Point 
            { return m_current; }

        explicit constexpr operator bool() const noexcept
            { return m_current.y < m_size.height; }

        constexpr auto operator++() noexcept -> SizeCursor & {
            if (++m_current.x == m_size.width) {
                m_current.x = 0;
                ++m_current.y;
            }
            return *this;
        }

        constexpr auto operator++(int) noexcept -> SizeCursor {
            SizeCursor ret = *this;
            ++(*this);
            return ret;
        }

        friend constexpr auto operator==(const SizeCursor & lhs, const SizeCursor & rhs) noexcept -> bool
            { return lhs.m_current == rhs.m_current; }
        friend constexpr auto operator!=(const SizeCursor & lhs, const SizeCursor & rhs)  noexcept -> bool
            { return !(lhs == rhs); }
        
        friend constexpr auto operator==(const SizeCursor & lhs, std::nullptr_t) noexcept -> bool
            { return !lhs; }
        friend constexpr auto operator==(std::nullptr_t, const SizeCursor & rhs) noexcept -> bool
            { return !rhs; }
        friend constexpr auto operator!=(const SizeCursor & lhs, std::nullptr_t rhs) noexcept -> bool
            { return !(lhs == rhs); }
        friend constexpr auto operator!=(std::nullptr_t lhs, const SizeCursor & rhs) noexcept -> bool
            { return !(lhs == rhs); }

    private:
        Size m_size;
        Point m_current;
    };

    class SizeByRow {
    public:
        constexpr SizeByRow(Size size) noexcept : m_size(size)
        {}

        constexpr auto begin() const noexcept -> SizeCursor {
            return SizeCursor(m_size);
        }
        constexpr auto end() const noexcept -> std::nullptr_t {
            return nullptr;
        }
    private:
        Size m_size;
    };
    
    class DimensionIterator {
        friend class ReverseDimensionIterator;
    public:
        using size_type = SizeType;
        using difference_type = DistanceType;
        using value_type = SizeType;
        using reference = const SizeType;
        using pointer = void;
        
        using iterator_category = std::random_access_iterator_tag;
    public:
        constexpr DimensionIterator(SizeType pos) noexcept:
            m_current{pos}
        {}
        
        constexpr auto operator*() const noexcept -> SizeType
            { return m_current; }
        
        constexpr auto operator++() noexcept -> DimensionIterator & {
            ++m_current;
            return *this;
        }
        constexpr auto operator++(int) noexcept -> DimensionIterator {
            DimensionIterator ret = *this;
            ++(*this);
            return ret;
        }
        constexpr auto operator--() noexcept -> DimensionIterator & {
            --m_current;
            return *this;
        }
        constexpr auto operator--(int) noexcept -> DimensionIterator {
            DimensionIterator ret = *this;
            --(*this);
            return ret;
        }
        
        constexpr auto operator+=(DistanceType dist) noexcept -> DimensionIterator & {
            m_current += dist;
            return *this;
        }
        constexpr auto operator-=(DistanceType dist) noexcept -> DimensionIterator & {
            m_current -= dist;
            return *this;
        }
        friend constexpr auto operator+(const DimensionIterator & lhs, DistanceType rhs) noexcept -> DimensionIterator {
            return DimensionIterator(lhs.m_current + rhs);
        }
        friend constexpr auto operator+(DistanceType lhs, const DimensionIterator & rhs) noexcept -> DimensionIterator {
            return DimensionIterator(lhs + rhs.m_current);
        }
        friend constexpr auto operator-(const DimensionIterator & lhs, DistanceType rhs) noexcept -> DimensionIterator {
            return DimensionIterator(lhs.m_current - rhs);
        }
        friend constexpr auto operator-(const DimensionIterator & lhs, const DimensionIterator & rhs) noexcept -> DistanceType {
            return lhs.m_current - rhs.m_current;
        }
        
        friend constexpr auto operator==(const DimensionIterator & lhs, const DimensionIterator & rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(const DimensionIterator & lhs, const DimensionIterator & rhs) noexcept -> bool = default;
        friend constexpr auto operator<=>(const DimensionIterator & lhs, const DimensionIterator & rhs) noexcept -> std::strong_ordering = default;
        
    private:
        SizeType m_current;
    };
    
    class ReverseDimensionIterator {
    public:
        using size_type = SizeType;
        using difference_type = DistanceType;
        using value_type = SizeType;
        using reference = const SizeType;
        using pointer = void;
        
        using iterator_category = std::random_access_iterator_tag;
    public:
        constexpr ReverseDimensionIterator(SizeType pos) noexcept:
            m_current{pos}
        {}
        
        constexpr ReverseDimensionIterator(DimensionIterator base) noexcept:
            m_current{base.m_current}
        {}

        constexpr auto operator*() const noexcept -> SizeType
            { return m_current - 1; }
        
        constexpr auto operator++() noexcept -> ReverseDimensionIterator & {
            --m_current;
            return *this;
        }
        constexpr auto operator++(int) noexcept -> ReverseDimensionIterator {
            ReverseDimensionIterator ret = *this;
            ++(*this);
            return ret;
        }
        constexpr auto operator--() noexcept -> ReverseDimensionIterator & {
            ++m_current;
            return *this;
        }
        constexpr auto operator--(int) noexcept -> ReverseDimensionIterator {
            ReverseDimensionIterator ret = *this;
            --(*this);
            return ret;
        }
        
        constexpr auto operator+=(DistanceType dist) noexcept -> ReverseDimensionIterator & {
            m_current -= dist;
            return *this;
        }
        constexpr auto operator-=(DistanceType dist) noexcept -> ReverseDimensionIterator & {
            m_current += dist;
            return *this;
        }
        friend constexpr auto operator+(const ReverseDimensionIterator & lhs, DistanceType rhs) noexcept -> ReverseDimensionIterator {
            return ReverseDimensionIterator(lhs.m_current - rhs);
        }
        friend constexpr auto operator+(DistanceType lhs, const ReverseDimensionIterator & rhs) noexcept -> ReverseDimensionIterator {
            return ReverseDimensionIterator(rhs.m_current - lhs);
        }
        friend constexpr auto operator-(const ReverseDimensionIterator & lhs, DistanceType rhs) noexcept -> ReverseDimensionIterator {
            return ReverseDimensionIterator(lhs.m_current + rhs);
        }
        friend constexpr auto operator-(const ReverseDimensionIterator & lhs, const ReverseDimensionIterator & rhs) noexcept -> DistanceType {
            return rhs.m_current - lhs.m_current;
        }
        
        constexpr auto base() const noexcept -> DimensionIterator {
            return DimensionIterator(m_current);
        }

        friend constexpr auto operator==(const ReverseDimensionIterator & lhs, const ReverseDimensionIterator & rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(const ReverseDimensionIterator & lhs, const ReverseDimensionIterator & rhs) noexcept -> bool = default;
        friend constexpr auto operator<=>(const ReverseDimensionIterator & lhs, const ReverseDimensionIterator & rhs) noexcept -> std::strong_ordering {
            return rhs.m_current <=> lhs.m_current;
        }
    private:
        SizeType m_current;
    };
    
    inline constexpr auto reverse(DimensionIterator it) noexcept -> ReverseDimensionIterator {
        return ReverseDimensionIterator(it);
    }
    inline constexpr auto reverse(ReverseDimensionIterator it) noexcept -> DimensionIterator {
        return it.base();
    }
    
    template<class It>
    class DimensionRangeImpl {
    public:
        using iterator = It;
        using reverse_iterator = decltype(reverse(std::declval<It>()));
        
        using size_type = typename iterator::size_type;
        using difference_type = typename iterator::difference_type;
        using value_type = typename iterator::value_type;
        using reference = typename iterator::reference;
        using pointer = typename iterator::pointer;
        
        
    public:
        constexpr DimensionRangeImpl(iterator start, iterator end) noexcept:
            m_start{start}, m_end{end} {
        
            SPR_ASSERT_INPUT(m_start <= m_end);
        }
        
        constexpr auto size() const noexcept -> size_type
            { return m_end - m_start; }
        constexpr auto ssize() const noexcept -> difference_type
            { return m_end - m_start; }
        constexpr auto empty() const noexcept
            { return m_start == m_end; }

        constexpr auto first() const noexcept -> value_type
            { return *m_start; }
        constexpr auto last() const noexcept -> value_type
            { return *(m_end - 1); }

        
        auto begin() const noexcept -> iterator { return m_start; }
        auto end() const noexcept -> iterator { return m_end; }
        auto rbegin() const noexcept -> reverse_iterator { return reverse(m_end); }
        auto rend() const noexcept -> reverse_iterator { return reverse(m_start); }
        
        
        friend constexpr auto reverse(DimensionRangeImpl range) noexcept -> DimensionRangeImpl<reverse_iterator> {
            return DimensionRangeImpl<reverse_iterator>(range.rbegin(), range.rend());
        }
        
    private:
        iterator m_start;
        iterator m_end;
    };
    
    using DimensionRange = DimensionRangeImpl<DimensionIterator>;
    using ReverseDimensionRange = DimensionRangeImpl<ReverseDimensionIterator>;
}

#endif
