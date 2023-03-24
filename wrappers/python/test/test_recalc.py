from spreader import Sheet
from .utils import strictEqual

def test_suspendResumeRecalc():
    s = Sheet()

    s.suspendRecalc()
    s.setFormulaCell("A1", "1 + 1")
    assert strictEqual(s.getValue("A1"), None)
    s.resumeRecalc()
    assert strictEqual(s.getValue("A1"), 2.)

    s.suspendRecalc()
    s.setFormulaCell("A1", "5 * 18")
    assert strictEqual(s.getValue("A1"), 2.)
    s.recalculate()
    assert strictEqual(s.getValue("A1"), 90.)

def test_nestedSuspendResumeRecalc():
    s = Sheet()

    s.suspendRecalc()
    s.suspendRecalc()
    s.setFormulaCell("A1", "1 + 1");
    assert strictEqual(s.getValue("A1"), None)
    s.resumeRecalc()
    assert strictEqual(s.getValue("A1"), None)
    s.resumeRecalc()
    assert strictEqual(s.getValue("A1"), 2.)