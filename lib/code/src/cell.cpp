// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/cell.h>

#include <spreader/fixed-size-memory-resource.h>

using namespace Spreader;

static FixedSizeMemoryResource<sizeof(ValueCell), alignof(ValueCell), 1024> g_valueCellMemory;

void * ValueCell::operator new(size_t /*size*/) {
    return g_valueCellMemory.allocate();
}

void ValueCell::operator delete(void * p, size_t /*size*/) noexcept {
    g_valueCellMemory.deallocate(p);
}

static FixedSizeMemoryResource<sizeof(FormulaCell), alignof(FormulaCell), 1024> g_formulaCellMemory;

void * FormulaCell::operator new(size_t /*size*/) {
    return g_formulaCellMemory.allocate();
}

void FormulaCell::operator delete(void * p, size_t /*size*/) noexcept {
    g_formulaCellMemory.deallocate(p);
}

static FixedSizeMemoryResource<sizeof(FormulaCellExtension), alignof(FormulaCellExtension), 1024> g_formulaCellExtensionMemory;

void * FormulaCellExtension::operator new(size_t /*size*/) {
    return g_formulaCellExtensionMemory.allocate();
}

void FormulaCellExtension::operator delete(void * p, size_t /*size*/) noexcept {
    g_formulaCellExtensionMemory.deallocate(p);
}


