// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_GEOMETRY_H_INCLUDED
#define SPR_HEADER_GEOMETRY_H_INCLUDED

#include <spreader/types.h>

#include <stdint.h>

#if SPR_TESTING
    #include <iostream>
#endif

namespace Spreader {

    using SizeType = uint32_t;
    using DistanceType = std::make_signed_t<SizeType>;

    constexpr DistanceType MaxDistanceType = std::numeric_limits<DistanceType>::max();
    constexpr DistanceType MinDistanceType = -MaxDistanceType; //NOT numeric_limits::min()!
    constexpr SizeType MaxSizeType = static_cast<SizeType>(MaxDistanceType);

    struct Point {
        SizeType x = 0;
        SizeType y = 0;

        friend constexpr auto operator==(Point lhs, Point rhs) noexcept -> bool { 
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
        friend constexpr auto operator!=(Point lhs, Point rhs) noexcept -> bool { 
            return !(lhs == rhs);
        }
    };

    struct Size {
        SizeType width = 0;
        SizeType height = 0;

        friend constexpr auto operator==(Size lhs, Size rhs) noexcept -> bool { 
            return lhs.width == rhs.width && lhs.height == rhs.height;
        }
        friend constexpr auto operator!=(Size lhs, Size rhs) noexcept -> bool { 
            return !(lhs == rhs);
        }

    #define DECLARE_BIN_MATH(op) \
        constexpr auto operator op##=(Size rhs) noexcept -> Size & { \
            this->width op##= rhs.width; \
            this->height op##= rhs.height; \
            return *this; \
        } \
        friend constexpr auto operator op(Size lhs, Size rhs) -> Size { \
            lhs op##= rhs; \
            return lhs; \
        }

        DECLARE_BIN_MATH(*)
        DECLARE_BIN_MATH(/)
        DECLARE_BIN_MATH(%)
        DECLARE_BIN_MATH(+)
        DECLARE_BIN_MATH(-)
        DECLARE_BIN_MATH(<<)
        DECLARE_BIN_MATH(>>)
        DECLARE_BIN_MATH(&)
        DECLARE_BIN_MATH(|)

    #undef DECLARE_BIN_MATH

        constexpr auto operator~() const noexcept -> Size {
            return Size{~this->width, ~this->height};
        }
        
        void extendTo(Size other) {
            if (other.width > this->width)
                this->width = other.width;
            if (other.height > this->height)
                this->height = other.height;
        }
    };

    constexpr inline auto asSize(Point pt) -> Size {
        return Size{pt.x, pt.y};
    }

    #define DECLARE_POINT_SIZE_MATH(op) \
        inline constexpr auto operator op##=(Point & p, Size s) noexcept -> Point & { \
            p.x op##= s.width; \
            p.y op##= s.height; \
            return p; \
        } \
        inline constexpr auto operator op(Point p, Size s) noexcept -> Point { \
            p op##= s; \
            return p; \
        }

    DECLARE_POINT_SIZE_MATH(*)
    DECLARE_POINT_SIZE_MATH(/)
    DECLARE_POINT_SIZE_MATH(%)
    DECLARE_POINT_SIZE_MATH(+)
    DECLARE_POINT_SIZE_MATH(-)
    DECLARE_POINT_SIZE_MATH(<<)
    DECLARE_POINT_SIZE_MATH(>>)
    DECLARE_POINT_SIZE_MATH(&)
    DECLARE_POINT_SIZE_MATH(|)

    #undef DECLARE_POINT_SIZE_MATH

    struct Rect {
        Point origin;
        Size size;

        Point end() const noexcept {
            return this->origin + this->size;
        }

        friend constexpr auto operator==(Rect lhs, Rect rhs) noexcept -> bool { 
            return lhs.origin == rhs.origin && lhs.size == rhs.size;
        }
        friend constexpr auto operator!=(Rect lhs, Rect rhs) noexcept -> bool { 
            return !(lhs == rhs);
        }
    };

    #if SPR_TESTING

        inline auto operator<<(std::ostream & str, Size size) -> std::ostream & {
            return str << '{' << size.width << ',' << size.height << '}';
        }
        inline auto operator<<(std::ostream & str, Point pt) -> std::ostream & {
            return str << '{' << pt.x << ',' << pt.y << '}';
        }
        inline auto operator<<(std::ostream & str, Rect rect) -> std::ostream & {
            return str << '{' << rect.origin << ',' << rect.size << '}';
        }

    #endif
}

#endif
