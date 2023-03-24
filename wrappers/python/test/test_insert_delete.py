from spreader import Sheet, Size, FormulaInfo
from .utils import strictEqual
import pytest

def test_deleteRowsInEmptySheet():
    s = Sheet()

    try: 
        s.deleteRows(0, 0)
    except:
        assert False
    assert strictEqual(s.size(), Size(0, 0))
    try:
        s.deleteRows(0, 5)
    except:
        assert False
    assert strictEqual(s.size(), Size(0, 0))

def test_deleteExtraRows():
    s = Sheet()

    s.setFormulaCell("A1", "A20 + 3")
    s.deleteRows(5, 4)
    assert strictEqual(s.size(), Size(1, 1))
    assert strictEqual(s.getEditInfo("A1").formula, FormulaInfo("A16 + 3", Size(1, 1)))

def test_deleteFurthestRows():
    s = Sheet()

    s.setFormulaCell("A2147483647", "B2147483647 + 3")
    assert strictEqual(s.size(), Size(1, 2147483647))
    s.deleteRows("1", 2147483647)
    assert strictEqual(s.size(), Size(1, 0))

    s.setFormulaCell("A2147483647", "B2147483647 + 3")
    s.deleteRows("1", 2147483646)
    assert strictEqual(s.getEditInfo("A1").formula, FormulaInfo("B1 + 3", Size(1, 1)))

def test_deleteColumnsInEmptySheet():
    s = Sheet()

    try: 
        s.deleteColumns(0, 0)
    except:
        assert False
    assert strictEqual(s.size(), Size(0, 0))
    try:
        s.deleteColumns(0, 5)
    except:
        assert False
    assert strictEqual(s.size(), Size(0, 0))

def test_deleteExtraColumns():
    s = Sheet()

    s.setFormulaCell("A1", "T1 + 3")
    s.deleteColumns(5, 4)
    assert strictEqual(s.size(), Size(1, 1))
    assert strictEqual(s.getEditInfo("A1").formula, FormulaInfo("P1 + 3", Size(1, 1)))


def test_deleteFurthestColumns():
    s = Sheet()

    s.setFormulaCell("CRXO1", "CRXO2 + 3")
    assert strictEqual(s.size(), Size(65535, 1))
    s.deleteColumns("A", 65535)
    assert strictEqual(s.size(), Size(0, 1))

    s.setFormulaCell("CRXO1", "CRXO2 + 3")
    s.deleteColumns("A", 65534)
    assert strictEqual(s.getEditInfo("A1").formula, FormulaInfo("A2 + 3", Size(1, 1)))

def test_insertRowsInEmptySheet():
    s = Sheet()

    s.insertRows(0, 1)
    assert strictEqual(s.size(), Size(0, 1))
    s.deleteRows(0, 1)

    s.insertRows("1", 15)
    assert strictEqual(s.size(), Size(0, 15))
    s.deleteRows(0, 15)

    s.insertRows("5", 15)
    assert strictEqual(s.size(), Size(0, 19))
    s.deleteRows(0, 20)

def test_insertFurthestRows():
    s = Sheet()

    s.insertRows(2147483646, 1)
    assert strictEqual(s.size(), Size(0, 2147483647))
    s.deleteRows(0, 2147483647)

    with pytest.raises(OverflowError):
        s.insertRows(2147483647, 1)

    s.setValueCell("A2147483646", 1)
    s.insertRows(2147483646, 1)
    assert strictEqual(s.size(), Size(1, 2147483647))

def test_insertColumnsInEmptySheet():
    s = Sheet()

    s.insertColumns(0, 1)
    assert strictEqual(s.size(), Size(1, 0))
    s.deleteColumns(0, 1)

    s.insertColumns("A", 15)
    assert strictEqual(s.size(), Size(15, 0))
    s.deleteColumns(0, 15)

    s.insertColumns("E", 15)
    assert strictEqual(s.size(), Size(19, 0))
    s.deleteColumns(0, 20)

def test_insertFurthestColumns():
    s = Sheet()

    s.insertColumns(65534, 1)
    assert strictEqual(s.size(), Size(65535, 0))
    s.deleteColumns(0, 65535)

    with pytest.raises(OverflowError):
        s.insertColumns(65535, 1)

    s.setValueCell("CRXN1", 1)
    s.insertColumns(65534, 1)
    assert strictEqual(s.size(), Size(65535, 1))

