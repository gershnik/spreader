// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_TYPES_H_INCLUDED
#define SPR_HEADER_TYPES_H_INCLUDED

#include <sys_string/sys_string.h>

#include <intrusive_shared_ptr/refcnt_ptr.h>
#include <intrusive_shared_ptr/ref_counted.h>


namespace Spreader {

    using String = sysstr::sys_string;
    using StringBuilder = sysstr::sys_string_builder;
    #define SPRS(x) SYS_STRING_STATIC(x)


    using isptr::refcnt_ptr;
    using isptr::refcnt_attach;
    using isptr::refcnt_retain;
    using isptr::make_refcnt;
    using isptr::ref_counted;
    using isptr::ref_counted_flags;

    #if SPR_SINGLE_THREADED
        #define REFCNT_FLAGS ref_counted_flags::single_threaded
    #else
        #define REFCNT_FLAGS ref_counted_flags::none
    #endif

    template<class T>
    struct MarkBitwiseEnum { static constexpr bool Value = false; };

    template<class T>
    constexpr bool IsBitwiseEnum = std::is_enum_v<T> && MarkBitwiseEnum<T>::Value;

    template<class Enum>
    requires(IsBitwiseEnum<Enum>)
    constexpr auto operator|(Enum lhs, Enum rhs) noexcept {
        return static_cast<Enum>(
                    static_cast<std::underlying_type_t<Enum>>(lhs) |
                    static_cast<std::underlying_type_t<Enum>>(rhs));
    }
    template<class Enum>
    requires(IsBitwiseEnum<Enum>)
    constexpr auto operator&(Enum lhs, Enum rhs) noexcept {
        return static_cast<Enum>(
                    static_cast<std::underlying_type_t<Enum>>(lhs) &
                    static_cast<std::underlying_type_t<Enum>>(rhs));
    }
    template<class Enum>
    requires(IsBitwiseEnum<Enum>)
    constexpr auto operator^(Enum lhs, Enum rhs) noexcept {
        return static_cast<Enum>(
                    static_cast<std::underlying_type_t<Enum>>(lhs) ^
                    static_cast<std::underlying_type_t<Enum>>(rhs));
    }
    template<class Enum>
    requires(IsBitwiseEnum<Enum>)
    constexpr auto operator~(Enum arg) noexcept {
        return static_cast<Enum>(
                    ~static_cast<std::underlying_type_t<Enum>>(arg));
    }
    template<class Enum>
    requires(IsBitwiseEnum<Enum>)
    constexpr auto testFlags(Enum arg, Enum bits) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(arg & bits) != 0;
    }
    
    #define SPR_ESC(...) __VA_ARGS__
    #define SPR_U16(x) u##x
    #define SPR_U32(x) U##x

}

#endif
