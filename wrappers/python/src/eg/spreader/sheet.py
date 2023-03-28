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
    '''A spreadsheet'''
    
    _maxSizeValue = None

    def __init__(self):
        self._impl = SheetImpl()

    def size(self) -> Size :
        '''The size of the sheet'''
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
        '''Maximum possible size of a sheet'''
        return Sheet._maxSize()
    
    def nonNullCellCount(self) -> int :
        '''
        Returns the number of non-null cells in the sheet.

        This is mainly a debugging/troubleshooting aid.
        '''
        return self._impl.nonNullCellCount()
    
    def setValueCell(self, coord: CellCoord, value: Scalar) -> None:
        '''
        Stores a scalar value (rather than a formula) in a cell.
        
        If the `value` is `None` this is equivalent to `clearCellValue`
        When `value` is an `str` it should never be escaped. This function
        stores the argument as-is and does no input parsing.
        '''
        self._impl.setValueCell(self._unpackPoint(coord), value)

    def setFormulaCell(self, coord: CellCoord, formula: str) -> None:
        '''
        Stores a formula (rather than a scalar value) in a cell.

        There is no error if the formula fails to parse. Instead it is 
        stored in the cell and evaluates to `Errors.InvalidFormula`
        '''
        self._impl.setFormulaCell(self._unpackPoint(coord), formula)

    def clearCellValue(self, coord: CellCoord) -> None:
        '''
        Clears contents of the cell

        This erases cell content (whether a value or a formula) and sets
        the cell value to `None`. It is equivalent to `setValueCell(coord, None)`.
        '''
        self._impl.clearCellValue(self._unpackPoint(coord))

    def getValue(self, coord: CellCoord) -> Scalar:
        '''
        Retrieves cell's value

        For value cells the value is the one stored in it. For formula cells it is
        the evaluated result.
        '''
        return self._impl.getValue(self._unpackPoint(coord))
    
    def getEditInfo(self, coord: CellCoord) -> EditInfo:
        '''
        Retrieves information about cell of interest when editing it.

        This roughly corresponds to the information spreadsheet UI would need
        when user selects a cell e.g. presence of formula and its text, extent
        of array formula etc.
        This call is relatively expensive and is expected to be called "rarely"
        on one cell at a time rather than in bulk.
        '''
        formula, = self._impl.getEditInfo(self._unpackPoint(coord))
        if formula is None:
            return EditInfo()
        text, (width, height) = formula
        return EditInfo(formula = FormulaInfo(text, Size(width, height)))
    
    def copyCell(self, src: CellCoord, dst: AreaCoord) -> None :
        '''
        Copies a single cell to a destination area

        This is equivalent to copy/paste operation in GUI spreadsheets.
        The destination area can, of course, be a single cell
        '''
        self._impl.copyCell(self._unpackPoint(src), self._unpackArea(dst))
    
    def copyCells(self, src: AreaCoord, dst: CellCoord) -> None :
        '''
        Copies an area of cells to another location.

        The `dst` argument is the top left corner of destination area.
        This is equivalent to copy/paste operation in GUI spreadsheets.
        The source are can, of course, be a single cell in which case
        this call is equivalent to `copyCell(src ,dst)`
        '''
        self._impl.copyCells(self._unpackArea(src), self._unpackPoint(dst))

    def moveCell(self, src: CellCoord, dst: CellCoord) -> None :
        '''
        Moves a single cell to another location.

        This is equivalent to cut/paste operation in GUI spreadsheets.
        '''
        self._impl.moveCell(self._unpackPoint(src), self._unpackPoint(dst))
    
    def moveCells(self, src: AreaCoord, dst: CellCoord) -> None :
        '''
        Moves an area of cells to another location

        The `dst` argument is the top left corner of destination area.
        This is equivalent to cut/paste operation in GUI 
        The source are can, of course, be a single cell i
        this call is equivalent to `moveCell(from ,to)`
        '''
        self._impl.moveCells(self._unpackArea(src), self._unpackPoint(dst))

    def deleteRows(self, y: RowCoord, count: int) -> None:
        '''
        Deletes rows from the sheet

        Args:
            y: row to start deletion from
            count: number of rows to delete. 0 is a valid value and results in a no-op
        '''
        row = self._unpackRow(y)
        self._impl.deleteRows(row, count)

    def deleteColumns(self, x: ColumnCoord, count: int) -> None:
        '''
        Deletes columns from the sheet

        Args:
            x: column to start deletion from
            count: number of columns to delete. 0 is a valid value and results in a no-op
        '''
        col = self._unpackColumn(x)
        self._impl.deleteColumns(col, count)

    def insertRows(self, y: RowCoord, count: int) -> None:
        '''
        Inserts new rows into the sheet

        Args:
            y: coordinate of the row **before** which to perform the insertion
            count: number of rows to insert. 0 is a valid value and results in a no-op
        '''
        row = self._unpackRow(y)
        self._impl.insertRows(row, count)

    def insertColumns(self, x: ColumnCoord, count: int) -> None:
        '''
        Inserts new columns into the sheet

        Args:
            x: coordinate of the column **before** which to perform the insertion
            count: number of columns to insert. 0 is a valid value and results in a no-op
        '''
        col = self._unpackColumn(x)
        self._impl.insertColumns(col, count)

    def suspendRecalc(self) -> None:
        '''
        Suspends automatic recalculation of the sheet

        You call this method multiple times. To undo the suspension
        call `resumeRecalc` the same number of times. 
        '''
        self._impl.suspendRecalc()

    def resumeRecalc(self) -> None:
        '''
        Resumes automatic recalculation of the sheet

        This is the reverse of `suspendRecalc`
        '''
        self._impl.resumeRecalc()

    def recalculate(self) -> None:
        '''
        Manually recalculates the sheet

        This method allows you to manually recalculate the sheet when automatic 
        recalculation is suspended by a call to `suspendRecalc`
        '''
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
        '''
        Converts row index to a row name

        Currently this function exists mainly for symmetry with
        `indexToColumn`. It converts 0 to "1", 1 to "2" and so on
        which you can easily do yourself.
        However, in the future, the library might gain ability to give
        friendly names to rows in a sheet. In this case, this method will
        become necessary.

        Args:
            y: 0-based row index
        '''
        return self._impl.indexToRow(y)
    
    def indexToColumn(self, x: int) -> str:
        '''
        Converts column index to a column name

        0 is converted to "A", 1 to "B" and so on.

        Args:
            x: 0-based column index
        '''
        return self._impl.indexToColumn(x)
    
    def parseRow(self, val:str) -> Optional[int]:
        '''
        Converts row names to indices, if possible

        Currently this function exists mainly for symmetry with
        `parseColumn`. It converts "1" to 0, "2" to 1 and so on
        which you can easily do yourself.
        However, in the future, the library might gain ability to give
        friendly names to rows in a sheet. In this case, this method will
        become necessary.

        Args:
            val: row name like "1" or "5"
        Returns:
            corresponding row index if the name is valid or `None` otherwise
        '''
        return self._impl.parseRow(val)
    
    def parseColumn(self, val:str) -> Optional[int]:
        '''
        Converts column names to indices, if possible

        Args:
            val: column name like "A" or "AB"
        Returns:
            corresponding column index if the name is valid or `None` otherwise
        '''
        return self._impl.parseColumn(val)

    def parsePoint(self, val:str) -> Optional[Point]:
        '''
        Converts string cell coordinates to instances of `Point`, if possible

        Args:
            val: cell coordinate like "A1" or "B5"
        Returns:
            corresponding `Point` if the input is valid or `None` otherwise
        '''
        res = self._impl.parsePoint(val)
        return Point(res[0], res[1]) if not res is None else None
    
    def parseArea(self, val:str) -> Optional[Rect]:
        '''
        Converts string area coordinates to instances of `Rect`, if possible

        Note that an area could be a single cell like "A1", in which case the output will
        be a `Rect` of size `Size(1, 1)`, or a true area like "A1:B2".

        Args:
            val: area coordinate like "A1:C3" or "B5"
        Returns:
            corresponding `Rect` if the input is valid or `None` otherwise
        '''
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
    