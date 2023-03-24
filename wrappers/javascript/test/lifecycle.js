// @ts-check
"use strict";

const { runWithSpreader } = require('./util')
const assert = require('assert')

describe('Lifecycle', function () {

    it('null handle throws exception', runWithSpreader((spreader) => {
        
        let s = new spreader.Sheet();
        s.delete();
        assert.throws(() => s.size(), WebAssembly.RuntimeError);
    }));

    it('double delete is harmless', runWithSpreader((spreader) => {
        
        let s = new spreader.Sheet();
        s.delete();
        assert.doesNotThrow(() => s.delete());
    }));

});

