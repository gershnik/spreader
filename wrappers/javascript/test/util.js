// @ts-check
"use strict";

const Spreader  = require('spreader')

/** @type {Spreader.Module} */
let spreader;

/**
 * @callback SpreaderFunc
 * @param {Spreader.Module} spreader 
 */

/**
 * 
 * @param {SpreaderFunc} func 
 */
 module.exports.runWithSpreader = function runWithSpreader(func) {
    return async () => {
        if (spreader === undefined)
            spreader = await Spreader();
        func(spreader)
    }
} 



/**
 * @callback SheetFunc
 * @param {Spreader.Sheet} s 
 * @param {Spreader.Module} spreader 
 */

/**
 * 
 * @param {SheetFunc} func 
 */
 module.exports.runWithSheet = function runWithSheet(func) {
    return async () => {
        if (spreader === undefined)
            spreader = await Spreader();
        let s = new spreader.Sheet();
        try {
            func(s, spreader)
        } finally {
            s.delete();
        }
    }
} 
