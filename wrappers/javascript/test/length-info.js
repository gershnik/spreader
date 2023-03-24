// @ts-check

"use strict";

const { runWithSheet } = require('./util')
const assert = require('assert')

function lengthRangeStr(startIdx, endIdx, info) {
    let ret = `[${startIdx}, ${endIdx}, `
    if (info.length !== undefined)
        ret += `${info.length}, `;
    ret += `${info.hidden}]`;
    return ret;
}

describe('Length Info', function () {

    it('sets and gets row height', runWithSheet((s) => {

        assert.deepStrictEqual(s.getRowHeight(3), {hidden: false});

        s.setRowsHeight(3, 1, 22);
        assert.deepStrictEqual(s.getRowHeight(3), {length: 22, hidden: false});
        assert.deepStrictEqual(s.getRowHeight("4"), {length: 22, hidden: false});
        
        s.setRowsHeight("5", 2, 13);
        assert.deepStrictEqual(s.getRowHeight(4), {length: 13, hidden: false});
        assert.deepStrictEqual(s.getRowHeight("5"), {length: 13, hidden: false});

        s.clearRowsHeight(2, 2);
        assert.deepStrictEqual(s.getRowHeight(2), {hidden: false});

        s.hideRows(2, 2, true);
        assert.deepStrictEqual(s.getRowHeight(2), {hidden: true});

        s.hideRows(2, 2, false);
        assert.deepStrictEqual(s.getRowHeight(2), {hidden: false});

        s.hideRows(4, 1, true);
        assert.deepStrictEqual(s.getRowHeight(4), {length: 13, hidden: true});

        let buf = "";
        assert.strictEqual(s.forEachRowHeight(0, 7, (startIdx, endIdx, info) => {
            buf += lengthRangeStr(startIdx, endIdx, info);
        }), true);
        assert.deepStrictEqual(buf, "[0, 4, false][4, 5, 13, true][5, 6, 13, false][6, 7, false]");

        buf = "";
        assert.strictEqual(s.forEachRowHeight(0, 7, (startIdx, endIdx, info) => {
            buf += lengthRangeStr(startIdx, endIdx, info);
            return true;
        }), true);
        assert.deepStrictEqual(buf, "[0, 4, false][4, 5, 13, true][5, 6, 13, false][6, 7, false]");

        buf = "";
        assert.strictEqual(s.forEachRowHeight(0, 7, (startIdx, endIdx, info) => {
            buf += lengthRangeStr(startIdx, endIdx, info);
            return false;
        }), false);
        assert.deepStrictEqual(buf, "[0, 4, false]");
    }));

    it('handles row height boundaries', runWithSheet((s) => {

        assert.throws(() => s.setRowsHeight(0, -1, 13), RangeError);
        assert.throws(() => s.setRowsHeight(0, 1, Math.pow(2, 33)), RangeError);

        // @ts-expect-error
        assert.throws(() => s.setRowsHeight(0, 1, "hello"), TypeError);
    }));

    it('sets and gets column width', runWithSheet((s) => {

        assert.deepStrictEqual(s.getColumnWidth(3), {hidden: false});

        s.setColumnsWidth(3, 1, 22);
        assert.deepStrictEqual(s.getColumnWidth(3), {length: 22, hidden: false});
        assert.deepStrictEqual(s.getColumnWidth("D"), {length: 22, hidden: false});
        
        s.setColumnsWidth("E", 2, 13);
        assert.deepStrictEqual(s.getColumnWidth(4), {length: 13, hidden: false});
        assert.deepStrictEqual(s.getColumnWidth("E"), {length: 13, hidden: false});

        s.clearColumnsWidth(2, 2);
        assert.deepStrictEqual(s.getColumnWidth(2), {hidden: false});

        s.hideColumns(2, 2, true);
        assert.deepStrictEqual(s.getColumnWidth(2), {hidden: true});

        s.hideColumns(2, 2, false);
        assert.deepStrictEqual(s.getColumnWidth(2), {hidden: false});

        s.hideColumns(4, 1, true);
        assert.deepStrictEqual(s.getColumnWidth(4), {length: 13, hidden: true});

        let buf = "";
        assert.strictEqual(s.forEachColumnWidth(0, 7, (startIdx, endIdx, info) => {
            buf += lengthRangeStr(startIdx, endIdx, info);
        }), true);
        assert.deepStrictEqual(buf, "[0, 4, false][4, 5, 13, true][5, 6, 13, false][6, 7, false]");

        buf = "";
        assert.strictEqual(s.forEachColumnWidth(0, 7, (startIdx, endIdx, info) => {
            buf += lengthRangeStr(startIdx, endIdx, info);
            return true;
        }), true);
        assert.deepStrictEqual(buf, "[0, 4, false][4, 5, 13, true][5, 6, 13, false][6, 7, false]");

        buf = "";
        assert.strictEqual(s.forEachColumnWidth(0, 7, (startIdx, endIdx, info) => {
            buf += lengthRangeStr(startIdx, endIdx, info);
            return false;
        }), false);
        assert.deepStrictEqual(buf, "[0, 4, false]");
    }));

    it('handles column width boundaries', runWithSheet((s) => {

        assert.throws(() => s.setColumnsWidth(0, -1, 13), RangeError);
        assert.throws(() => s.setColumnsWidth(0, 1, Math.pow(2, 33)), RangeError);

        // @ts-expect-error
        assert.throws(() => s.setColumnsWidth(0, 1, "hello"), TypeError);
    }));

});