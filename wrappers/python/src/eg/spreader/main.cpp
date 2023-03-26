// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "util.h"
#include "marshalling.h"
#include "arguments.h"
#include "globals.h"


using namespace Spreader;
using namespace isptr;

#define EXTERNAL_PROLOG try {

#define EXTERNAL_EPILOG } catch (std::bad_alloc &) { \
    return PyErr_NoMemory(); \
} catch (std::exception & ex) { \
    PyErr_SetString(PyExc_Exception, ex.what());\
    return nullptr; \
}

py_ptr<PyTypeObject> g_errorValueClass;

struct LengthInfoGeneratorObject {
    using GeneratorType = CoroGenerator<std::tuple<SizeType, SizeType, const Sheet::LengthInfo &>>;

    PyObject_HEAD
private:
    GeneratorType generator;
    GeneratorType::iterator iter;

    static PyMethodDef methods[];
    static PyTypeObject typeObject;

public:
    static auto registerType(const py_ptr<PyObject> & module) -> bool {
        if (PyType_Ready(&typeObject) < 0)
            return false;
        auto type = py_retain((PyObject *)&typeObject);
        if (PyModule_AddObject(module.get(), "LengthInfoGenerator", type.get()) < 0)
            return false;
        type.release();
        return true;
    }
    static auto createInstance(GeneratorType && gen) -> py_ptr<PyObject> {
        auto args = toPython(std::tuple());
        if (!args)
            return nullptr;
        auto ret = py_attach(PyObject_CallObject((PyObject *)&typeObject, args.get()));
        if (!ret)
            return nullptr;
        auto * self = (LengthInfoGeneratorObject *)ret.get();
        self->generator = std::move(gen);
        self->iter = self->generator.begin();
        return ret;
    }

private:
    
    static auto _new__(PyTypeObject * type, PyObject * args, PyObject * kwds) -> PyObject *
    {
        EXTERNAL_PROLOG
            void * mem = type->tp_alloc(type, 0);
            if (!mem)
                return nullptr;
            new (mem) LengthInfoGeneratorObject;
            return (PyObject *)mem;
        EXTERNAL_EPILOG
    }

    static void dealloc(LengthInfoGeneratorObject * self) {
        self->~LengthInfoGeneratorObject();
        Py_TYPE(self)->tp_free((PyObject *)self);
    }

    static auto _iter__(LengthInfoGeneratorObject * self) -> PyObject * {
        return py_retain((PyObject *)self).release();
    }
    static auto _next__(LengthInfoGeneratorObject * self) -> PyObject * {
        EXTERNAL_PROLOG
            if (self->iter != self->generator.end()) {
                auto ret = toPython(*(self->iter));
                ++self->iter;
                return ret.release();
            }
            PyErr_SetNone(PyExc_StopIteration); 
            return nullptr;
        EXTERNAL_EPILOG
    }
};

PyMethodDef LengthInfoGeneratorObject::methods[] = {
    {nullptr}  /* Sentinel */
};

PyTypeObject LengthInfoGeneratorObject::typeObject = [](){
    PyTypeObject ret = {PyVarObject_HEAD_INIT(nullptr, 0)};
    
    ret.tp_name = "_spreader.LengthInfoGenerator";
    ret.tp_basicsize = sizeof(LengthInfoGeneratorObject);
    ret.tp_itemsize = 0;
    ret.tp_dealloc = destructor(dealloc);
    ret.tp_flags = Py_TPFLAGS_DEFAULT;
    ret.tp_doc = nullptr;
    ret.tp_iter = getiterfunc(_iter__);
    ret.tp_iternext = iternextfunc(_next__);
    ret.tp_methods = methods;
    ret.tp_new = _new__;
    return ret;
}();


struct SheetObject {
    PyObject_HEAD

private:

    Sheet sheet;

    static PyMethodDef methods[];
    static PyTypeObject typeObject;

public:

    static auto registerType(const py_ptr<PyObject> & module) -> bool {
        if (PyType_Ready(&typeObject) < 0)
            return false;
        auto type = py_retain((PyObject *)&typeObject);
        if (PyModule_AddObject(module.get(), "Sheet", type.get()) < 0)
            return false;
        type.release();
        return true;
    }
    
    static auto _new__(PyTypeObject * type, PyObject * args, PyObject * kwds) -> PyObject *
    {
        EXTERNAL_PROLOG
            void * mem = type->tp_alloc(type, 0);
            if (!mem)
                return nullptr;
            new (mem) SheetObject; //we don't need unique_ptr here since this is the last throwing statement
            return (PyObject *)mem;
        EXTERNAL_EPILOG
    }

    static void dealloc(SheetObject * self) {
        self->~SheetObject();
        Py_TYPE(self)->tp_free((PyObject *)self);
    }

    static auto size(SheetObject * self, PyObject * Py_UNUSED(ignored)) -> PyObject * {
        EXTERNAL_PROLOG
            return toPython(self->sheet.size()).release();
        EXTERNAL_EPILOG
    }

    static auto maxSize(void * /*null*/, PyObject * Py_UNUSED(ignored)) -> PyObject * {
        EXTERNAL_PROLOG
            return toPython(Sheet::maxSize()).release();
        EXTERNAL_EPILOG
    }

    static auto nonNullCellCount(SheetObject * self, PyObject * Py_UNUSED(ignored)) -> PyObject * {
        EXTERNAL_PROLOG
            return toPython(self->sheet.nonNullCellCount()).release();
        EXTERNAL_EPILOG
    }

    static auto setValueCell(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Point, Scalar>>(args, {"getEditInfo", {{"coordinate"}, {"value"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [coord, value] = parsedArgs->values;

            self->sheet.setValueCell(coord, value);
            Py_RETURN_NONE;
        EXTERNAL_EPILOG
    }

    static auto setFormulaCell(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Point, String>>(args, {"getEditInfo", {{"coordinate"}, {"formula"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [coord, formula] = parsedArgs->values;

            self->sheet.setFormulaCell(coord, formula);
            Py_RETURN_NONE;
        EXTERNAL_EPILOG
    }

    static auto clearCellValue(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Point>>(args, {"getEditInfo", {{"coordinate"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [coord] = parsedArgs->values;

            self->sheet.clearCell(coord);
            Py_RETURN_NONE;
        EXTERNAL_EPILOG
    }

    static auto getValue(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Point>>(args, {"getEditInfo", {{"coordinate"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [coord] = parsedArgs->values;

            return toPython(self->sheet.getValue(coord)).release();
        EXTERNAL_EPILOG
    }

    static auto getEditInfo(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Point>>(args, {"getEditInfo", {{"coordinate"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [coord] = parsedArgs->values;

            auto formula = toPython(self->sheet.getFormulaInfo(coord));
            if (!formula)
                return nullptr;
            return toPython(std::tuple(formula)).release();
        EXTERNAL_EPILOG
    }

    static auto copyCell(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Point, Spreader::Rect>>(args, {"copyCell", {{"from"}, {"to"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [from, to] = parsedArgs->values;

            self->sheet.copyCell(from, to);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto copyCells(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Rect, Spreader::Point>>(args, {"copyCells", {{"from"}, {"to"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [from, to] = parsedArgs->values;

            self->sheet.copyCells(from, to);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto moveCell(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Point, Spreader::Point>>(args, {"moveCell", {{"from"}, {"to"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [from, to] = parsedArgs->values;

            self->sheet.moveCell(from, to);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto moveCells(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<Spreader::Rect, Spreader::Point>>(args, {"moveCells", {{"from"}, {"to"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [from, to] = parsedArgs->values;

            self->sheet.moveCells(from, to);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto deleteRows(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, {"deleteRows", {{"y"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [row, count] = parsedArgs->values;

            if (!checkRowRange(row, count))
                return nullptr;
            
            self->sheet.deleteRows(row, count);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto deleteColumns(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, {"deleteColumns", {{"x"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [column, count] = parsedArgs->values;

            if (!checkColumnRange(column, count))
                return nullptr;
            
            self->sheet.deleteColumns(column, count);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto insertRows(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, {"insertRows", {{"y"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [row, count] = parsedArgs->values;

            if (!checkRowRange(row, count))
                return nullptr;
            
            self->sheet.insertRows(row, count);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto insertColumns(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, {"insertColumns", {{"x"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [column, count] = parsedArgs->values;

            if (!checkColumnRange(column, count))
                return nullptr;
            
            self->sheet.insertColumns(column, count);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto suspendRecalc(SheetObject * self, PyObject * Py_UNUSED(ignored)) -> PyObject * {
        EXTERNAL_PROLOG
            self->sheet.suspendRecalc();
            Py_RETURN_NONE;
        EXTERNAL_EPILOG
    }

    static auto resumeRecalc(SheetObject * self, PyObject * Py_UNUSED(ignored)) -> PyObject * {
        EXTERNAL_PROLOG
            self->sheet.resumeRecalc();
            Py_RETURN_NONE;
        EXTERNAL_EPILOG
    }

    static auto recalculate(SheetObject * self, PyObject * Py_UNUSED(ignored)) -> PyObject * {
        EXTERNAL_PROLOG
            self->sheet.recalculate();
            Py_RETURN_NONE;
        EXTERNAL_EPILOG
    }

    static auto setRowsHeight(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType, Sheet::LengthType>>(args, 
                                    {"setRowsHeight", {{"y"}, {"count"}, {"height"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [row, count, height] = parsedArgs->values;

            if (!checkRowRange(row, count))
                return nullptr;
            
            self->sheet.setRowHeight(row, count, height);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto clearRowsHeight(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, 
                                    {"clearRowsHeight", {{"y"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [row, count] = parsedArgs->values;

            if (!checkRowRange(row, count))
                return nullptr;
            
            self->sheet.clearRowHeight(row, count);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto hideRows(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType, bool>>(args, 
                                    {"hideRows", {{"y"}, {"count"}, {"hidden"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [row, count, hidden] = parsedArgs->values;

            if (!checkRowRange(row, count))
                return nullptr;
            
            self->sheet.hideRow(row, count, hidden);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto getRowHeight(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType>>(args,  {"getRowHeight", {{"y"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [row] = parsedArgs->values;

            if (!checkRow(row))
                return nullptr;
            
            auto res = self->sheet.getRowHeight(row);

            return toPython(res).release();

        EXTERNAL_EPILOG
    }

    static auto rowHeightGenerator(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, 
                                    {"rowHeightGenerator", {{"y"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [row, count] = parsedArgs->values;

            if (!checkRowRange(row, count))
                return nullptr;

            auto gen = self->sheet.rowHeights(row, count);
            auto ret = LengthInfoGeneratorObject::createInstance(std::move(gen));
            return ret.release();

        EXTERNAL_EPILOG
    }

    static auto setColumnsWidth(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType, Sheet::LengthType>>(args, 
                                    {"setColumnsWidth", {{"x"}, {"count"}, {"width"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [col, count, width] = parsedArgs->values;

            if (!checkColumnRange(col, count))
                return nullptr;
            
            self->sheet.setColumnWidth(col, count, width);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto clearColumnsWidth(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, 
                                    {"clearColumnsWidth", {{"x"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [col, count] = parsedArgs->values;

            if (!checkColumnRange(col, count))
                return nullptr;
            
            self->sheet.clearColumnWidth(col, count);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto hideColumns(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType, bool>>(args, 
                                    {"hideColumns", {{"x"}, {"count"}, {"hidden"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [col, count, hidden] = parsedArgs->values;

            if (!checkColumnRange(col, count))
                return nullptr;
            
            self->sheet.hideColumn(col, count, hidden);

            Py_RETURN_NONE;

        EXTERNAL_EPILOG
    }

    static auto getColumnWidth(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType>>(args,  {"getColumnWidth", {{"x"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [col] = parsedArgs->values;

            if (!checkColumn(col))
                return nullptr;
            
            auto res = self->sheet.getColumnWidth(col);

            return toPython(res).release();

        EXTERNAL_EPILOG
    }

    static auto columnWidthGenerator(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType, SizeType>>(args, 
                                    {"columnWidthGenerator", {{"x"}, {"count"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [col, count] = parsedArgs->values;

            if (!checkColumnRange(col, count))
                return nullptr;

            auto gen = self->sheet.columnWidths(col, count);
            auto ret = LengthInfoGeneratorObject::createInstance(std::move(gen));
            return ret.release();

        EXTERNAL_EPILOG
    }

    static auto indexToRow(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType>>(args, {"indexToRow", {{"index"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [idx] = parsedArgs->values;
            if (!checkRow(idx))
                return nullptr;
            return toPython(self->sheet.indexToRow(idx)).release();
        EXTERNAL_EPILOG
    }

    static auto indexToColumn(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<SizeType>>(args, {"indexToColumn", {{"index"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [idx] = parsedArgs->values;
            if (!checkColumn(idx))
                return nullptr;
            return toPython(self->sheet.indexToColumn(idx)).release();
        EXTERNAL_EPILOG
    }

    static auto parseRow(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<String>>(args, {"parseRow", {{"argument"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [val] = parsedArgs->values;
            return toPython(self->sheet.parseRow(val)).release();
        EXTERNAL_EPILOG
    }

    static auto parseColumn(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<String>>(args, {"parseColumn", {{"argument"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [val] = parsedArgs->values;
            return toPython(self->sheet.parseColumn(val)).release();
        EXTERNAL_EPILOG
    }

    static auto parsePoint(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<String>>(args, {"parsePoint", {{"argument"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [val] = parsedArgs->values;
            return toPython(self->sheet.parsePoint(val)).release();
        EXTERNAL_EPILOG
    }

    static auto parseArea(SheetObject * self, PyObject * args) -> PyObject * {
        EXTERNAL_PROLOG
            auto parsedArgs = parseArguments<Typelist<String>>(args, {"parseArea", {{"argument"}}});
            if (!parsedArgs)
                return nullptr;
            auto & [val] = parsedArgs->values;
            return toPython(self->sheet.parseArea(val)).release();
        EXTERNAL_EPILOG
    }

private:
    static auto checkRow(SizeType row) -> bool {
        constexpr auto max = Sheet::maxSize().height;
        if (row >= max) {
            PyErr_SetString(PyExc_OverflowError, ("invalid row: " + std::to_string(row)).c_str());
            return false;
        }
        return true;
    }

    static auto checkColumn(SizeType column) -> bool {
        constexpr auto max = Sheet::maxSize().width;
        if (column >= max) {
            PyErr_SetString(PyExc_OverflowError, ("invalid column: " + std::to_string(column)).c_str());
            return false;
        }
        return true;
    }

    static auto checkRowRange(SizeType row, SizeType count) -> bool {
        constexpr auto max = Sheet::maxSize().height;
        if (count < 0 || count > max || max - count < row) {
            PyErr_SetString(PyExc_OverflowError, 
                ("invalid row range: (start:" + std::to_string(row) + ", len:" + std::to_string(count) + ")").c_str());
            return false;
        }
        return true;
    }

    static auto checkColumnRange(SizeType column, SizeType count) -> bool {
        constexpr auto max = Sheet::maxSize().width;
        if (count < 0 || count > max || max - count < column) {
            PyErr_SetString(PyExc_OverflowError, 
                ("invalid column range: (start:" + std::to_string(column) + ", len:" + std::to_string(count) + ")").c_str());
            return false;
        }
        return true;
    }
};

PyMethodDef SheetObject::methods[] = {
    {"size",                (PyCFunction)size,                  METH_NOARGS, nullptr},
    {"maxSize",             (PyCFunction)maxSize,               METH_STATIC | METH_NOARGS, nullptr},
    {"nonNullCellCount",    (PyCFunction)nonNullCellCount,      METH_NOARGS, nullptr},
    {"setValueCell",        (PyCFunction)setValueCell,          METH_VARARGS, nullptr},
    {"setFormulaCell",      (PyCFunction)setFormulaCell,        METH_VARARGS, nullptr},
    {"clearCellValue",      (PyCFunction)clearCellValue,        METH_VARARGS, nullptr},
    {"getValue",            (PyCFunction)getValue,              METH_VARARGS, nullptr},
    {"getEditInfo",         (PyCFunction)getEditInfo,           METH_VARARGS, nullptr},
    {"copyCell",            (PyCFunction)copyCell,              METH_VARARGS, nullptr},
    {"copyCells",           (PyCFunction)copyCells,             METH_VARARGS, nullptr},
    {"moveCell",            (PyCFunction)moveCell,              METH_VARARGS, nullptr},
    {"moveCells",           (PyCFunction)moveCells,             METH_VARARGS, nullptr},
    {"deleteRows",          (PyCFunction)deleteRows,            METH_VARARGS, nullptr},
    {"deleteColumns",       (PyCFunction)deleteColumns,         METH_VARARGS, nullptr},
    {"insertRows",          (PyCFunction)insertRows,            METH_VARARGS, nullptr},
    {"insertColumns",       (PyCFunction)insertColumns,         METH_VARARGS, nullptr},
    {"suspendRecalc",       (PyCFunction)suspendRecalc,         METH_NOARGS, nullptr},
    {"resumeRecalc",        (PyCFunction)resumeRecalc,          METH_NOARGS, nullptr},
    {"recalculate",         (PyCFunction)recalculate,           METH_NOARGS, nullptr},
    {"setRowsHeight",       (PyCFunction)setRowsHeight,         METH_VARARGS, nullptr},
    {"clearRowsHeight",     (PyCFunction)clearRowsHeight,       METH_VARARGS, nullptr},
    {"hideRows",            (PyCFunction)hideRows,              METH_VARARGS, nullptr},
    {"getRowHeight",        (PyCFunction)getRowHeight,          METH_VARARGS, nullptr},
    {"rowHeightGenerator",  (PyCFunction)rowHeightGenerator,    METH_VARARGS, nullptr},
    {"setColumnsWidth",     (PyCFunction)setColumnsWidth,       METH_VARARGS, nullptr},
    {"clearColumnsWidth",   (PyCFunction)clearColumnsWidth,     METH_VARARGS, nullptr},
    {"hideColumns",         (PyCFunction)hideColumns,           METH_VARARGS, nullptr},
    {"getColumnWidth",      (PyCFunction)getColumnWidth,        METH_VARARGS, nullptr},
    {"columnWidthGenerator",(PyCFunction)columnWidthGenerator,  METH_VARARGS, nullptr},
    {"indexToRow",          (PyCFunction)indexToRow,            METH_VARARGS, nullptr},
    {"indexToColumn",       (PyCFunction)indexToColumn,         METH_VARARGS, nullptr},
    {"parseRow",            (PyCFunction)parseRow,              METH_VARARGS, nullptr},
    {"parseColumn",         (PyCFunction)parseColumn,           METH_VARARGS, nullptr},
    {"parsePoint",          (PyCFunction)parsePoint,            METH_VARARGS, nullptr},
    {"parseArea",           (PyCFunction)parseArea,             METH_VARARGS, nullptr},
    {nullptr}  /* Sentinel */
};

PyTypeObject SheetObject::typeObject = [](){
    PyTypeObject ret = {PyVarObject_HEAD_INIT(nullptr, 0)};
    ret.tp_name = "_spreader.Sheet";
    ret.tp_basicsize = sizeof(SheetObject);
    ret.tp_itemsize = 0;
    ret.tp_dealloc = destructor(dealloc);
    ret.tp_flags = Py_TPFLAGS_DEFAULT;
    ret.tp_doc = nullptr;
    ret.tp_methods = methods;
    ret.tp_new = _new__;
    return ret;
}();


static PyMethodDef _spreaderMethods[] = {
    {nullptr}        /* Sentinel */
};

PyModuleDef _spreaderModule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "impl",
    .m_doc = nullptr,
    .m_size = -1,
    .m_methods = _spreaderMethods
};


#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC visibility push(default)
#endif

PyMODINIT_FUNC PyInit__spreader() {

    EXTERNAL_PROLOG

        auto typesMod = py_attach(PyImport_ImportModule("eg.spreader.types")); 
        if (!typesMod)
            return nullptr;

        g_errorValueClass = py_attach((PyTypeObject *)PyObject_GetAttr(typesMod.get(), SPRS("ErrorValue").py_str()));
        if (!g_errorValueClass) 
            return nullptr;
        if (!PyType_Check(g_errorValueClass.get())) {
            PyErr_SetString(PyExc_TypeError, "spreader.types.ErrorValue isn't a class");
            return nullptr;
        }

        auto mod = py_attach(PyModule_Create(&_spreaderModule));
        if (!mod )
            return nullptr;
        if (!SheetObject::registerType(mod))
            return nullptr;
        if (!LengthInfoGeneratorObject::registerType(mod))
            return nullptr;

        return mod.release();

    EXTERNAL_EPILOG
}

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC visibility pop
#endif

