// @ts-check
"use strict";

const { runWithSheet } = require('./util')
const assert = require('assert')

describe('Copy cells', function () {
    it('copies single cell to a single cell', runWithSheet((s) => {
        for(let src of [[0, 0], {x:0, y:0}, "A1"]) {
            s.setValueCell(src, `${src}`);
            for(let dst of [
                    [65534, 2147483646], {x:65534, y:2147483646}, "CRXO2147483647",
                    [65534, 2147483646, 1 ,1], {origin:  {x:65534, y:2147483646}, size: {width:1, height:1} }, 
                    "CRXO2147483647:CRXO2147483647"]) {
                s.copyCell(src, dst);
                // @ts-expect-error
                assert.strictEqual(s.getValue(s._unpackArea(dst).origin), `${src}`);
            }
            assert.strictEqual(s.getValue(src), `${src}`);
        }
    }));

    it('copies single cell to a rect', runWithSheet((s) => {
        for(let src of [[0, 0], {x:0, y:0}, "A1"]) {
            s.setValueCell(src, `${src}`);
            for(let dst of [
                    [65533, 2147483645, 2, 2], {origin:  {x:65533, y:2147483645}, size: {width:2, height:2} }, 
                    "CRXN2147483646:CRXO2147483647"]) {
                
                s.copyCell(src, dst);
                // @ts-expect-error
                let dstPoint = s._unpackArea(dst).origin
                assert.strictEqual(s.getValue(dstPoint), `${src}`);
                assert.strictEqual(s.getValue({x: dstPoint.x + 1, y: dstPoint.y}), `${src}`);
                assert.strictEqual(s.getValue({x: dstPoint.x, y: dstPoint.y + 1}), `${src}`);
                assert.strictEqual(s.getValue({x: dstPoint.x + 1, y: dstPoint.y + 1}), `${src}`);
            }
            assert.strictEqual(s.getValue(src), `${src}`);
        }
    }));

    it('throws exception on out-of-range copy of a single cell', runWithSheet((s) => {
        assert.throws(() => s.copyCell([-1, 0], "A2"), RangeError);
        assert.throws(() => s.copyCell([0, -1], "A2"), RangeError);
        assert.throws(() => s.copyCell([65535, 0], "A2"), RangeError);
        assert.throws(() => s.copyCell({x:65535, y:0}, "A2"), RangeError);
        assert.throws(() => s.copyCell("CRXP1", "A2"), RangeError);
        assert.throws(() => s.copyCell([0, 2147483647], "A2"), RangeError);
        assert.throws(() => s.copyCell({x:0, y:2147483647}, "A2"), RangeError);
        assert.throws(() => s.copyCell("A2147483649", "A2"), RangeError);

        assert.throws(() => s.copyCell("A2", [-1, 0]), RangeError);
        assert.throws(() => s.copyCell("A2", [0, -1]), RangeError);
        assert.throws(() => s.copyCell("A2", [65535, 0]), RangeError);
        assert.throws(() => s.copyCell("A2", {x:65535, y:0}), RangeError);
        assert.throws(() => s.copyCell("A2", "CRXP1"), RangeError);
        assert.throws(() => s.copyCell("A2", [0, 2147483647]), RangeError);
        assert.throws(() => s.copyCell("A2", {x:0, y:2147483647}), RangeError);
        assert.throws(() => s.copyCell("A2", "A2147483649"), RangeError);

        assert.throws(() => s.copyCell("A2", [-1, 0, 10, 10]), RangeError);
        assert.throws(() => s.copyCell("A2", [0, -1, 10, 10]), RangeError);
        assert.throws(() => s.copyCell("A2", [10, 10, -10, 10]), RangeError);
        assert.throws(() => s.copyCell("A2", [65534, 0, 2, 2]), RangeError);
        assert.throws(() => s.copyCell("A2", {origin: {x:65534, y:0}, size:{width: 2, height:2}}), RangeError);
        assert.throws(() => s.copyCell("A2", "CRXN1:CRXP1"), RangeError);
        assert.throws(() => s.copyCell("A2", [0, 2147483646, 2, 2]), RangeError);
        assert.throws(() => s.copyCell("A2", {origin:{x:0, y:2147483646}, size:{width:2, height:2}}), RangeError);
        assert.throws(() => s.copyCell("A2", "A2147483647:A2147483648"), RangeError);
    }));

    it('copies a single cell rectangle', runWithSheet((s) => {
        for(let src of [[0, 0], {x:0, y:0}, "A1", [0, 0, 1, 1], {origin: {x:0, y:0}, size: {width: 1, height: 1}}, "A1:A1"]) {
            // @ts-expect-error
            let srcPoint = s._unpackArea(src).origin;
            s.setValueCell(srcPoint, `${src}`);
            for(let dst of [[65534, 2147483646], {x:65534, y:2147483646}, "CRXO2147483647"]) {
                
                s.copyCells(src, dst);
                // @ts-expect-error
                let dstPoint = s._unpackArea(dst).origin
                assert.strictEqual(s.getValue(dstPoint), `${src}`);
            }
            assert.strictEqual(s.getValue(srcPoint), `${src}`);
        }
    }));

    it('copies a regular cell rectangle', runWithSheet((s) => {
        for(let src of [[0, 0, 2, 2], {origin: {x:0, y:0}, size: {width: 2, height: 2}}, "A1:B2"]) {
            // @ts-expect-error
            let srcRect = s._unpackArea(src);
            for(let y = 0; y < srcRect.size.height; ++y)
                for(let x = 0; x < srcRect.size.width; ++x)
                    s.setValueCell([srcRect.origin.x + x, srcRect.origin.y + y], `${src} ${x} ${y}`);

            for(let dst of [[65533, 2147483645], {x:65533, y:2147483645}, "CRXN2147483646"]) {
                
                s.copyCells(src, dst);
                // @ts-expect-error
                let dstOrigin = s._unpackPoint(dst);
                for(let y = 0; y < srcRect.size.height; ++y)
                    for(let x = 0; x < srcRect.size.width; ++x) {
                        assert.strictEqual(s.getValue([dstOrigin.x + x, dstOrigin.y + y]), `${src} ${x} ${y}`);
                        assert.strictEqual(s.getValue([srcRect.origin.x + x, srcRect.origin.y + y]), `${src} ${x} ${y}`);
                    }
            }
        }
    }));
});

describe('Move cells', function () {

    it('moves single cell to a single cell', runWithSheet((s) => {
        for(let src of [[0, 0], {x:0, y:0}, "A1"]) {
            for(let dst of [[65534, 2147483646], {x:65534, y:2147483646}, "CRXO2147483647"]) {
                s.setValueCell(src, `${src}`);
                s.moveCell(src, dst);
                // @ts-expect-error
                let dstPoint = s._unpackArea(dst).origin
                assert.strictEqual(s.getValue(dstPoint), `${src}`);
                assert.strictEqual(s.getValue(src), null);
            }
        }
    }));

    it('moves a single cell rectangle', runWithSheet((s) => {
        for(let src of [[0, 0], {x:0, y:0}, "A1", [0, 0, 1, 1], {origin: {x:0, y:0}, size: {width: 1, height: 1}}, "A1:A1"]) {
            for(let dst of [[65534, 2147483646], {x:65534, y:2147483646}, "CRXO2147483647"]) {
                // @ts-expect-error
                let srcPoint = s._unpackArea(src).origin;
                s.setValueCell(srcPoint, `${src}`);
                s.moveCells(src, dst);
                // @ts-expect-error
                let dstPoint = s._unpackArea(dst).origin
                assert.strictEqual(s.getValue(dstPoint), `${src}`);
                assert.strictEqual(s.getValue(srcPoint), null);
            }
        }
    }));

    it('moves a regular cell rectangle', runWithSheet((s) => {
        for(let src of [[0, 0, 2, 2], {origin: {x:0, y:0}, size: {width: 2, height: 2}}, "A1:B2"]) {
            for(let dst of [[65533, 2147483645], {x:65533, y:2147483645}, "CRXN2147483646"]) {
                // @ts-expect-error
                let srcRect = s._unpackArea(src);
                for(let y = 0; y < srcRect.size.height; ++y)
                    for(let x = 0; x < srcRect.size.width; ++x)
                        s.setValueCell([srcRect.origin.x + x, srcRect.origin.y + y], `${src} ${x} ${y}`);
                s.moveCells(src, dst);
                // @ts-expect-error
                let dstOrigin = s._unpackPoint(dst);
                for(let y = 0; y < srcRect.size.height; ++y)
                    for(let x = 0; x < srcRect.size.width; ++x) {
                        assert.strictEqual(s.getValue([dstOrigin.x + x, dstOrigin.y + y]), `${src} ${x} ${y}`);
                        assert.strictEqual(s.getValue([srcRect.origin.x + x, srcRect.origin.y + y]), null);
                    }
            }
        }
    }));

});