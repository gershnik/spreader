// @ts-check
"use strict";

const { runWithSheet } = require('./util')
const assert = require('assert')

describe('Recalc', function () {

    it('suspends and resumes recalc', runWithSheet((s) => {

        s.suspendRecalc();
        s.setFormulaCell("A1", "1 + 1");
        assert.strictEqual(s.getValue("A1"), null);
        s.resumeRecalc();
        assert.strictEqual(s.getValue("A1"), 2);

        s.suspendRecalc();
        s.setFormulaCell("A1", "5 * 18");
        assert.strictEqual(s.getValue("A1"), 2);
        s.recalculate();
        assert.strictEqual(s.getValue("A1"), 90);

    }));

    it('nests suspend and resume recalc', runWithSheet((s) => {

        s.suspendRecalc();
        s.suspendRecalc();
        s.setFormulaCell("A1", "1 + 1");
        assert.strictEqual(s.getValue("A1"), null);
        s.resumeRecalc();
        assert.strictEqual(s.getValue("A1"), null);
        s.resumeRecalc();
        assert.strictEqual(s.getValue("A1"), 2);

    }));
});