// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef HEADER_PYWRAPPER_MARSHALLING_H_INCLUDED
#define HEADER_PYWRAPPER_MARSHALLING_H_INCLUDED

#include "common.h"
#include "globals.h"

//Make call to toPython dependent
template<class T> auto callToPython(T && arg) -> py_ptr<PyObject>;

template<class T>
struct KnownType {
    PyObject * ptr = nullptr;

    explicit operator bool() const noexcept 
        { return ptr != nullptr; }

    static auto check(PyObject * ptr) -> KnownType;
    static auto badTypeMessage(const std::string & name) -> std::string;
};

template<class T>
struct NameProvider {
    auto name() const -> std::string {
        return this->_name;
    }

    const char * _name;
};

template<class T> struct MakeFromPythonResult { using Type = std::optional<T>; };
template<class T> struct MakeFromPythonResult<py_ptr<T>> { using Type = py_ptr<T>; };

template<class T> using FromPythonResult = typename MakeFromPythonResult<T>::Type;

template<class T> auto IsOptionalHelper(std::optional<T> * t) -> std::true_type;
auto IsOptionalHelper(...) -> std::false_type;
template<class T> constexpr bool IsOptional = decltype(IsOptionalHelper((T*)nullptr))::value;


template<class T>
auto fromPython(PyObject * obj, NameProvider<T> nameProvider) -> FromPythonResult<T> {
    if (auto known = KnownType<T>::check(obj)) 
        return fromPython(known, nameProvider);

    PyErr_SetString(PyExc_TypeError, KnownType<T>::badTypeMessage(nameProvider.name()).c_str());
    return FromPythonResult<T>{};
}


//PyObject

inline auto toPython(const py_ptr<PyObject> & obj) -> py_ptr<PyObject> {
    return obj;
}
inline auto toPython(py_ptr<PyObject> && obj) -> py_ptr<PyObject> {
    return std::move(obj);
}

template<> 
inline auto KnownType<py_ptr<PyObject>>::check(PyObject * obj) -> KnownType {
    return KnownType{obj};
}

template<> 
inline auto KnownType<py_ptr<PyObject>>::badTypeMessage(const std::string &) -> std::string {
    Py_FatalError("logic error"); //this can never be called
}

inline auto fromPython(KnownType<py_ptr<PyObject>> obj, NameProvider<py_ptr<PyObject>>) -> py_ptr<PyObject> {
    return py_retain(obj.ptr);
}

//PyTypeObject

template<> 
inline auto KnownType<py_ptr<PyTypeObject>>::check(PyObject * obj) -> KnownType {
    return KnownType{PyType_Check(obj) ? obj : nullptr};
}

template<> 
inline auto KnownType<py_ptr<PyTypeObject>>::badTypeMessage(const std::string & name) -> std::string {
    return name + " must be a type";
}

inline auto fromPython(KnownType<py_ptr<PyTypeObject>> obj, NameProvider<py_ptr<PyTypeObject>>) -> py_ptr<PyTypeObject> {
    return py_retain((PyTypeObject *)obj.ptr);
}

// std::optional

template<class T>
auto toPython(const std::optional<T> & obj) -> py_ptr<PyObject> {
    if (obj)
        return callToPython(*obj);
    return py_retain(Py_None);
}

template<class T>
auto toPython(std::optional<T> && obj) -> py_ptr<PyObject> {
    if (obj)
        return callToPython(std::move(*obj));
    return py_retain(Py_None);
}

//Tuple

template<size_t N>
auto makePythonTuple(py_ptr<PyObject> (&& array)[N]) -> py_ptr<PyObject> {
    auto res = py_attach(PyTuple_New(N));
    if (!res)
        return nullptr;
    for(size_t i = 0; i < N; ++i)
        PyTuple_SET_ITEM(res.get(), i, array[i].release());
    return res;
}

template<size_t I, class... Args>
auto tupleToPythonImpl(const std::tuple<Args...> & obj, py_ptr<PyObject> (&& itemsArray)[sizeof...(Args)]) -> py_ptr<PyObject> {

    constexpr size_t Length = sizeof...(Args);
    
    itemsArray[I] = callToPython(std::get<I>(obj));
    if (!itemsArray[I])
        return nullptr;

    if constexpr (I < Length - 1) {
        return tupleToPythonImpl<I + 1>(obj, std::move(itemsArray));
    } else {
        return makePythonTuple(std::move(itemsArray));
    }
}

template<size_t I, class... Args>
auto tupleToPythonImpl(std::tuple<Args...> && obj, py_ptr<PyObject> (&& itemsArray)[sizeof...(Args)]) -> py_ptr<PyObject> {

    constexpr size_t Length = sizeof...(Args);
    
    itemsArray[I] = callToPython(std::move(std::get<I>(obj)));
    if (!itemsArray[I])
        return nullptr;

    if constexpr (I < Length - 1) {
        return tupleToPythonImpl<I + 1>(std::move(obj), std::move(itemsArray));
    } else {
        return makePythonTuple(std::move(itemsArray));
    }
}

template<class... Args>
auto toPython(const std::tuple<Args...> & obj) -> py_ptr<PyObject> {
    if constexpr (sizeof...(Args) > 0) {
        py_ptr<PyObject> items[sizeof...(Args)];
        return tupleToPythonImpl<0>(obj, std::move(items));
    } else {
        return py_attach(PyTuple_New(0));
    }
}

template<class... Args>
auto toPython(std::tuple<Args...> && obj) -> py_ptr<PyObject> {
    if constexpr (sizeof...(Args) > 0) {
        py_ptr<PyObject> items[sizeof...(Args)];
        return tupleToPythonImpl<0>(std::move(obj), std::move(items));
    } else {
        return py_attach(PyTuple_New(0));
    }
}

template<class... Args>
struct NameProvider<std::tuple<Args...>> {

    auto name() const -> std::string {
        return this->_name;
    }

    template<size_t I>
    auto itemNameProvider() const {
        return std::get<I>(this->_items);
    }

    const char * _name;
    std::tuple<NameProvider<Args>...> _items;
};

template<class... Args>
struct KnownType<std::tuple<Args...>> {
    PyObject * ptr;

    explicit operator bool() const noexcept 
        { return ptr != nullptr; }
    static auto check(PyObject * ptr) -> KnownType 
        { return KnownType{PyTuple_Check(ptr) ? ptr : nullptr}; }
    static auto badTypeMessage(const std::string & name) -> std::string 
        { return name + " must be a tuple"; }
};

template<size_t I, class... Args>
auto tupleFromPythonImpl(PyObject * tuple, NameProvider<std::tuple<Args...>> nameProvider, 
                         std::tuple<FromPythonResult<Args>...> & items) -> bool {

    constexpr size_t Length = sizeof...(Args);

    auto elem = PyTuple_GET_ITEM(tuple, I);
    auto & item = std::get<I>(items);
    using ExtractType = std::remove_cvref_t<decltype(*item)>;
    item = ::fromPython<ExtractType>(elem, nameProvider.template itemNameProvider<I>());
    if (!item)
        return false;

    if constexpr (I < Length - 1) {
        return tupleFromPythonImpl<I + 1>(tuple, nameProvider, items);
    } else {
        return true;
    }
    
}

template<class... Args>
inline auto fromPython(KnownType<std::tuple<Args...>> tuple, NameProvider<std::tuple<Args...>> nameProvider) -> FromPythonResult<std::tuple<Args...>> {

    constexpr size_t Length = sizeof...(Args);

    if (PyTuple_GET_SIZE(tuple.ptr) != Length) {
        PyErr_SetString(PyExc_TypeError, (nameProvider.name() + " must have " + std::to_string(Length) + " elements").c_str());
        return std::nullopt;
    }

    std::tuple<FromPythonResult<Args>...> items;
    if (!tupleFromPythonImpl<0>(tuple.ptr, nameProvider, items))
        return std::nullopt;
    return tupleTransform<std::tuple<Args...>>(std::move(items), [](auto && arg) {
        using ArgType = std::remove_cvref_t<decltype(arg)>;
        if constexpr (IsOptional<ArgType>)
            return std::move(*arg);
        else
            return std::move(arg);
            
    });
}


//bool 

inline auto toPython(bool val) -> py_ptr<PyObject> {
    return py_retain(val ? Py_True : Py_False);
}

template<> 
inline auto KnownType<bool>::check(PyObject * obj) -> KnownType {
    return KnownType{PyBool_Check(obj) ? obj : nullptr};
}

template<>
inline auto KnownType<bool>::badTypeMessage(const std::string & name) -> std::string {
    return name + " must be a bool";
}

inline auto fromPython(KnownType<bool> obj, NameProvider<bool> nameProvider) -> FromPythonResult<bool> {
    return obj.ptr == Py_True;
}


//unsigned integers

template<class T>
concept PythonicUnsigned =  std::is_integral_v<T> && 
                            !std::is_signed_v<T> && 
                            !std::is_same_v<T, bool> && 
                            sizeof(T) <= sizeof(unsigned long long);

template<PythonicUnsigned T>
inline auto toPython(T val) -> py_ptr<PyObject> {
    if constexpr (sizeof(T) <= sizeof(unsigned long))
        return py_attach(PyLong_FromUnsignedLong(val));
    else 
        return py_attach(PyLong_FromUnsignedLongLong(val));
}

template<PythonicUnsigned T>
struct KnownType<T> {
    PyObject * ptr;

    explicit operator bool() const noexcept 
        { return ptr != nullptr; }
    static auto check(PyObject * ptr) -> KnownType 
        { return KnownType{PyLong_Check(ptr) ? ptr : nullptr}; }
    static auto badTypeMessage(const std::string & name) -> std::string
        { return name + " must be an integer"; }
};


template<PythonicUnsigned T>
inline auto fromPython(KnownType<T> obj, NameProvider<T> nameProvider) -> FromPythonResult<T> {
    
    using MarshalType = std::conditional_t<sizeof(T) <= sizeof(unsigned long), unsigned long, unsigned long long>;

    MarshalType ret;
    if constexpr (sizeof(T) <= sizeof(unsigned long)) {
        ret = PyLong_AsUnsignedLong(obj.ptr);
    } else {
        ret = PyLong_AsUnsignedLong(obj.ptr);
    }
    if (PyErr_Occurred()) 
        return FromPythonResult<T>{};
    if constexpr (sizeof(T) < sizeof(MarshalType)) {
        if (ret > std::numeric_limits<T>::max()) {
            PyErr_SetString(PyExc_OverflowError, (nameProvider.name() + " is out of range").c_str());
            return FromPythonResult<T>{};
        }
    }
    return ret;
    
}

//double

template<>
struct KnownType<double> {
    PyObject * ptr;
    bool isInteger;

    explicit operator bool() const noexcept 
        { return ptr != nullptr; }
    static auto check(PyObject * ptr) -> KnownType { 
        if (PyLong_Check(ptr))
            return KnownType{ptr, true};
        else if (PyFloat_Check(ptr))
            return KnownType{ptr, false};
        return KnownType{nullptr}; 
    }
    static auto badTypeMessage(const std::string & name) -> std::string
        { return name + " must be numeric"; }
};

inline auto fromPython(KnownType<double> obj, NameProvider<double> nameProvider) -> FromPythonResult<double> {
    double ret;
    if (obj.isInteger)
        ret = PyLong_AsDouble(obj.ptr);
    else
        ret = PyFloat_AS_DOUBLE(obj.ptr);
    if (PyErr_Occurred()) 
        return FromPythonResult<double>{};
    return ret;
}

//String

inline auto toPython(const String & obj) -> py_ptr<PyObject> {
    return py_retain(obj.py_str());
}

template<> 
inline auto KnownType<String>::check(PyObject * obj) -> KnownType {
    return KnownType{PyUnicode_Check(obj) ? obj : nullptr};
}

template<>
inline auto KnownType<String>::badTypeMessage(const std::string & name) -> std::string {
    return name + " must be a string";
}

inline auto fromPython(KnownType<String> obj, NameProvider<String>) -> FromPythonResult<String> {
    return String(obj.ptr);
}

//Number

inline auto toPython(Spreader::Number num) -> py_ptr<PyObject> {
    return py_attach(PyFloat_FromDouble(num.value()));
}

//Blank

inline auto toPython(Spreader::Scalar::Blank) -> py_ptr<PyObject> {
    return py_retain(Py_None);
}

template<> 
inline auto KnownType<Spreader::Scalar::Blank>::check(PyObject * obj) -> KnownType {
    return KnownType{obj == Py_None ? obj : nullptr};
}

template<>
inline auto KnownType<Spreader::Scalar::Blank>::badTypeMessage(const std::string & name) -> std::string {
    return name + " must be None";
}

inline auto fromPython(KnownType<Spreader::Scalar::Blank>, NameProvider<Spreader::Scalar::Blank>) -> FromPythonResult<Spreader::Scalar::Blank> {
    return Spreader::Scalar::Blank{};
}

//Point

inline auto toPython(Spreader::Point pt) -> py_ptr<PyObject> {
    return toPython(std::tuple(pt.x, pt.y));
}

template<> 
struct KnownType<Spreader::Point> {

    using ImplType = KnownType<std::tuple<SizeType, SizeType>>;

    explicit operator bool() const noexcept 
        { return bool(impl); }
    static auto check(PyObject * obj) -> KnownType 
        { return {ImplType::check(obj)}; }
    static auto badTypeMessage(const std::string & name) -> std::string 
        { return ImplType::badTypeMessage(name); }

    ImplType impl;
};

inline auto fromPython(KnownType<Spreader::Point> tuple, NameProvider<Spreader::Point> nameProvider) -> FromPythonResult<Spreader::Point> {

    auto unpacked = fromPython(tuple.impl, { nameProvider.name().c_str(), { {"x"}, {"y"} } });
    if (!unpacked)
        return std::nullopt;
    auto [x, y] = *unpacked;
    constexpr auto max = Sheet::maxSize();
    if (x >= max.width || y >= max.height) {
        PyErr_SetString(PyExc_OverflowError, (nameProvider.name() + " is out of range").c_str());
        return FromPythonResult<Spreader::Point>{};
    }
    return Spreader::Point{x, y};    
}

//Size

inline auto toPython(Spreader::Size size) -> py_ptr<PyObject> {
    return toPython(std::tuple(size.width, size.height));
}

//Rect

inline auto toPython(Spreader::Rect rect) -> py_ptr<PyObject> {
    return toPython(std::tuple(rect.origin.x, rect.origin.y,
                               rect.size.width, rect.size.height));
}

template<> 
struct KnownType<Spreader::Rect> {

    using ImplType = KnownType<std::tuple<SizeType, SizeType, SizeType, SizeType>>;

    explicit operator bool() const noexcept 
        { return bool(impl); }
    static auto check(PyObject * obj) -> KnownType 
        { return {ImplType::check(obj)}; }
    static auto badTypeMessage(const std::string & name) -> std::string 
        { return ImplType::badTypeMessage(name); }

    ImplType impl;
};


inline auto fromPython(KnownType<Spreader::Rect> tuple, NameProvider<Spreader::Rect> nameProvider) -> FromPythonResult<Spreader::Rect> {

    auto unpacked = fromPython(tuple.impl, { nameProvider.name().c_str(), { {"x"}, {"y"}, {"width"}, {"height"} } });
    if (!unpacked)
        return std::nullopt;
    auto [x, y, width, height] = *unpacked;
    constexpr auto max = Sheet::maxSize();
    if (x >= max.width || y >= max.height ||
        width > max.width || height > max.height ||
        max.width - width < x || max.height - y < height) {
        PyErr_SetString(PyExc_OverflowError, (nameProvider.name() + " is out of range").c_str());
        return FromPythonResult<Spreader::Rect>{};
    }
    return Spreader::Rect{{x, y}, {width, height}};
}

//FormulaInfo

inline auto toPython(const Sheet::FormulaInfo & info) -> py_ptr<PyObject> {
    return toPython(std::forward_as_tuple(info.text, info.extent));
}

//LengthInfo

inline auto toPython(const Sheet::LengthInfo & info) -> py_ptr<PyObject> {
    return toPython(std::forward_as_tuple(info.length, info.hidden));
}

//Error

template<> 
inline auto KnownType<Error>::check(PyObject * obj) -> KnownType {
    return KnownType{PyObject_TypeCheck(obj, g_errorValueClass.get()) ? obj : nullptr};
}

template<>
inline auto KnownType<Error>::badTypeMessage(const std::string & name) -> std::string {
    return name + " must be an ErrorValue";
}

inline auto fromPython(KnownType<Error> errObj, NameProvider<Error> nameProvider) -> FromPythonResult<Error> {

    static auto valueAttr = SPRS("value");
    
    auto attr = py_attach(PyObject_GetAttr(errObj.ptr, valueAttr.py_str()));
    if (!attr) {
        PyErr_SetString(PyExc_TypeError, ("'value' attribute not present on " + nameProvider.name()).c_str());
        return FromPythonResult<Error>{};
    }
    auto value = fromPython<unsigned long>(attr.get(), {"error value"});
    if (!value)
        return FromPythonResult<Error>{};
    return Error(*value);
}

inline auto toPython(Error err) -> py_ptr<PyObject> {
    auto args = toPython(std::tuple((unsigned long)err));
    if (!args)
        return nullptr;
    return py_attach(PyObject_CallObject((PyObject *)g_errorValueClass.get(), args.get()));
}

//Scalar


template<>
inline auto fromPython<Spreader::Scalar>(PyObject * obj, NameProvider<Spreader::Scalar> nameProvider) -> FromPythonResult<Spreader::Scalar> {

    if (auto known = KnownType<Spreader::Scalar::Blank>::check(obj)) {
        return fromPython(known, {nameProvider._name});
    } else if (auto known = KnownType<bool>::check(obj)) {
        return fromPython(known, {nameProvider._name});
    } else if (auto known = KnownType<double>::check(obj)) {
        return fromPython(known, {nameProvider._name});
    } else if (auto known = KnownType<String>::check(obj)) {
        return fromPython(known, {nameProvider._name});
    } else if (auto known = KnownType<Error>::check(obj)) {
        return fromPython(known, {nameProvider._name});
    } else {
        PyErr_SetString(PyExc_TypeError, (nameProvider.name() + " is not a valid scalar type").c_str());
        return FromPythonResult<Spreader::Scalar>{};
    }
}

inline auto toPython(const Spreader::Scalar & scalar) -> py_ptr<PyObject> {
    return applyVisitor([](const auto & val) -> py_ptr<PyObject> {
        return toPython(val);
    }, scalar);
}


//Epilog

template<class T> auto callToPython(T && arg)  -> py_ptr<PyObject> {
    return toPython(std::forward<T>(arg));
}

#endif
