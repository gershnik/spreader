from eg.spreader import Sheet, LengthInfo, LengthInfoRange
from .utils import strictEqual
import pytest

def lengthRangeStr(range: LengthInfoRange):
    ret = f'[{range.startIdx}, {range.endIdx}, '
    if not range.info.length is None:
        ret += f'{range.info.length}, '
    ret += f'{range.info.hidden}]'
    return ret

def test_setAndGetRowHeight():
    s = Sheet()

    assert strictEqual(s.getRowHeight(3), LengthInfo(hidden=False))

    s.setRowsHeight(3, 1, 22)
    assert strictEqual(s.getRowHeight(3), LengthInfo(length=22, hidden=False))
    assert strictEqual(s.getRowHeight("4"), LengthInfo(length=22, hidden=False))
    
    s.setRowsHeight("5", 2, 13)
    assert strictEqual(s.getRowHeight(4), LengthInfo(length=13, hidden=False))
    assert strictEqual(s.getRowHeight("5"), LengthInfo(length=13, hidden=False))

    s.clearRowsHeight(2, 2)
    assert strictEqual(s.getRowHeight(2), LengthInfo(hidden=False))

    s.hideRows(2, 2, True)
    assert strictEqual(s.getRowHeight(2), LengthInfo(hidden=True))

    s.hideRows(2, 2, False)
    assert strictEqual(s.getRowHeight(2), LengthInfo(hidden=False))

    s.hideRows(4, 1, True)
    assert strictEqual(s.getRowHeight(4), LengthInfo(length=13, hidden=True))

    buf = ""
    for range in s.rowHeightRanges(0, 7):
        buf += lengthRangeStr(range)
    assert buf == "[0, 4, False][4, 5, 13, True][5, 6, 13, False][6, 7, False]"

    buf = ""
    for range in s.rowHeightRanges(0, 7):
        buf += lengthRangeStr(range)
    assert buf == "[0, 4, False][4, 5, 13, True][5, 6, 13, False][6, 7, False]"


def test_handlingOfRowHeightBoundaries():
    s = Sheet()

    with pytest.raises(OverflowError):
        s.setRowsHeight(0, -1, 13)
    with pytest.raises(OverflowError):
        s.setRowsHeight(0, 1, 2**33)

    with pytest.raises(TypeError):
        s.setRowsHeight(0, 1, "hello")


def test_setAndGetColumnWidth():
    s = Sheet()

    assert strictEqual(s.getColumnWidth(3), LengthInfo(hidden=False))

    s.setColumnsWidth(3, 1, 22)
    assert strictEqual(s.getColumnWidth(3), LengthInfo(length=22, hidden=False))
    assert strictEqual(s.getColumnWidth("D"), LengthInfo(length=22, hidden=False))
    
    s.setColumnsWidth("E", 2, 13)
    assert strictEqual(s.getColumnWidth(4), LengthInfo(length=13, hidden=False))
    assert strictEqual(s.getColumnWidth("E"), LengthInfo(length=13, hidden=False))

    s.clearColumnsWidth(2, 2)
    assert strictEqual(s.getColumnWidth(2), LengthInfo(hidden=False))

    s.hideColumns(2, 2, True)
    assert strictEqual(s.getColumnWidth(2), LengthInfo(hidden=True))

    s.hideColumns(2, 2, False)
    assert strictEqual(s.getColumnWidth(2), LengthInfo(hidden=False))

    s.hideColumns(4, 1, True)
    assert strictEqual(s.getColumnWidth(4), LengthInfo(length=13, hidden=True))

    buf = ""
    for range in s.columnWidthRanges(0, 7):
        buf += lengthRangeStr(range)
    assert buf == "[0, 4, False][4, 5, 13, True][5, 6, 13, False][6, 7, False]"

    buf = ""
    for range in s.columnWidthRanges(0, 7):
        buf += lengthRangeStr(range)
    assert buf == "[0, 4, False][4, 5, 13, True][5, 6, 13, False][6, 7, False]"

def test_handlingOfColumnWidthBoundaries():
    s = Sheet()

    with pytest.raises(OverflowError):
        s.setColumnsWidth(0, -1, 13)
    with pytest.raises(OverflowError):
        s.setColumnsWidth(0, 1, 2**33)

    with pytest.raises(TypeError):
        s.setColumnsWidth(0, 1, "hello")