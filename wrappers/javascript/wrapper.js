// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause


/**
 * @typedef Point
 * @type {{x: number, y: number}}
 */

/**
 * @typedef Size
 * @type {{width: number, height: number}}
 */

/**
 * @typedef Rect
 * @type {{origin: Point, size: Size}}
 */


/**
 * @typedef CellCoord
 * @type {string | number[] | Point}
 */

/**
 * @typedef AreaCoord
 * @type {CellCoord | Rect}
 */

/**
 * @typedef RowCoord
 * @type {string | number}
 */

/**
 * @typedef ColumnCoord
 * @type {string | number}
 */

/**
 * @typedef Scalar
 * @type {null | boolean | number | string | ErrorValue}
 */

/**
 * @typedef EditInfo
 * @type {{formula?: {text: string, extent: Size}}}
 */

/**
 * @typedef LengthInfo
 * @type { {length?: number, hidden: boolean}}
 */

/**
 * @typedef LengthInfoHandler 
 * @type {(startIdx: number, endIdx: number, info: LengthInfo) => boolean}
 */


class ErrorValue {

    /**
     * @param {number} code 
     */
    constructor(code) {
        this.code = code;
    }

    static fromCode(code) {
        switch(code) {
            case  1: return ErrorValue.NullRange; 
            case  2: return ErrorValue.DivisionByZero;
            case  3: return ErrorValue.InvalidValue;
            case  4: return ErrorValue.InvalidReference;
            case  5: return ErrorValue.InvalidName;
            case  6: return ErrorValue.NotANumber;
            case  7: return ErrorValue.InvalidArgs;
            case  8: return ErrorValue.GettingData;
            case  9: return ErrorValue.Spill;
            case 10:  return ErrorValue.InvalidFormula;
        }
        if (ErrorValue[`Error${code}`] === undefined) {
            ErrorValue[`Error${code}`] = Object.freeze(new ErrorValue(code));
        }
        return ErrorValue[`Error${code}`];
    }
}

Module['ErrorValue'] = ErrorValue;
Module['ErrorValue'].NullRange        = Object.freeze(new ErrorValue( 1)); //#NULL!
Module['ErrorValue'].DivisionByZero   = Object.freeze(new ErrorValue( 2)); //#DIV/0!
Module['ErrorValue'].InvalidValue     = Object.freeze(new ErrorValue( 3)); //#VALUE!
Module['ErrorValue'].InvalidReference = Object.freeze(new ErrorValue( 4)); //#REF!
Module['ErrorValue'].InvalidName      = Object.freeze(new ErrorValue( 5)); //#NAME?
Module['ErrorValue'].NotANumber       = Object.freeze(new ErrorValue( 6)); //#NUM!
Module['ErrorValue'].InvalidArgs      = Object.freeze(new ErrorValue( 7)); //#N/A
Module['ErrorValue'].GettingData      = Object.freeze(new ErrorValue( 8)); //#N/A
Module['ErrorValue'].Spill            = Object.freeze(new ErrorValue( 9)); //#SPILL!
Module['ErrorValue'].InvalidFormula   = Object.freeze(new ErrorValue(10)); //#ERROR!



class Sheet {

    constructor() {
        this.handle = _makeSheet();
    }

    delete() {
        _deleteSheet(this.handle);
        this.handle = 0;
    }

    /** @return {Size} */
    size() {
        let val = _sheetSize(this.handle);
        try {
            return Emval.toValue(val);
        } finally {
            __emval_decref(val);
        }
    }

    /** @return {Size} */
    static get maxSize() {
        let val = _sheetMaxSize();
        try {
            let ret = Emval.toValue(val);
            Object.defineProperty(this, 'maxSize', {
                value: ret
            });
            return ret;
        } finally {
            __emval_decref(val);
        }
    }

    /** @return {Size} */
    get maxSize() {
        return Sheet.maxSize;
    }

    /** @return {bigint} */
    nonNullCellCount() {
        let val = _sheetNonNullCellCount(this.handle);
        try {
            return Emval.toValue(val);
        } finally {
            __emval_decref(val);
        }
    }

    /**
     * @param {CellCoord} coord 
     * @param {Scalar} value 
     */
    setValueCell(coord, value) {
        let point = this._unpackPoint(coord);

        if (value === null) {
            _sheetClearCell(this.handle, point.x, point.y);
        } else if (typeof value === 'boolean') {
            _sheetSetValueCellBool(this.handle, point.x, point.y, value);
        } else if (typeof value === 'number') {
            _sheetSetValueCellDouble(this.handle, point.x, point.y, value);
        } else if (typeof value === 'string' || value instanceof String) {
            let val = Emval.toHandle(value);
            try {
                _sheetSetValueCellString(this.handle, point.x, point.y, val);
            } finally {
                __emval_decref(val);
            }
        } else if (value instanceof ErrorValue) { 
            _sheetSetValueCellError(this.handle, point.x, point.y, value.code);
        } else {
            throw new TypeError("invalid value type");
        }
    }

    /**
     * @param {CellCoord} coord 
     * @param {string} formula 
     */
    setFormulaCell(coord, value) {
        let point = this._unpackPoint(coord);
        if (typeof value !== 'string' && !(value instanceof String))
            throw new TypeError("formaula is not a string: " + value);
        let val = Emval.toHandle(value);
        try {
            _sheetSetFormulaCell(this.handle, point.x, point.y, val);
        } finally {
            __emval_decref(val);
        }
    }

    /**
     * @param {CellCoord} coord 
     */
    clearCellValue(coord) {
        let point = this._unpackPoint(coord);
        _sheetClearCell(this.handle, point.x, point.y);   
    }

    /**
     * @param {CellCoord} coord 
     * @return {Scalar}
     */
    getValue(coord) {
        let point = this._unpackPoint(coord);
        let ret = _sheetGetValue(this.handle, point.x, point.y);
        try {
            return Emval.toValue(ret).value;
        } finally {
            __emval_decref(ret);
        }
    }

    /**
     * 
     * @param {CellCoord} coord 
     * @return {EditInfo}
     */
    getEditInfo(coord) {
        let point = this._unpackPoint(coord);
        let ret = _sheetGetEditInfo(this.handle, point.x, point.y);
        try {
            return Emval.toValue(ret);
        } finally {
            __emval_decref(ret);
        }
    }

    /**
     * @param {CellCoord} from 
     * @param {AreaCoord} to 
     */
    copyCell(from, to) {
        let fromPoint = this._unpackPoint(from);
        let toRect = this._unpackArea(to);
        _sheetCopyCell(this.handle, fromPoint.x, fromPoint.y, toRect.origin.x, toRect.origin.y, toRect.size.width, toRect.size.height);
    }

    /**
     * @param {AreaCoord} from 
     * @param {CellCoord} to 
     */
    copyCells(from, to) {
        let fromRect = this._unpackArea(from);
        let toPoint = this._unpackPoint(to);
        _sheetCopyCells(this.handle, fromRect.origin.x, fromRect.origin.y, fromRect.size.width, fromRect.size.height, toPoint.x, toPoint.y);
    }

    /**
     * @param {CellCoord} from 
     * @param {CellCoord} to 
     */
    moveCell(from, to) {
        let fromPoint = this._unpackPoint(from);
        let toPoint = this._unpackPoint(to);
        _sheetMoveCell(this.handle, fromPoint.x, fromPoint.y,  toPoint.x, toPoint.y);
    }

    /**
     * @param {AreaCoord} from 
     * @param {CellCoord} to 
     */
    moveCells(from, to) {
        let fromRect = this._unpackArea(from);
        let toPoint = this._unpackPoint(to);
        _sheetMoveCells(this.handle, fromRect.origin.x, fromRect.origin.y, fromRect.size.width, fromRect.size.height, toPoint.x, toPoint.y);
    }

    /**
     * @param {RowCoord} y 
     * @param {number} count 
     */
    deleteRows(y, count) {
        const row = this._unpackRow(y);
        this._validateRowCount(row, count);
        _sheetDeleteRows(this.handle, row, count);
    }

    /**
     * @param {ColumnCoord} x 
     * @param {number} count 
     */
     deleteColumns(x, count) {
        const column = this._unpackColumn(x);
        this._validateColumnCount(column, count);
        _sheetDeleteColumns(this.handle, column, count);
    }

    /**
     * @param {RowCoord} y 
     * @param {number} count 
     */
    insertRows(y, count) {
        const row = this._unpackRow(y);
        this._validateRowCount(row, count);
        _sheetInsertRows(this.handle, row, count);
    }

    /**
     * @param {ColumnCoord} x 
     * @param {number} count 
     */
    insertColumns(x, count) {
        const column = this._unpackColumn(x);
        this._validateColumnCount(column, count);
        _sheetInsertColumns(this.handle, column, count);
    }

    suspendRecalc() {
        _sheetSuspendRecalc(this.handle);
    }

    resumeRecalc() {
        _sheetResumeRecalc(this.handle);
    }

    recalculate() {
        _sheetRecalculate(this.handle);
    }

    /**
     * @param {RowCoord} y 
     * @param {number} count 
     * @param {number} height 
     */
    setRowsHeight(y, count, height) {
        y = this._unpackRow(y);
        this._validateRowCount(y, count);
        this._validateLength(height);
        _sheetSetRowHeight(this.handle, y, count, height);
    }

    /**
     * @param {RowCoord} y 
     * @param {number} count 
     */
    clearRowsHeight(y, count) {
        y = this._unpackRow(y);
        this._validateRowCount(y, count);
        _sheetClearRowHeight(this.handle, y, count);
    }

    /**
     * @param {RowCoord} y 
     * @param {number} count 
     * @param {boolean} hidden 
     */
    hideRows(y, count, hidden) {
        y = this._unpackRow(y);
        this._validateRowCount(y, count);
        _sheetHideRow(this.handle, y, count, Boolean(hidden));
    }

    /**
     * @param {RowCoord} y 
     * @returns {LengthInfo}
     */
    getRowHeight(y) {
        y = this._unpackRow(y);
        let ret = _sheetGetRowHeight(this.handle, y);
        try {
            return Emval.toValue(ret);
        } finally {
            __emval_decref(ret);
        }
    }

    /**
     * @param {RowCoord} y 
     * @param {number} count 
     * @param {LengthInfoHandler} func 
     */
    forEachRowHeight(y, count, func) {
        y = this._unpackRow(y);
        this._validateRowCount(y, count);
        let funcHandle = Emval.toHandle(func);
        try {
            return _sheetForEachRowHeight(this.handle, y, count, funcHandle) != 0;
        } finally {
            __emval_decref(funcHandle);
        }
    }

    /**
     * @param {ColumnCoord} x 
     * @param {number} count 
     * @param {number} width 
     */
    setColumnsWidth(x, count, width) {
        x = this._unpackColumn(x);
        this._validateColumnCount(x, count);
        this._validateLength(width);
        _sheetSetColumnWidth(this.handle, x, count, width);
    }

    /**
     * @param {ColumnCoord} x 
     * @param {number} count 
     */
    clearColumnsWidth(x, count) {
        x = this._unpackColumn(x);
        this._validateColumnCount(x, count);
        _sheetClearColumnWidth(this.handle, x, count);
    }

    /**
     * @param {ColumnCoord} x 
     * @param {number} count 
     * @param {boolean} hidden 
     */
    hideColumns(x, count, hidden) {
        x = this._unpackColumn(x);
        this._validateColumnCount(x, count);
        _sheetHideColumn(this.handle, x, count, Boolean(hidden));
    }

    /**
     * @param {ColumnCoord} x 
     * @returns {LengthInfo}
     */
    getColumnWidth(x) {
        x = this._unpackColumn(x);
        let ret = _sheetGetColumnWidth(this.handle, x);
        try {
            return Emval.toValue(ret);
        } finally {
            __emval_decref(ret);
        }
    }

    /**
     * @param {ColumnCoord} x 
     * @param {number} count 
     * @param {LengthInfoHandler} func 
     */
     forEachColumnWidth(x, count, func) {
        x = this._unpackColumn(x);
        this._validateColumnCount(x, count);
        let funcHandle = Emval.toHandle(func);
        try {
            return _sheetForEachColumnWidth(this.handle, x, count, funcHandle) != 0;
        } finally {
            __emval_decref(funcHandle);
        }
    }

    /**
     * @param {number} x 
     * @return {string} 
     */
    indexToColumn(x) {
        let res = _sheetIndexToColumn(this.handle, this._validateColumn(x));
        try {
            return Emval.toValue(res);
        } finally {
            __emval_decref(res);
        }
    }

    /**
     * @param {number} y
     * @return {string} 
     */
    indexToRow(y) {
        let res = _sheetIndexToRow(this.handle, this._validateRow(y));
        try {
            return Emval.toValue(res);
        } finally {
            __emval_decref(res);
        }
    }

    /** 
     * @param {string} str
     * @return {?number}
    */
    parseColumn(str) {
        let val = Emval.toHandle(str);
        try {
            let res = _sheetParseColumn(this.handle, val);
            return res >= 0 ? res : null;
        } finally {
            __emval_decref(val);
        }
    }

    /** 
     * @param {string} str
     * @return {?number}
    */
    parseRow(str) {
        let val = Emval.toHandle(str);
        try {
            let res = _sheetParseRow(this.handle, val);
            return res >= 0 ? res : null;
        } finally {
            __emval_decref(val);
        }
    }

    /** 
     * @param {string} str
     * @return {?Point}
    */
    parsePoint(str) {
        let val = Emval.toHandle(str);
        try {
            let res = _sheetParsePoint(this.handle, val);
            try {
                return Emval.toValue(res);
            } finally {
                __emval_decref(res);
            }
        } finally {
            __emval_decref(val);
        }
    }

    /** 
     * @param {string} str
     * @return {?Rect}
    */
    parseArea(str) {
        let val = Emval.toHandle(str);
        try {
            let res = _sheetParseArea(this.handle, val);
            try {
                return Emval.toValue(res);
            } finally {
                __emval_decref(res);
            }
        } finally {
            __emval_decref(val);
        }
    }

    /**
     * @param {RowCoord} y 
     */
    _unpackRow(y) {
        if (typeof y === 'string' || y instanceof String) {
            const ret = this.parseRow(y);
            if (ret === null)
                throw new RangeError("invalid row: " + y);
            return ret;
        } else {
            return this._validateRow(y);
        }
    }

    /**
     * @param {ColumnCoord} x 
     */
     _unpackColumn(x) {
        if (typeof x === 'string' || x instanceof String) {
            const ret = this.parseColumn(x);
            if (ret === null)
                throw new RangeError("invalid column: " + x);
            return ret;
        } else {
            return this._validateColumn(x);
        }
    }

    /**
     * @param {CellCoord} coord 
     * @returns {Point}
     */
    _unpackPoint(coord) {
        if (typeof coord === 'string' || coord instanceof String) {
            const ret = this.parsePoint(coord);
            if (ret === null) 
                throw new RangeError("invalid point reference: " + coord);
            return ret;
        } else if (Array.isArray(coord)) {
            if (coord.length != 2)
                throw new TypeError("invalid point coordinate array: " + coord)
            return this._validatePoint({ x: coord[0], y: coord[1]});
        } else if (coord !== null) {
            return this._validatePoint(coord);
        } else {
            throw new TypeError("invalid point coordinate: " + JSON.stringify(coord));
        }
    }

    /**
     * @param {AreaCoord} coord 
     * @returns {Rect}
     */
     _unpackArea(coord) {
        if (typeof coord === 'string' || coord instanceof String) {
            const ret = this.parseArea(coord, this.size());
            if (ret === null) 
                throw new RangeError("invalid area reference: " + coord)
            return ret;
        } else if (Array.isArray(coord)) {
            if (coord.length == 4) {
                return this._validateRect({ 
                    origin: { x: coord[0], y: coord[1]}, 
                    size: {width: coord[2], height: coord[3]}
                });
            } else if (coord.length == 2) {
                return { 
                    origin: this._validatePoint({ x: coord[0], y: coord[1]}), 
                    size: {width: 1, height: 1}
                };
            }
            throw new TypeError("invalid area coordinate array: " + coord)
        } else if (coord !== null) {
            if (coord.size !== undefined)
                return this._validateRect(coord);
            return { 
                origin: this._validatePoint(coord), 
                size: {width: 1, height: 1}
            };
        } else {
            throw new TypeError("invalid area coordinate: " + JSON.stringify(coord));
        }
    }

    /**
     * @param {number} y 
     * @returns {number}
     */
     _validateRow(y) {
        if (!Number.isInteger(y))
            throw new TypeError("invalid row index: " + JSON.stringify(y));
        const max = this.maxSize.height
        if (y < 0 || y >= max)
            throw new RangeError("invalid row index: " + y);
        return y;
    }

    /**
     * @param {number} x 
     * @returns {number}
     */
     _validateColumn(x) {
        if (!Number.isInteger(x))
            throw new TypeError("invalid column index: " + JSON.stringify(x));
        const max = this.maxSize.width
        if (x < 0 || x >= max)
            throw new RangeError("invalid column index: " + x);
        return x;
    }

    /**
     * @param {Point} pt 
     * @returns {Point}
     */
    _validatePoint(pt) {
        if (!Number.isInteger(pt.x) || !Number.isInteger(pt.y))
            throw new TypeError("invalid point: " + JSON.stringify(pt));
        const max = this.maxSize
        if (pt.x < 0 || pt.x >= max.width || pt.y < 0 || pt.y >= max.height)
            throw new RangeError("invalid point: " + JSON.stringify(pt));
        return pt;
    }

    /**
     * @param {Size} size 
     * @returns {Size}
     */
    _validateSize(size) {
        if (!Number.isInteger(size.width) || !Number.isInteger(size.height))
            throw new TypeError("invalid size: " + JSON.stringify(size));
        const max = this.maxSize
        if (size.width < 0 || size.width > max.width || size.height < 0 || size.height > max.height)
            throw new RangeError("invalid size: " + JSON.stringify(size));
        return size;
    }

    /**
     * @param {Rect} rect 
     * @returns {Rect}
     */
     _validateRect(rect) {
        if (!Number.isInteger(rect.origin.x) || !Number.isInteger(rect.origin.y) || 
            !Number.isInteger(rect.size.width) || !Number.isInteger(rect.size.height))
            throw new TypeError("invalid rectangle: " + JSON.stringify(rect));
        const max = this.maxSize
        if (rect.origin.x < 0 || rect.origin.x >= max.width || rect.origin.y < 0 || rect.origin.y >= max.height)
            throw new RangeError("invalid rectangle origin: " + JSON.stringify(rect.origin));
        if (rect.size.width < 0 || rect.size.width > max.width || rect.size.height < 0 || rect.size.height > max.height)
            throw new RangeError("invalid rectangle size: " + JSON.stringify(rect.size));
        if (max.width - rect.size.width < rect.origin.x || max.height - rect.size.height < rect.origin.y)
            throw new RangeError("invalid rectangle size: " + JSON.stringify(rect.size));
        return rect;
    }

    /**
     * @param {number} row 
     * @param {number} count 
     */
    _validateRowCount(row ,count) {

        if (!Number.isInteger(count))
            throw new TypeError("invalid row count: " + JSON.stringify(count));
        const max = this.maxSize.height
        if (count < 0 || count > max || max - count < row)
            throw new RangeError("invalid row count: " + count);
    }

    /**
     * @param {number} row 
     * @param {number} count 
     */
    _validateColumnCount(column, count) {
        if (!Number.isInteger(count))
            throw new TypeError("invalid column count: " + JSON.stringify(count));
        const max = this.maxSize.width
        if (count < 0 || count > max || max - count < column)
            throw new RangeError("invalid column count: " + count);
    }

    /**
     * @param {number} length 
     */
    _validateLength(length) {
        if (!Number.isInteger(length))
            throw new TypeError("invalid length: " + JSON.stringify(length));
        if (length < 0 || length > 0xFFFFFFFF)
            throw new RangeError("invalid length: " + length);
    }

}

Module['Sheet'] = Sheet;



