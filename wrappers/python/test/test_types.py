#! /usr/bin/env python3

from spreader import Sheet, Size, Point, Errors, ErrorValue
from .utils import strictEqual


def test_Size():
    s = Size(3,4)
    assert str(s) == 'Size(width=3, height=4)'
    assert s == Size(3, 4)
    assert s != Point(3, 4)

def test_Point():
    s = Point(3,4)
    assert str(s) == 'Point(x=3, y=4)'
    assert s == Point(3, 4)
    assert s != Size(3, 4)

def test_ErrorValue():
    assert str(Errors.DivisionByZero) == '#DIV/0!'
    assert str(ErrorValue(256)) == 'Unknown error: <256>'
    assert ErrorValue(6) is Errors.NotANumber
    assert ErrorValue(6) == Errors.NotANumber
    assert ErrorValue(6) != Errors.NullRange

