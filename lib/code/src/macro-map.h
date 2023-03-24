/*
 * Created by William Swanson in 2012.
 *
 * I, William Swanson, dedicate this work to the public domain.
 * I waive all rights to the work worldwide under copyright law,
 * including all related and neighboring rights,
 * to the extent allowed by law.
 *
 * You can copy, modify, distribute and perform the work,
 * even for commercial purposes, all without asking permission.
 */

#ifndef SPR_HEADER_MACRO_MAP_H_INCLUDED
#define SPR_HEADER_MACRO_MAP_H_INCLUDED

#define SPR_EVAL0(...) __VA_ARGS__
#define SPR_EVAL1(...) SPR_EVAL0(SPR_EVAL0(SPR_EVAL0(__VA_ARGS__)))
#define SPR_EVAL2(...) SPR_EVAL1(SPR_EVAL1(SPR_EVAL1(__VA_ARGS__)))
#define SPR_EVAL3(...) SPR_EVAL2(SPR_EVAL2(SPR_EVAL2(__VA_ARGS__)))
#define SPR_EVAL4(...) SPR_EVAL3(SPR_EVAL3(SPR_EVAL3(__VA_ARGS__)))
#define SPR_EVAL(...)  SPR_EVAL4(SPR_EVAL4(SPR_EVAL4(__VA_ARGS__)))

#define SPR_MAP_END(...)
#define SPR_MAP_OUT
#define SPR_MAP_COMMA ,

#define SPR_MAP_GET_END2() 0, SPR_MAP_END
#define SPR_MAP_GET_END1(...) SPR_MAP_GET_END2
#define SPR_MAP_GET_END(...) SPR_MAP_GET_END1
#define SPR_MAP_NEXT0(test, next, ...) next SPR_MAP_OUT
#define SPR_MAP_NEXT1(test, next) SPR_MAP_NEXT0(test, next, 0)
#define SPR_MAP_NEXT(test, next)  SPR_MAP_NEXT1(SPR_MAP_GET_END test, next)

#define SPR_MAP0(f, x, peek, ...) f(x) SPR_MAP_NEXT(peek, SPR_MAP1)(f, peek, __VA_ARGS__)
#define SPR_MAP1(f, x, peek, ...) f(x) SPR_MAP_NEXT(peek, SPR_MAP0)(f, peek, __VA_ARGS__)

#define SPR_MAP_LIST_NEXT1(test, next) SPR_MAP_NEXT0(test, SPR_MAP_COMMA next, 0)
#define SPR_MAP_LIST_NEXT(test, next)  SPR_MAP_LIST_NEXT1(SPR_MAP_GET_END test, next)

#define SPR_MAP_LIST0(f, x, peek, ...) f(x) SPR_MAP_LIST_NEXT(peek, SPR_MAP_LIST1)(f, peek, __VA_ARGS__)
#define SPR_MAP_LIST1(f, x, peek, ...) f(x) SPR_MAP_LIST_NEXT(peek, SPR_MAP_LIST0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define SPR_MAP(f, ...) SPR_EVAL(SPR_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/**
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 */
#define SPR_MAP_LIST(f, ...) SPR_EVAL(SPR_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif