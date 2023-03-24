from spreader import Sheet, Point, Size, Rect
import pytest

def test_indexToRow():
    s = Sheet()

    assert s.indexToRow(0) == "1"
    assert s.indexToRow(99) == "100"
    with pytest.raises(OverflowError):
        s.indexToRow(2147483647)
    with pytest.raises(OverflowError):
        s.indexToRow(-1)

def test_indexToColumn():
    s = Sheet()

    assert s.indexToColumn(0) == "A"
    assert s.indexToColumn(25) == "Z"
    assert s.indexToColumn(26) == "AA"
    with pytest.raises(OverflowError):
        s.indexToColumn(65536)
    with pytest.raises(OverflowError):
        s.indexToColumn(-1)

def test_parseRow():
    s = Sheet()

    assert s.parseRow("1") == 0
    assert s.parseRow("100") == 99

    assert s.parseRow("1A") is None
    assert s.parseRow("") is None
    assert s.parseRow("123456789899") is None

def test_parseColumn():
    s = Sheet()

    assert s.parseColumn("A") == 0
    assert s.parseColumn("Z") == 25
    assert s.parseColumn("AA") == 26

    assert s.parseColumn("1A") is None
    assert s.parseColumn("") is None
    assert s.parseColumn("XYZHH") is None

def test_parsePoint():
    s = Sheet()

    assert s.parsePoint("A1") == Point(0, 0)
    assert s.parsePoint("AA100") == Point(26, 99)

    assert s.parsePoint("") is None
    assert s.parsePoint("1A") is None

def test_parseArea():
    s = Sheet()

    assert s.parseArea("A1") == Rect(Point(0, 0), Size(1, 1))
    assert s.parseArea("A1:AA100") == Rect(Point(0, 0), Size(27, 100))

    assert s.parseArea("") is None
    assert s.parseArea("A1:") is None