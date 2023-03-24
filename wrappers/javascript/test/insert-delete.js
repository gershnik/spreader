// @ts-check
"use strict";

const { runWithSheet } = require('./util')
const assert = require('assert')

describe('Delete rows', function () {

    it('deletes rows in empty sheet', runWithSheet((s) => {

        assert.doesNotThrow(() => s.deleteRows(0, 0));
        assert.deepStrictEqual(s.size(), {width:0, height:0});
        assert.doesNotThrow(() => s.deleteRows(0, 5));
        assert.deepStrictEqual(s.size(), {width:0, height:0});

    }));

    it('deletes extra rows', runWithSheet((s) => {

        s.setFormulaCell("A1", "A20 + 3");
        s.deleteRows(5, 4);
        assert.deepStrictEqual(s.size(), {width:1, height:1});
        assert.deepStrictEqual(s.getEditInfo("A1").formula, {text: "A16 + 3", extent: {width: 1, height: 1}});

    }));

    it('deletes furthest rows', runWithSheet((s) => {

        s.setFormulaCell("A2147483647", "B2147483647 + 3");
        assert.deepStrictEqual(s.size(), {width:1, height:2147483647});
        s.deleteRows("1", 2147483647);
        assert.deepStrictEqual(s.size(), {width:1, height:0});

        s.setFormulaCell("A2147483647", "B2147483647 + 3");
        s.deleteRows("1", 2147483646);
        assert.deepStrictEqual(s.getEditInfo("A1").formula, {text: "B1 + 3", extent: {width: 1, height: 1}});

    }));

});

describe('Delete columns', function () {

    it('deletes columns in empty sheet', runWithSheet((s) => {

        assert.doesNotThrow(() => s.deleteColumns(0, 0));
        assert.deepStrictEqual(s.size(), {width:0, height:0});
        assert.doesNotThrow(() => s.deleteColumns(0, 5));
        assert.deepStrictEqual(s.size(), {width:0, height:0});

    }));

    it('deletes extra columns', runWithSheet((s) => {

        s.setFormulaCell("A1", "T1 + 3");
        s.deleteColumns(5, 4);
        assert.deepStrictEqual(s.size(), {width:1, height:1});
        assert.deepStrictEqual(s.getEditInfo("A1").formula, {text: "P1 + 3", extent: {width: 1, height: 1}});

    }));

    it('deletes furthest columns', runWithSheet((s) => {

        s.setFormulaCell("CRXO1", "CRXO2 + 3");
        assert.deepStrictEqual(s.size(), {width:65535, height:1});
        s.deleteColumns("A", 65535);
        assert.deepStrictEqual(s.size(), {width:0, height:1});

        s.setFormulaCell("CRXO1", "CRXO2 + 3");
        s.deleteColumns("A", 65534);
        assert.deepStrictEqual(s.getEditInfo("A1").formula, {text: "A2 + 3", extent: {width: 1, height: 1}});

    }));

});

describe('Insert rows', function () {

    it('inserts rows in empty sheet', runWithSheet((s) => {

        s.insertRows(0, 1);
        assert.deepStrictEqual(s.size(), {width:0, height:1});
        s.deleteRows(0, 1);

        s.insertRows("1", 15);
        assert.deepStrictEqual(s.size(), {width:0, height:15});
        s.deleteRows(0, 15);

        s.insertRows("5", 15);
        assert.deepStrictEqual(s.size(), {width:0, height:19});
        s.deleteRows(0, 20);
    }));

    it('inserts furthest rows', runWithSheet((s) => {

        s.insertRows(2147483646, 1);
        assert.deepStrictEqual(s.size(), {width:0, height:2147483647});
        s.deleteRows(0, 2147483647);

        assert.throws(() => s.insertRows(2147483647, 1), RangeError);

        s.setValueCell("A2147483646", 1);
        s.insertRows(2147483646, 1);
        assert.deepStrictEqual(s.size(), {width:1, height:2147483647});
    }));
});

describe('Insert columns', function () {

    it('inserts columns in empty sheet', runWithSheet((s) => {

        s.insertColumns(0, 1);
        assert.deepStrictEqual(s.size(), {width:1, height:0});
        s.deleteColumns(0, 1);

        s.insertColumns("A", 15);
        assert.deepStrictEqual(s.size(), {width:15, height:0});
        s.deleteColumns(0, 15);

        s.insertColumns("E", 15);
        assert.deepStrictEqual(s.size(), {width:19, height:0});
        s.deleteColumns(0, 20);
    }));

    it('inserts furthest columns', runWithSheet((s) => {

        s.insertColumns(65534, 1);
        assert.deepStrictEqual(s.size(), {width:65535, height:0});
        s.deleteColumns(0, 65535);

        assert.throws(() => s.insertColumns(65535, 1), RangeError);

        s.setValueCell("CRXN1", 1);
        s.insertColumns(65534, 1);
        assert.deepStrictEqual(s.size(), {width:65535, height:1});
    }));
});