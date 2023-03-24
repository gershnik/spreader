from spreader import Sheet, Point, Size, Rect
from .utils import strictEqual
import pytest

def test_copySingleCellToASingleCell():
    s = Sheet()
    for src in [(0, 0), Point(x=0, y=0), "A1"]:
        s.setValueCell(src, f"{src}")
        for dst in [
                (65534, 2147483646), Point(x=65534, y=2147483646), "CRXO2147483647",
                (65534, 2147483646, 1 ,1), Rect(origin=Point(65534, 2147483646), size = Size(1, 1)), 
                "CRXO2147483647:CRXO2147483647"]:
            s.copyCell(src, dst)
            assert strictEqual(s.getValue(s._unpackArea(dst)[:2]), f"{src}")
        assert strictEqual(s.getValue(src), f"{src}")

def test_copySingleCellToARect():
    s = Sheet()
    for src in [(0, 0), Point(0, 0), "A1"]:
        s.setValueCell(src, f'{src}')
        for dst in [
                (65533, 2147483645, 2, 2), Rect(Point(65533, 2147483645), Size(2, 2)), 
                "CRXN2147483646:CRXO2147483647"]:
            
            s.copyCell(src, dst)
            dstPoint = s._unpackArea(dst)[:2]
            assert strictEqual(s.getValue(dstPoint), f'{src}')
            assert strictEqual(s.getValue(Point(dstPoint[0] + 1, dstPoint[1])), f'{src}')
            assert strictEqual(s.getValue(Point(dstPoint[0], dstPoint[1] + 1)), f'{src}')
            assert strictEqual(s.getValue(Point(dstPoint[0] + 1, dstPoint[1] + 1)), f'{src}')
        assert strictEqual(s.getValue(src), f'{src}')

def test_throwsExceptionOnOutOfRangeCopy():
    s = Sheet()
    with pytest.raises(OverflowError):
        s.copyCell((-1, 0), "A2")
    with pytest.raises(OverflowError):
        s.copyCell((0, -1), "A2")
    with pytest.raises(OverflowError):
        s.copyCell((65535, 0), "A2")
    with pytest.raises(OverflowError):
        s.copyCell(Point(65535, 0), "A2")
    with pytest.raises(ValueError):
        s.copyCell("CRXP1", "A2")
    with pytest.raises(OverflowError):
        s.copyCell((0, 2147483647), "A2")
    with pytest.raises(OverflowError):
        s.copyCell(Point(0, 2147483647), "A2")
    with pytest.raises(ValueError):
        s.copyCell("A2147483649", "A2")

    with pytest.raises(OverflowError):
        s.copyCell("A2", (-1, 0))
    with pytest.raises(OverflowError):
        s.copyCell("A2", (0, -1))
    with pytest.raises(OverflowError):
        s.copyCell("A2", (65535, 0))
    with pytest.raises(OverflowError):
        s.copyCell("A2", Point(65535, 0))
    with pytest.raises(ValueError):
        s.copyCell("A2", "CRXP1")
    with pytest.raises(OverflowError):
        s.copyCell("A2", (0, 2147483647))
    with pytest.raises(OverflowError):
        s.copyCell("A2", Point(0, 2147483647))
    with pytest.raises(ValueError):
        s.copyCell("A2", "A2147483649")

    with pytest.raises(OverflowError):
        s.copyCell("A2", (-1, 0, 10, 10))
    with pytest.raises(OverflowError):
        s.copyCell("A2", (0, -1, 10, 10))
    with pytest.raises(OverflowError):
        s.copyCell("A2", (10, 10, -10, 10))
    with pytest.raises(OverflowError):
        s.copyCell("A2", (65534, 0, 2, 2))
    with pytest.raises(OverflowError):
        s.copyCell("A2", Rect(Point(65534, 0), Size(2, 2)))
    with pytest.raises(ValueError):
        s.copyCell("A2", "CRXN1:CRXP1")
    with pytest.raises(OverflowError):
        s.copyCell("A2", (0, 2147483646, 2, 2))
    with pytest.raises(OverflowError):
        s.copyCell("A2", Rect(Point(0, 2147483646), Size(2, 2)))
    with pytest.raises(ValueError):
        s.copyCell("A2", "A2147483647:A2147483648")

def test_copySingleCellRectange():
    s = Sheet()
    for src in [(0, 0), Point(0, 0), "A1", (0, 0, 1, 1), Rect(Point(0, 0), Size(1, 1)), "A1:A1"]:
        srcPoint = s._unpackArea(src)[:2]
        s.setValueCell(srcPoint, f'{src}')
        for dst in [(65534, 2147483646), Point(65534, 2147483646), "CRXO2147483647"]:
            s.copyCells(src, dst)
            dstPoint = s._unpackArea(dst)[:2]
            assert strictEqual(s.getValue(dstPoint), f'{src}')
        assert strictEqual(s.getValue(srcPoint), f'{src}')

def test_copyRegularRectange():
    s = Sheet()
    for src in [(0, 0, 2, 2), Rect(Point(0, 0), Size(2, 2)), "A1:B2"]:
        srcRect = s._unpackArea(src)
        for y in range(0, srcRect[3]):
            for x in range(0, srcRect[2]):
                s.setValueCell((srcRect[0] + x, srcRect[1] + y), f'{src} {x} {y}')

        for  dst in [(65533, 2147483645), Point(65533, 2147483645), "CRXN2147483646"]:
            
            s.copyCells(src, dst)
            dstOrigin = s._unpackPoint(dst)[:2]
            for y in range(0, srcRect[3]):
                for x in range(0, srcRect[2]):
                    assert strictEqual(s.getValue((dstOrigin[0] + x, dstOrigin[1] + y)), f'{src} {x} {y}')
                    assert strictEqual(s.getValue((srcRect[0] + x, srcRect[1] + y)), f'{src} {x} {y}')

def test_moveSingleCellToASingleCell():
    s = Sheet()
    for src in [(0, 0), Point(x=0, y=0), "A1"]:
        for dst in [
                (65534, 2147483646), Point(x=65534, y=2147483646), "CRXO2147483647"]:
            s.setValueCell(src, f"{src}")
            s.moveCell(src, dst)
            assert strictEqual(s.getValue(s._unpackArea(dst)[:2]), f"{src}")
            assert strictEqual(s.getValue(src), None)

def test_moveSingleCellRectange():
    s = Sheet()
    for src in [(0, 0), Point(0, 0), "A1", (0, 0, 1, 1), Rect(Point(0,0), Size(1,1)), "A1:A1"]:
        for dst in [(65534, 2147483646), Point(65534, 2147483646), "CRXO2147483647"]:
            srcPoint = s._unpackArea(src)[:2]
            s.setValueCell(srcPoint, f'{src}')
            s.moveCells(src, dst)
            dstPoint = s._unpackArea(dst)[:2]
            assert strictEqual(s.getValue(dstPoint), f"{src}")
            assert strictEqual(s.getValue(srcPoint), None)

def test_moveRegularRectange():
    s = Sheet()
    for src in [(0, 0, 2, 2), Rect(Point(0, 0), Size(2, 2)), "A1:B2"]:
        for  dst in [(65533, 2147483645), Point(65533, 2147483645), "CRXN2147483646"]:
            srcRect = s._unpackArea(src)
            for y in range(0, srcRect[3]):
                for x in range(0, srcRect[2]):
                    s.setValueCell((srcRect[0] + x, srcRect[1] + y), f'{src} {x} {y}')

            s.moveCells(src, dst)
            dstOrigin = s._unpackPoint(dst)[:2]
            for y in range(0, srcRect[3]):
                for x in range(0, srcRect[2]):
                    assert strictEqual(s.getValue((dstOrigin[0] + x, dstOrigin[1] + y)), f'{src} {x} {y}')
                    assert strictEqual(s.getValue((srcRect[0] + x, srcRect[1] + y)), None)