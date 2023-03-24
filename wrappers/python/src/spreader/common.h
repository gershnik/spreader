// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef HEADER_PYWRAPPER_COMMON_H_INCLUDED
#define HEADER_PYWRAPPER_COMMON_H_INCLUDED

#include <spreader/sheet.h>

#include <Python.h>

#include <intrusive_shared_ptr/python_ptr.h>

using String = Spreader::String;
using StringBuilder = Spreader::StringBuilder;
using SizeType = Spreader::SizeType;
using Sheet = Spreader::Sheet;
using Error = Spreader::Error;


using isptr::py_ptr;
using isptr::py_attach;
using isptr::py_retain;

#endif
