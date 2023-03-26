# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

from ._spreader import (
    Sheet as SheetImpl
)
from .types import *

from typing import Tuple, Optional, Iterator

class MetaSheet(type):
    @property
    def maxSize(cls):
        return cls._maxSize()

class Sheet(object, metaclass=MetaSheet):
    _maxSizeValue = None

    def __init__(self):
        self._impl = SheetImpl()

    def size(self) -> Size :
        res = self._impl.size()
        return Size(width=res[0], height=res[1])

    @staticmethod
    def _maxSize() -> Size :
        if Sheet._maxSizeValue is None:
            res = SheetImpl.maxSize()
            Sheet._maxSizeValue = Size(width=res[0], height=res[1])
        return Sheet._maxSizeValue
    
    @property
    def maxSize(self):
        return Sheet._maxSize()
    
    def nonNullCellCount(self) -> int :
        return self._impl.nonNullCellCount()
    
    def setValueCell(self, coord: CellCoord, value: Scalar) -> None:
        self._impl.setValueCell(self._unpackPoint(coord), value)

    def setFormulaCell(self, coord: CellCoord, formula: str) -> None:
        self._impl.setFormulaCell(self._unpackPoint(coord), formula)

    def clearCellValue(self, coord: CellCoord) -> None:
        self._impl.clearCellValue(self._unpackPoint(coord))

    def getValue(self, coord: CellCoord) -> Scalar:
        return self._impl.getValue(self._unpackPoint(coord))
    
    def getEditInfo(self, coord: CellCoord) -> EditInfo:
        formula, = self._impl.getEditInfo(self._unpackPoint(coord))
        if formula is None:
            return EditInfo()
        text, (width, height) = formula
        return EditInfo(formula = FormulaInfo(text, Size(width, height)))
    
    def copyCell(self, frm: CellCoord, to: AreaCoord) -> None :
        self._impl.copyCell(self._unpackPoint(frm), self._unpackArea(to))
    
    def copyCells(self, frm: AreaCoord, to: CellCoord) -> None :
        self._impl.copyCells(self._unpackArea(frm), self._unpackPoint(to))

    def moveCell(self, frm: CellCoord, to: CellCoord) -> None :
        self._impl.moveCell(self._unpackPoint(frm), self._unpackPoint(to))
    
    def moveCells(self, frm: AreaCoord, to: CellCoord) -> None :
        self._impl.moveCells(self._unpackArea(frm), self._unpackPoint(to))

    def deleteRows(self, y: RowCoord, count: int) -> None:
        row = self._unpackRow(y)
        self._impl.deleteRows(row, count)

    def deleteColumns(self, x: ColumnCoord, count: int) -> None:
        col = self._unpackColumn(x)
        self._impl.deleteColumns(col, count)

    def insertRows(self, y: RowCoord, count: int) -> None:
        row = self._unpackRow(y)
        self._impl.insertRows(row, count)

    def insertColumns(self, x: ColumnCoord, count: int) -> None:
        col = self._unpackColumn(x)
        self._impl.insertColumns(col, count)

    def suspendRecalc(self) -> None:
        self._impl.suspendRecalc()

    def resumeRecalc(self) -> None:
        self._impl.resumeRecalc()

    def recalculate(self) -> None:
        self._impl.recalculate()

    def setRowsHeight(self, y: RowCoord, count: int, height: int) -> None:
        row = self._unpackRow(y)
        self._impl.setRowsHeight(row, count, height)

    def clearRowsHeight(self, y: RowCoord, count: int) -> None:
        row = self._unpackRow(y)
        self._impl.clearRowsHeight(row, count)

    def hideRows(self, y: RowCoord, count: int, hidden: bool) -> None:
        row = self._unpackRow(y)
        self._impl.hideRows(row, count, hidden)

    def getRowHeight(self, y: RowCoord) -> LengthInfo:
        row = self._unpackRow(y)
        ret = self._impl.getRowHeight(row)
        return LengthInfo(length=ret[0], hidden=ret[1])
    
    def rowHeightRanges(self, y: RowCoord, count: int) -> Iterator[LengthInfoRange]:
        row = self._unpackRow(y)
        return self._adaptLengthInfoRangeGenerator(self._impl.rowHeightGenerator(row, count))
    
    def setColumnsWidth(self, x: ColumnCoord, count: int, width: int) -> None:
        col = self._unpackColumn(x)
        self._impl.setColumnsWidth(col, count, width)

    def clearColumnsWidth(self, x: ColumnCoord, count: int) -> None:
        col = self._unpackColumn(x)
        self._impl.clearColumnsWidth(col, count)

    def hideColumns(self, x: ColumnCoord, count: int, hidden: bool) -> None:
        col = self._unpackColumn(x)
        self._impl.hideColumns(col, count, hidden)

    def getColumnWidth(self, x: ColumnCoord) -> LengthInfo:
        col = self._unpackColumn(x)
        ret = self._impl.getColumnWidth(col)
        return LengthInfo(length=ret[0], hidden=ret[1])
    
    def columnWidthRanges(self, x: ColumnCoord, count: int) -> Iterator[LengthInfoRange]:
        col = self._unpackColumn(x)
        return self._adaptLengthInfoRangeGenerator(self._impl.columnWidthGenerator(col, count))

    def indexToRow(self, y: int) -> str:
        return self._impl.indexToRow(y)
    
    def indexToColumn(self, x: int) -> str:
        return self._impl.indexToColumn(x)
    
    def parseRow(self, val:str) -> Optional[int]:
        return self._impl.parseRow(val)
    
    def parseColumn(self, val:str) -> Optional[int]:
        return self._impl.parseColumn(val)

    def parsePoint(self, val:str) -> Optional[Point]:
        res = self._impl.parsePoint(val)
        return Point(res[0], res[1]) if not res is None else None
    
    def parseArea(self, val:str) -> Optional[Rect]:
        res = self._impl.parseArea(val)
        return Rect(Point(res[0], res[1]), Size(res[2], res[3])) if not res is None else None

    def _unpackRow(self, row: RowCoord) -> int:
        if isinstance(row, str):
            ret = self._impl.parseRow(row)
            if ret is None:
                raise ValueError(f"invalid row: {row}")
            return ret
        return row
    
    def _unpackColumn(self, column: ColumnCoord) -> int:
        if isinstance(column, str):
            ret = self._impl.parseColumn(column)
            if ret is None:
                raise ValueError(f"invalid column: {column}")
            return ret
        return column

    def _unpackPoint(self, coord: CellCoord) -> Tuple[int, int]:
        if isinstance(coord, str):
            ret = self._impl.parsePoint(coord)
            if ret is None:
                raise ValueError(f"invalid point reference: {coord}")
            return ret
        if isinstance(coord, Point):
            return (coord.x, coord.y)
        if isinstance(coord, tuple):
            return coord  #let imple check validity
        if isinstance(coord, list):
            if len(coord) == 2:
                return (coord[0], coord[1])
            raise TypeError(f"point coordinate list must be of length 2: {coord}")
        raise TypeError(f"invalid point coordinate: {coord}")
    
    def _unpackArea(self, coord: AreaCoord) -> Tuple[int, int, int, int]:
        if isinstance(coord, str):
            ret = self._impl.parseArea(coord)
            if ret is None:
                raise ValueError(f"invalid area reference: {coord}")
            return ret
        if isinstance(coord, Rect):
            return (coord.origin.x, coord.origin.y, coord.size.width, coord.size.height)
        if isinstance(coord, Point):
            return (coord.x, coord.y) + (1, 1)
        if isinstance(coord, tuple):
            if len(coord) == 4:
                return coord
            elif len(coord) == 2:
                return coord + (1, 1)
            raise TypeError(f"area coordinate tuple must be of length 2 or 4: {coord}")
        if isinstance(coord, list):
            if len(coord) == 4:
                return (coord[0], coord[1], coord[2], coord[3])
            elif len(coord) == 2:
                return (coord[0], coord[1], 1, 1)
            raise TypeError(f"area coordinate list must be of length 2 or 4: {coord}")
        raise TypeError(f"invalid area coordinate: {coord}")
    
    def _adaptLengthInfoRangeGenerator(self, impl):
        for startIdx, endIdx, (length, hidden) in impl:
            yield LengthInfoRange(startIdx, endIdx, LengthInfo(length=length, hidden=hidden))
    