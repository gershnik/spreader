# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

from dataclasses import dataclass
from typing import Optional, Union

@dataclass
class Point:
    '''
    Coordinate of a cell in a sheet.

    This is the "natural" format for cell coordinates in this library.
    All coordinates are returned using this type though for input you can use other formats. 
    See `CellCoord`
    '''

    x: int = 0
    y: int = 0

@dataclass
class Size:
    '''A size of an area in a sheet'''

    width: int = 0
    height: int = 0

@dataclass
class Rect:
    '''
    A rectangular area in a sheet.

    This is the "natural" format for areas in this library.
    All areas are returned using this type though for input you can use other formats. 
    See `AreaCoord`
    '''

    origin: Point = 0
    size: Size = 0

CellCoord = Union[str, Point]
'''
A rectangular area in a sheet.
 
This is the "natural" format for areas in this library.
All areas are returned using this type though for input you can
use other formats. See `AreaCoord`
'''

AreaCoord = Union[CellCoord, Rect]
'''
Possible ways to represent rectangular area coordinates in a sheet

An area can be passed to this library in the following ways:
- As a `CellCoord` indicating an area that consists of a single cell
'''

ColumnCoord = Union[str, int]
'''
Possible ways to represent columns in a sheet

A column can be passed to this library in the following ways:
- As a column index: 0-based numbers (0 for "A", 1 for "B" etc.)
- As a column name: strings "A", "B", "C" and so on
'''

RowCoord = Union[str, int]
'''
Possible ways to represent a row in a sheet

A row can be passed to this library in the following ways:
- As a row index: 0-based numbers
- As a row name: strings "1", "2", "3" and so on
'''

class ErrorValue:
    '''
    Error values in cells
    
    Errors can be produced as a result of formula calculations or manually
    set as a cell's value. These correspond to the usual spreadsheet errors 
    such as "#REF!" etc.
    '''
    
    _known = {}

    def __new__(cls, code: int, text: str = None):
        ret = ErrorValue._known.get(code, None)
        if ret is None:
            ret = super().__new__(cls)
            ret.value = code
            ret._text = text if not text is None else f"Unknown error: <{code}>"
            ErrorValue._known[code] = ret
        return ret

    def __eq__(self, __o: object) -> bool:
        if isinstance(__o, self.__class__):
            return self.value == __o.value
        return NotImplemented
        
    def __hash__(self) -> int:
        return self.value.__hash__()

    def __str__(self):
        return self._text


class Errors:
    '''
    Known errors
     
    The set of errors exposed by this class is stable but not closed. 
    Additional errors might be added in the future. Thus if you serialize sheet 
    data or interchange it with other libraries or even versions of this library 
    you should be prepared to deal with `ErrorValue`s not defined here.
    '''
    
    NullRange        = ErrorValue( 1, '#NULL!')
    '''Represents `#NULL!` error'''
    DivisionByZero   = ErrorValue( 2, '#DIV/0!')
    '''Represents `#DIV/0!` error'''
    InvalidValue     = ErrorValue( 3, '#VALUE!')
    '''Represents `#VALUE!` error'''
    InvalidReference = ErrorValue( 4, '#REF!') 
    '''Represents `#REF!` error'''
    InvalidName      = ErrorValue( 5, '#NAME?')
    '''Represents `#NAME?` error'''
    NotANumber       = ErrorValue( 6, '#NUM!')
    '''Represents `#NUM!` error'''
    InvalidArgs      = ErrorValue( 7, '#N/A')
    '''Represents `#N/A` error'''
    GettingData      = ErrorValue( 8, '#GETTING_DATA')
    '''Represents `#GETTING_DATA` error'''
    Spill            = ErrorValue( 9, '#SPILL!') 
    '''Represents `#SPILL!` error'''
    InvalidFormula   = ErrorValue(10, '#ERROR!')
    '''
    Represents an error in parsing a formula.

    This is not an Excel compatible error.
    '''
        

Scalar = Union[None, bool, float, str, ErrorValue]
'''
Possible values of a single cell

`None` stands for a blank cell
'''

@dataclass
class FormulaInfo:
    '''Information about formula in a cell'''

    text: str
    '''Formula text'''
    extent: Size
    '''
    Extent of the formula's dynamic array, if any.
    
    If the formula is not an array the size is Size(1, 1)
    '''

@dataclass
class EditInfo:
    '''
    Information about cell content needed for editing it
    
    Currently only one field is defined: formula which describes
    whether the cell contains a formula and information about it.
    Additional fields will be added in the future
    '''

    formula: Optional[FormulaInfo] = None
    '''If present, indicates that the cell contains formula '''

@dataclass
class LengthInfo:
    hidden: bool
    length: Optional[int] = None

@dataclass
class LengthInfoRange:
    startIdx: int
    endIdx: int
    info: LengthInfo

    