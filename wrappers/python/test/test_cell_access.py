
from spreader import Sheet, Point, Size, EditInfo, FormulaInfo, Errors, ErrorValue
from .utils import strictEqual
import math
import pytest

def test_returnsNoneForUnsetValue():
    s = Sheet()
    for coord in ((0, 0), Point(x=0, y=0), "A1"):
        assert strictEqual(s.getValue(coord), None)

def test_returnsPreviouslySetValue():
    
    s = Sheet()
    for coord in ((0, 0), [0, 0], Point(x=0, y=0), "A1", "CRXO2147483647"):
        
        s.setValueCell(coord, None)
        assert strictEqual(s.getValue(coord), None)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 0

        s.setValueCell(coord, True)
        assert strictEqual(s.getValue(coord), True)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, False)
        assert strictEqual(s.getValue(coord), False)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, 1)
        assert strictEqual(s.getValue(coord), 1.)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, 2.5)
        assert strictEqual(s.getValue(coord), 2.5)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        veryBigInt = int(10) ** 320
        with pytest.raises(OverflowError):
            s.setValueCell(coord, veryBigInt)
        assert strictEqual(s.getValue(coord), 2.5)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, math.inf)
        assert strictEqual(s.getValue(coord), Errors.NotANumber)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, -math.inf)
        assert strictEqual(s.getValue(coord), Errors.NotANumber)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, -math.nan)
        assert strictEqual(s.getValue(coord), Errors.NotANumber)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, '')
        assert strictEqual(s.getValue(coord), '')
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, 'abc')
        assert strictEqual(s.getValue(coord), 'abc')
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, Errors.InvalidValue)
        assert strictEqual(s.getValue(coord), Errors.InvalidValue)
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setValueCell(coord, ErrorValue(256))
        assert strictEqual(s.getValue(coord), ErrorValue(256))
        assert s.getEditInfo(coord) == EditInfo()
        assert s.nonNullCellCount() == 1

        s.setFormulaCell(coord, "1 + 3")
        assert strictEqual(s.getValue(coord), 4.)
        assert s.getEditInfo(coord) == EditInfo(FormulaInfo("1 + 3", Size(1, 1)))
        assert s.nonNullCellCount() == 1

        s.setFormulaCell(coord, "")
        assert strictEqual(s.getValue(coord), Errors.InvalidFormula)
        assert s.getEditInfo(coord) == EditInfo(FormulaInfo("", Size(1, 1)))
        assert s.nonNullCellCount() == 1

        s.setFormulaCell(coord, "C1:D2 + 3")
        assert strictEqual(s.getValue(coord), 3.)
        if coord != "CRXO2147483647":
            assert s.getEditInfo(coord) == EditInfo(FormulaInfo("C1:D2 + 3", Size(2, 2)))
            assert s.nonNullCellCount() == 4
        else:
            assert s.getEditInfo(coord) == EditInfo(FormulaInfo("C1:D2 + 3", Size(1, 1)))
            assert s.nonNullCellCount() == 1

        s.clearCellValue(coord)
        
def test_clearsValueProperly():
    s = Sheet()
    for coord in ((0, 0), Point(x=0, y=0), "A1", "CRXO2147483647"):
        s.setValueCell(coord, 42.)
        s.clearCellValue(coord)
        assert strictEqual(s.getValue(coord), None)
        assert s.nonNullCellCount() == 0

        s.setFormulaCell(coord, "1 + 3")
        s.clearCellValue(coord)
        assert strictEqual(s.getValue(coord), None)
        assert s.nonNullCellCount() == 0
    
def test_throwsExceptionForInvalidValueType():
    s = Sheet()
    with pytest.raises(TypeError):
        s.setValueCell((0, 0), (1,))
    with pytest.raises(TypeError):
        s.setValueCell((0, 0), [1])
    with pytest.raises(TypeError):
        s.setValueCell((0, 0), {1:1})
    with pytest.raises(TypeError):
        s.setFormulaCell((0, 0), (1,))
    with pytest.raises(TypeError):
        s.setFormulaCell((0, 0), [1])


def test_throwsExceptionOnOutOfRangeAccess():
    s = Sheet()
    def helper(accessor):
        with pytest.raises(OverflowError):
            accessor((-1, 0))
        with pytest.raises(OverflowError):
            accessor((0, -1))
        with pytest.raises(OverflowError):
            accessor((65535, 0))
        with pytest.raises(OverflowError):
            accessor(Point(x=65535, y=0))
        with pytest.raises(ValueError):
            accessor("CRXP1")
        with pytest.raises(OverflowError):
            accessor((0, 2147483647))
        with pytest.raises(OverflowError):
            accessor(Point(x=0, y=2147483647))
        with pytest.raises(ValueError):
            accessor("A2147483649")
    helper(lambda coord: s.getValue(coord))
    helper(lambda coord: s.getEditInfo(coord))
    helper(lambda coord: s.setValueCell(coord, 1))
    helper(lambda coord: s.clearCellValue(coord))
    helper(lambda coord: s.setFormulaCell(coord, "1 + 1"))

def test_throwsExceptionOnBogusCoordinateType():
    s = Sheet()

    def helper(accessor):
        with pytest.raises(TypeError):
            accessor(3)
        with pytest.raises(TypeError):
            accessor([1])
        with pytest.raises(TypeError):
            accessor((1, 2, 3))
        with pytest.raises(TypeError):
            accessor(None)
        with pytest.raises(TypeError):
            accessor({1:1})
        with pytest.raises(TypeError):
            accessor((1.2, 0))
        with pytest.raises(TypeError):
            accessor((0, 1.2))
    helper(lambda coord: s.getValue(coord))
    helper(lambda coord: s.getEditInfo(coord))
    helper(lambda coord: s.setValueCell(coord, 1))
    helper(lambda coord: s.clearCellValue(coord))
    helper(lambda coord: s.setFormulaCell(coord, "1 + 1"))
        
        
        

