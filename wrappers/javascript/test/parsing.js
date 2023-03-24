// @ts-check

"use strict";

const { runWithSheet } = require('./util')
const assert = require('assert')

describe('Parsing', function () {

    it('converts row indices to numes', runWithSheet((s) => {
        
        assert.strictEqual(s.indexToRow(0), "1")
        assert.strictEqual(s.indexToRow(99), "100")
        assert.throws(() => s.indexToRow(2147483647), RangeError);
        assert.throws(() => s.indexToRow(-1), RangeError);
    }));

    it('converts column indices to names', runWithSheet((s) => {
        
        assert.strictEqual(s.indexToColumn(0), "A")
        assert.strictEqual(s.indexToColumn(25), "Z")
        assert.strictEqual(s.indexToColumn(26), "AA")
        assert.throws(() => s.indexToColumn(65536), RangeError);
        assert.throws(() => s.indexToColumn(-1), RangeError);
    }));

    it('parses row names', runWithSheet((s) => {
        
        assert.strictEqual(s.parseRow("1"), 0)
        assert.strictEqual(s.parseRow("100"), 99)
        assert.strictEqual(s.parseRow("1A"), null)
        assert.strictEqual(s.parseRow(""), null)
        assert.strictEqual(s.parseRow("123456789899"), null)
    }));

    it('parses column names', runWithSheet((s) => {
        
        assert.strictEqual(s.parseColumn("A"), 0)
        assert.strictEqual(s.parseColumn("Z"), 25)
        assert.strictEqual(s.parseColumn("AA"), 26)
        assert.strictEqual(s.parseColumn("1A"), null)
        assert.strictEqual(s.parseColumn(""), null)
        assert.strictEqual(s.parseColumn("XYZHH"), null)
    }));

    it('parses points', runWithSheet((s) => {
        
        assert.deepEqual(s.parsePoint("A1"), {x:0, y:0})
        assert.deepEqual(s.parsePoint("AA100"), {x:26, y:99})
        assert.strictEqual(s.parsePoint(""), null)
        assert.strictEqual(s.parsePoint("1A"), null)
    }));

    it('parses areas', runWithSheet((s) => {
        
        assert.deepEqual(s.parseArea("A1"), {origin:{x:0, y:0}, size:{width:1, height:1}})
        assert.deepEqual(s.parseArea("A1:AA100"), {origin:{x:0, y:0}, size:{width:27, height:100}})
        assert.strictEqual(s.parseArea(""), null)
        assert.strictEqual(s.parseArea("A1:"), null)
    }));

});