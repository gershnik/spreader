// @ts-check
"use strict";

const { runWithSheet } = require('./util')
const assert = require('assert')


describe('Value cells', function () {
    it('returns null for unset value', runWithSheet((s) => {
        for(let coord of [[0, 0], {x:0, y:0}, "A1"]) {
            assert.strictEqual(s.getValue(coord), null);
        }
    }));

    it('returns previously set value', runWithSheet((s, spreader) => {

        for(let coord of [[0, 0], {x:0, y:0}, "A1", "CRXO2147483647"]) {
            s.setValueCell(coord, null);
            assert.strictEqual(s.getValue(coord), null);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 0n);

            s.setValueCell(coord, true);
            assert.strictEqual(s.getValue(coord), true);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, false);
            assert.strictEqual(s.getValue(coord), false);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, 1);
            assert.strictEqual(s.getValue(coord), 1);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, 42.);
            assert.strictEqual(s.getValue(coord), 42.);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, +0.);
            assert.ok(Object.is(s.getValue(coord), +0.));
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, -0.);
            assert.ok(Object.is(s.getValue(coord), -0.));
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, Number.POSITIVE_INFINITY);
            assert.strictEqual(s.getValue(coord), spreader.ErrorValue.NotANumber);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, Number.NEGATIVE_INFINITY);
            assert.strictEqual(s.getValue(coord), spreader.ErrorValue.NotANumber);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, Number.NaN);
            assert.strictEqual(s.getValue(coord), spreader.ErrorValue.NotANumber);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, "");
            assert.strictEqual(s.getValue(coord), "");
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, "abc");
            assert.strictEqual(s.getValue(coord), "abc");
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, "a水𐀀𝄞bcå🤢");
            assert.strictEqual(s.getValue(coord), "a水𐀀𝄞bcå🤢");
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            // @ts-expect-error
            s.setValueCell(coord, new String("abc"));
            assert.strictEqual(s.getValue(coord), "abc");
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, spreader.ErrorValue.InvalidValue);
            assert.strictEqual(s.getValue(coord), spreader.ErrorValue.InvalidValue);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setValueCell(coord, spreader.ErrorValue.InvalidName);
            assert.strictEqual(s.getValue(coord), spreader.ErrorValue.InvalidName);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            // @ts-expect-error
            s.setValueCell(coord, spreader.ErrorValue.fromCode(225));
            // @ts-expect-error
            assert.strictEqual(s.getValue(coord), spreader.ErrorValue.Error225);
            assert.deepStrictEqual(s.getEditInfo(coord), {});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setFormulaCell(coord, "1 + 3");
            assert.strictEqual(s.getValue(coord), 4);
            assert.deepStrictEqual(s.getEditInfo(coord), {formula: {text: "1 + 3", extent: {width: 1, height: 1}}});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.setFormulaCell(coord, "");
            assert.strictEqual(s.getValue(coord), spreader.ErrorValue.InvalidFormula);
            assert.deepStrictEqual(s.getEditInfo(coord), {formula: {text: "", extent: {width: 1, height: 1}}});
            assert.strictEqual(s.nonNullCellCount(), 1n);

            s.clearCellValue(coord);
        }
    }));

    it('clears value properly', runWithSheet((s) => {
        for(let coord of [[0, 0], {x:0, y:0}, "A1", "CRXO2147483647"]) {
            s.setValueCell(coord, 42.);
            s.clearCellValue(coord);
            assert.strictEqual(s.getValue(coord), null);
            assert.strictEqual(s.nonNullCellCount(), 0n);

            s.setFormulaCell(coord, "1 + 3");
            s.clearCellValue(coord);
            assert.strictEqual(s.getValue(coord), null);
            assert.strictEqual(s.nonNullCellCount(), 0n);

        }
    }));

    it('throws exception for invalid value type', runWithSheet((s) => {
        // @ts-expect-error
        assert.throws(() => s.setValueCell([0, 0], [1]), TypeError);
        // @ts-expect-error
        assert.throws(() => s.setValueCell([0, 0], {}), TypeError);
        // @ts-expect-error
        assert.throws(() => s.setValueCell([0, 0], new Number(1)), TypeError);
        // @ts-expect-error
        assert.throws(() => s.setValueCell([0, 0], undefined), TypeError);

        // @ts-expect-error
        assert.throws(() => s.setFormulaCell([0, 0], null), TypeError);
        // @ts-expect-error
        assert.throws(() => s.setFormulaCell([0, 0], undefined), TypeError);
        // @ts-expect-error
        assert.throws(() => s.setFormulaCell([0, 0], 5), TypeError);
    }));

    const testOutOfRange = (accessor) => {
        assert.throws(() => accessor([-1, 0]), RangeError);
        assert.throws(() => accessor([0, -1]), RangeError);
        assert.throws(() => accessor([65535, 0]), RangeError);
        assert.throws(() => accessor({x:65535, y:0}), RangeError);
        assert.throws(() => accessor("CRXP1"), RangeError);
        assert.throws(() => accessor([0, 2147483647]), RangeError);
        assert.throws(() => accessor({x:0, y:2147483647}), RangeError);
        assert.throws(() => accessor("A2147483649"), RangeError);
    };

    it('throws exception on out-of-range access', runWithSheet((s) => {
        testOutOfRange((coord) => { s.getValue(coord)});
        testOutOfRange((coord) => { s.getEditInfo(coord)});
        testOutOfRange((coord) => { s.setValueCell(coord, null)});
        testOutOfRange((coord) => { s.clearCellValue(coord)});
        testOutOfRange((coord) => { s.setFormulaCell(coord, "1 + 1")});
    }));

    const testBogusCoord = (accessor) => {
        assert.throws(() => accessor(3), TypeError);
        assert.throws(() => accessor([1]), TypeError);
        assert.throws(() => accessor(), TypeError);
        assert.throws(() => accessor({y: 4}), TypeError);
        assert.throws(() => accessor(null), TypeError);
        assert.throws(() => accessor(undefined), TypeError);
        assert.throws(() => accessor([Number.NaN, 0]), TypeError);
        assert.throws(() => accessor([1.2, 0]), TypeError);
        assert.throws(() => accessor([1. / 0., 0]), TypeError);
    };

    it('throws exception on bogus coordinate type', runWithSheet((s) => {
        testBogusCoord((coord) => { s.getValue(coord)});
        testBogusCoord((coord) => { s.getEditInfo(coord)});
        testBogusCoord((coord) => { s.setValueCell(coord, null)});
        testBogusCoord((coord) => { s.clearCellValue(coord)});
        testBogusCoord((coord) => { s.setFormulaCell(coord, "1 + 1")});
    }));
});
