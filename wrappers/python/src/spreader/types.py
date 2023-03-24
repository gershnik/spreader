# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

from dataclasses import dataclass
from typing import Optional, Union

@dataclass
class Size:
    width: int = 0
    height: int = 0

@dataclass
class Point:
    x: int = 0
    y: int = 0

@dataclass
class Rect:
    origin: Point = 0
    size: Size = 0

CellCoord = Union[str, Point]
AreaCoord = Union[CellCoord, Rect]


class ErrorValue:
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
    NullRange        = ErrorValue( 1, '#NULL!')
    DivisionByZero   = ErrorValue( 2, '#DIV/0!')
    InvalidValue     = ErrorValue( 3, '#VALUE!')
    InvalidReference = ErrorValue( 4, '#REF!') 
    InvalidName      = ErrorValue( 5, '#NAME?')
    NotANumber       = ErrorValue( 6, '#NUM!')
    InvalidArgs      = ErrorValue( 7, '#N/A')
    GettingData      = ErrorValue( 8, '#N/A')
    Spill            = ErrorValue( 9, '#SPILL!') 
    InvalidFormula   = ErrorValue(10, '#ERROR!')
        

Scalar = Union[None, bool, float, int, str, ErrorValue]

@dataclass
class FormulaInfo:
    text: str
    extent: Size

@dataclass
class EditInfo:
    formula: Optional[FormulaInfo] = None

@dataclass
class LengthInfo:
    hidden: bool
    length: Optional[int] = None

@dataclass
class LengthInfoRange:
    startIdx: int
    endIdx: int
    info: LengthInfo

    