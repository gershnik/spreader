// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_COMPILER_H_INCLUDED
#define SPR_HEADER_COMPILER_H_INCLUDED

#define SPR_STRINGIZE1(x) #x
#define SPR_STRINGIZE(x) SPR_STRINGIZE1(x)

#ifdef _MSC_VER

	#define SPR_WARNING_PUSH _Pragma("warning(push)")
	#define SPR_MSVC_WARNING_DISABLE(code) __pragma(warning(disable:code))
	#define SPR_CLANG_WARNING_DISABLE(name)
	#define SPR_GCC_WARNING_DISABLE(name)
	#define SPR_WARNING_POP _Pragma("warning(pop)")

	#define SPR_ALWAYS_INLINE __forceinline
	#define SPR_TRIVIAL_ABI
    #define SPR_BIT_CAST(t, v) std::bit_cast<t>((v))
    #define SPR_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
	#define SPR_UNREACHABLE __assume(false)

	#if defined(_M_AMD64) 
		#define SPR_HAS_MSVC_64_BIT_INTRINSICS
	#endif

#elif defined(__clang__)

	#define SPR_WARNING_PUSH _Pragma("clang diagnostic push")
	#define SPR_MSVC_WARNING_DISABLE(code) 
	#define SPR_CLANG_WARNING_DISABLE(name) _Pragma(SPR_STRINGIZE(clang diagnostic ignored name)) 
	#define SPR_GCC_WARNING_DISABLE(name)
	#define SPR_WARNING_POP _Pragma("clang diagnostic pop")

	#define SPR_ALWAYS_INLINE [[gnu::always_inline]] inline
	#define SPR_TRIVIAL_ABI [[clang::trivial_abi]]
    #define SPR_BIT_CAST(t, v) __builtin_bit_cast(t, (v))
    #define SPR_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#define SPR_UNREACHABLE __builtin_unreachable()

	#ifdef __SIZEOF_INT128__

		#define SPR_HAS_UINT128 1
		using uint128_t = __uint128_t;

	#endif

#elif defined(__GNUC__)

	#define SPR_WARNING_PUSH _Pragma("GCC diagnostic push")
	#define SPR_MSVC_WARNING_DISABLE(code) 
	#define SPR_CLANG_WARNING_DISABLE(name) 
	#define SPR_GCC_WARNING_DISABLE(name) _Pragma(SPR_STRINGIZE(GCC diagnostic ignored name)) 
	#define SPR_WARNING_POP _Pragma("GCC diagnostic pop")

	#define SPR_ALWAYS_INLINE [[gnu::always_inline]] inline
	#define SPR_TRIVIAL_ABI
    #define SPR_BIT_CAST(t, v) __builtin_bit_cast(t, (v))
    #define SPR_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#define SPR_UNREACHABLE __builtin_unreachable()

	#ifdef __SIZEOF_INT128__

		#define SPR_HAS_UINT128 1
		using uint128_t = __uint128_t;

	#endif

#endif


#endif
