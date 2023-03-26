// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef HEADER_PYWRAPPER_ARGUMENTS_H_INCLUDED
#define HEADER_PYWRAPPER_ARGUMENTS_H_INCLUDED


#include "marshalling.h"

#include <spreader/typelist.h>

#include <tuple>

template<class RequiredTL, class OptionalTL>
using ArgumentsTupleType = 
    typename Spreader::Cat<RequiredTL, 
                           typename OptionalTL::template Transform<FromPythonResult>
    >::template WrapIn<std::tuple>;


template<class RequiredTL, class OptionalTL = Spreader::Typelist<>>
struct Arguments {

    template<size_t I>
    auto positional() {
        return std::get<I>(this->values);
    }

    ArgumentsTupleType<RequiredTL, OptionalTL> values;
};

template<class RequiredTL, class OptionalTL>
struct NameProvider<Arguments<RequiredTL, OptionalTL>>  {
    auto func() const -> std::string {
        return this->_func;
    }

    template<size_t I>
    auto argNameProvider() const {
        return std::get<I>(this->_args);
    }

    const char * _func;
    decltype(NameProvider<ArgumentsTupleType<RequiredTL, OptionalTL>>::_items) _args;
};

template<class RequiredTL, class OptionalTL>
struct ArgumentsFromPythonConverter {

private:
    using RawTypes = typename Spreader::Cat<RequiredTL, OptionalTL>;
    using TempType = typename RawTypes::template Transform<FromPythonResult>::template WrapIn<std::tuple>;

    template<size_t Idx>
    struct Transformer {
        auto operator()(auto && arg) {
            using ArgType = std::remove_cvref_t<decltype(arg)>;
            if constexpr (Idx < RequiredTL::size && IsOptional<ArgType>)
                return std::move(*arg);
            else
                return std::move(arg);
        }
    };

    template<size_t I, class NP>
    static auto parseArgsImpl(PyObject * tuple, NP nameProvider, TempType & items) -> bool {

        constexpr size_t Length = std::tuple_size_v<TempType>;

        if (I == PyTuple_GET_SIZE(tuple))
            return true;

        auto elem = PyTuple_GET_ITEM(tuple, I);
        auto & item = std::get<I>(items);
        using ExtractType = std::tuple_element_t<I, typename RawTypes::template WrapIn<std::tuple>>;
        item = ::fromPython<ExtractType>(elem, nameProvider.template argNameProvider<I>());
        if (!item)
            return false;

        if constexpr (I < Length - 1) {
            return parseArgsImpl<I + 1>(tuple, nameProvider, items);
        } else {
            return true;
        }
        
    }
    
public:
    static auto fromPython(PyObject * args, NameProvider<Arguments<RequiredTL, OptionalTL>> nameProvider) -> FromPythonResult<Arguments<RequiredTL, OptionalTL>> {
        
        constexpr size_t RequiredLength = RequiredTL::size;
        constexpr size_t OptionalLength = OptionalTL::size;

        if (!PyTuple_Check(args)) {
            Py_FatalError("arguments must be a tuple");
        }

        size_t givenLength = PyTuple_GET_SIZE(args);
        if (givenLength < RequiredLength) {
            PyErr_SetString(PyExc_TypeError, (nameProvider.func() + " takes at least " + std::to_string(RequiredLength) + " argument(s)").c_str());
            return std::nullopt;
        }
        if (givenLength > RequiredLength + OptionalLength) {
            PyErr_SetString(PyExc_TypeError, (nameProvider.func() + " takes at most " + std::to_string(RequiredLength + OptionalLength) + " argument(s)").c_str());
            return std::nullopt;
        }

        
        TempType items;
        if (!parseArgsImpl<0>(args, nameProvider, items))
            return std::nullopt;

        return Arguments<RequiredTL, OptionalTL>{
            .values = tupleTransformIndexed<ArgumentsTupleType<RequiredTL, OptionalTL>, Transformer>(std::move(items))
        };
        
    }
};

template<class RequiredTL, class OptionalTL = Spreader::Typelist<>>
auto parseArguments(PyObject * args, NameProvider<Arguments<RequiredTL, OptionalTL>> nameProvider) -> FromPythonResult<Arguments<RequiredTL, OptionalTL>> {
    return ArgumentsFromPythonConverter<RequiredTL, OptionalTL>::fromPython(args, nameProvider);
}


#endif
