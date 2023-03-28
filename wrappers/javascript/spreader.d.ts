// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

declare namespace Spreader {
    /**
     * Coordinate of a cell in a sheet
     * 
     * This is the "natural" format for cell coordinates in this library.
     * All coordinates are returned using this type though for input you can
     * use other formats. See {@link CellCoord}
     * */
    type Point = {
        x: number;
        y: number;
    };

    /**
     * A size of an area in a sheet
     */
    type Size = {
        width: number;
        height: number;
    };

    /**
     * A rectangular area in a sheet.
     * 
     * This is the "natural" format for areas in this library.
     * All areas are returned using this type though for input you can
     * use other formats. See {@link AreaCoord}
     */
    type Rect = {
        /** Top-left corner of the area */
        origin: Point;
        size: Size;
    };

    /**
     * Possible ways to represent cell coordinates in a sheet
     * 
     * A cell coordinate can be can be passed to this library:
     * - As a string in the usual "A1", "B2" format. 
     * - As a {@link Point} object 
     * - As an [x, y] array
     * 
     * The later two ways are slightly faster since they don't require 
     * parsing the string representation.
     */
    type CellCoord = string | number[] | Point;

    /**
     * Possible ways to represent rectangular area coordinates in a sheet
     * 
     * An area can be passed to this library in the following ways:
     * - As a {@link CellCoord} indicating an area that consists of a single cell
     * - As a string in the usual "A1:B2" format.
     * - As an [x, y, width, height] array
     * - As a {@link Rect} object 
     * 
     * The array/{@link Point}/{@link Rect} ways are slightly faster 
     * than passing strings since they don't require parsing the string representation.
     */
    type AreaCoord = CellCoord | number[] | Rect;

    /**
     * Possible ways to represent columns in a sheet
     * 
     * A column can be passed to this library in the following ways:
     * - As a column index: 0-based numbers (0 for "A", 1 for "B" etc.)
     * - As a column name: strings "A", "B", "C" and so on
     */
    type ColumnCoord = string | number;

    /**
     * Possible ways to represent a row in a sheet
     * 
     * A row can be passed to this library in the following ways:
     * - As a row index: 0-based numbers
     * - As a row name: strings "1", "2", "3" and so on
     */
    type RowCoord = string | number;

    /**
     * A value of a single cell
     * 
     * A cell can contain:
     * - `null` for a blank cell
     * - A `boolean` value
     * - A `number` which is never NaN or infinite
     * - A `string`. 
     * - One of {@link ErrorValue} enumeration values
     */
    type Scalar = null | boolean | number | string | ErrorValue;

    /**
     * Information about cell content needed for editing it
     * 
     * Currently only one field is defined: formula which describes
     * whether the cell contains a formula and information about it.
     * Additional fields will be added in the future
     * 
     * See {@link Sheet.getEditInfo}
     */
    type EditInfo = {
        /**If present, indicates that the cell contains formula */
        formula?: {
            /** Formula text */
            text: string;
            /**
             * Extent of the formula's dynamic array, if any.
             * If the formula is not an array the size is {width:1, height: 1}
             * */
            extent: Size
        }
    };


    type LengthInfo = {
        length?: number, 
        hidden: boolean
    };

    type LengthInfoHandler = (startIdx: number, endIdx: number, info: LengthInfo) => boolean|void;

    /**
     * Error values in cells
     * 
     * Errors can be produced as a result of formula calculations or manually
     * set as a cell's value. These correspond to the usual spreadsheet errors 
     * such as "#REF!" etc.
     * 
     * The set of known errors exposed by this class is stable but not closed. 
     * Additional errors might be added in the future. Thus if you serialize sheet 
     * data or interchange it with other libraries or even versions of this library 
     * you should be prepared to deal with errors not defined here.
     */
    class ErrorValue {
        /**The numerical value of the error. 
         * These correspond to ones returned by [ERROR.TYPE](https://support.microsoft.com/en-us/office/error-type-function-10958677-7c8d-44f7-ae77-b9a9ee6eefaa)
         * function.
         */
        code: number;

        /**
         * Construct an error not explicitly defined below
         * @param code Error code
         */
        constructor(code: number);

        /** Represents `#NULL!` error */
        static NullRange: ErrorValue;
        /** Represents `#DIV/0!` error */
        static DivisionByZero: ErrorValue;
        /** Represents `#VALUE!` error */
        static InvalidValue: ErrorValue;
        /** Represents `#REF!` error */
        static InvalidReference: ErrorValue;
        /** Represents `#NAME?` error */
        static InvalidName: ErrorValue;
        /** Represents `#NUM!` error */
        static NotANumber: ErrorValue;
        /** Represents `#N/A` error */
        static InvalidArgs: ErrorValue;
        /** Represents `#GETTING_DATA` error */
        static GettingData: ErrorValue;
        /** Represents `#SPILL!` error */
        static Spill: ErrorValue;

        /** Represents an error in parsing a formula.
         * 
         * This is not an Excel compatible error.
        */
        static InvalidFormula: ErrorValue;
    }

    /**
     * A spreadsheet
     */
    class Sheet {
        constructor();

        /**
         * Destroys the object
         * 
         * You must call this method after finishing using a Sheet in order to reclaim
         * its WebAssembly resources. The object cannot be used after `delete()` has been
         * called.
         */
        delete(): void;
        
        /**
         * Returns the size of the sheet
         * 
         * This is a method, not a property, to indicate that evaluating it involves a
         * WebAssembly call. In a high-performance scenarios you should store the returned value
         * in a variable rather than call this method repeatedly (unless you change the size).
         */
        size(): Size;

        /**
         * Returns maximum possible size of a sheet
         */
        static maxSize: Size;

        /**
         * Returns maximum possible size of a sheet. Same as {@link Sheet.maxSize}
         * 
         * This instance property is provided for convenience as an alias to static property
         */
        maxSize: Size;

        /**
         * Returns the number of non-null cells in the sheet.
         * 
         * This is mainly a debugging/troubleshooting aid.
         */
        nonNullCellCount(): bigint;
        
        //#region Cell Access
        
        /**
         * Stores a scalar value (rather than a formula) in a cell.
         * 
         * If the `value` is `null` this is equivalent to {@link clearCellValue}
         * When `value` is a `string` it should never be escaped. This function
         * stores the argument as-is and does no input parsing.
         *  
         * @param coord coordinate of the cell
         * @param value value to store
         */
        setValueCell(coord: CellCoord, value: Scalar): void;

        /**
         * Stores a formula (rather than a scalar value) in a cell.
         * 
         * There is no error if the formula fails to parse. Instead it is 
         * stored in the cell and evaluates to {@link ErrorValue.InvalidFormula}
         * 
         * @param coord coordinate of the cell
         * @param formula the formula text. Do not prefix the formula with '='. 
         */
        setFormulaCell(coord: CellCoord, formula: string): void;

        /**
         * Clears contents of the cell
         * 
         * This erases cell content (whether a value or a formula) and sets
         * the cell value to `null`. It is equivalent to `setValueCell(coord, null)`.
         * 
         * @param coord  coordinate of the cell
         */
        clearCellValue(coord: CellCoord): void;

        /**
         * Retrieves cell's value
         * 
         * For value cells the value is the one stored in it. For formula cells it is
         * the evaluated result.
         * 
         * @param coord coordinate of the cell
         */
        getValue(coord: CellCoord): Scalar;

        /**
         * Retrieves information about cell of interest when editing it.
         * 
         * This roughly corresponds to the information spreadsheet UI would need
         * when user selects a cell e.g. presence of formula and its text, extent
         * of array formula etc.
         * This call is relatively expensive and is expected to be called "rarely"
         * on one cell at a time rather than in bulk.
         * 
         * @param coord coordinate of the cell
         */
        getEditInfo(coord: CellCoord): EditInfo;

        //#endregion

        //#region Copy/Move

        /**
         * Copies a single cell to a destination area
         * 
         * This is equivalent to copy/paste operation in GUI spreadsheets.
         * The destination area can, of course, be a single cell
         * 
         * @param from the coordinate to copy from
         * @param to the area to copy to
         */
        copyCell(from: CellCoord, to: AreaCoord): void;

        /** 
         * Copies an area of cells to another location.
         * 
         * This is equivalent to copy/paste operation in GUI spreadsheets.
         * The source are can, of course, be a single cell in which case
         * this call is equivalent to `copyCell(from ,to)`
         *  
         * @param from the area to copy
         * @param to the top left corner of the location to copy to
         */
        copyCells(from: AreaCoord, to: CellCoord): void;

        /**
         * Moves a single cell to another location.
         * 
         * This is equivalent to cut/paste operation in GUI spreadsheets.
         * 
         * @param from the coordinate to move from
         * @param to the coordinate to move to
         */
        moveCell(from: CellCoord, to: CellCoord): void;

        /**
         * Moves an area of cells to another location
         * 
         * This is equivalent to cut/paste operation in GUI spreadsheets.
         * The source are can, of course, be a single cell in which case
         * this call is equivalent to `moveCell(from ,to)`
         * 
         * @param from the are to move from
         * @param to the top left corner of the destination to move to
         */
        moveCells(from: AreaCoord, to: CellCoord): void;

        //#endregion

        //#region Insert/Delete

        /**
         * Deletes rows from the sheet
         * 
         * @param y row to start deletion from
         * @param count number of rows to delete. 0 is a valid value and results in a no-op
         */
        deleteRows(y: RowCoord, count: number): void;

        /**
         * Deletes columns from the sheet
         * 
         * @param x column to start deletion from
         * @param count number of columns to delete. 0 is a valid value and results in a no-op
         */
        deleteColumns(x: ColumnCoord, count: number): void;

        /**
         * Inserts new rows into the sheet
         * 
         * @param y coordinate of the row **before** which to perform the insertion
         * @param count number of rows to insert. 0 is a valid value and results in a no-op
         */
        insertRows(y: RowCoord, count: number): void;

        /**
         * Inserts new columns into the sheet
         * 
         * @param y coordinate of the column **before** which to perform the insertion
         * @param count number of columns to insert. 0 is a valid value and results in a no-op
         */
        insertColumns(x: ColumnCoord, count: number): void;

        //#endregion
        
        //#region Recalculation

        /**
         * Suspends automatic recalculation of the sheet
         * 
         * You call this method multiple times. To undo the suspension
         * call {@link resumeRecalc} the same number of times. 
         */
        suspendRecalc(): void;

        /**
         * Resumes automatic recalculation of the sheet
         * 
         * This is the reverse of {@link suspendRecalc}
         */
        resumeRecalc(): void;

        /**
         * Manually recalculates the sheet
         * 
         * This method allows you to manually recalculate the sheet when automatic 
         * recalculation is suspended by a call to {@link suspendRecalc}
         */
        recalculate(): void;

        //#endregion

        //#region Coordinate transformations

        /**
         * Converts column index to a column name
         * 
         * 0 is converted to "A", 1 to "B" and so on.
         * 
         * @param x 0-based column index
         */
        indexToColumn(x: number): string;

        /**
         * Converts row index to a row name
         * 
         * Currently this function exists mainly for symmetry with
         * {@link indexToColumn}. It converts 0 to "1", 1 to "2" and so on
         * which you can easily do yourself.
         * However, in the future, the library might gain ability to give
         * friendly names to rows in a sheet. In this case, this method will
         * become necessary.
         * 
         * @param x 0-based row index
         */
        indexToRow(x: number): string;

        /**
         * Converts column names to indices, if possible
         * 
         * @param str column name like "A" or "AB"
         * @returns corresponding column index if the name is valid or `null` otherwise
         */
        parseColumn(str:string): number | null;

        /**
         * Converts row names to indices, if possible
         * 
         * Currently this function exists mainly for symmetry with
         * {@link parseColumn}. It converts "1" to 0, "2" to 1 and so on
         * which you can easily do yourself.
         * However, in the future, the library might gain ability to give
         * friendly names to rows in a sheet. In this case, this method will
         * become necessary.
         * 
         * @param str row name like "1" or "5"
         * @returns corresponding row index if the name is valid or `null` otherwise
         */
        parseRow(str:string): number | null;

        /**
         * Converts string cell coordinates to instances of {@link Point}, if possible
         * 
         * @param str cell coordinate like "A1" or "B5"
         * @returns corresponding {@link Point} if the input is valid or `null` otherwise
         */
        parsePoint(str: string): Point | null;

        /**
         * Converts string area coordinates to instances of {@link Rect}, if possible
         * 
         * Note that an area could be a single cell like "A1", in which case the output will
         * be a {@link Rect} of size `{width:1, height:1}` or a true area like "A1:B2".
         * 
         * @param str area coordinate like "A1:C3" or "B5"
         * @returns corresponding {@link Rect} if the input is valid or `null` otherwise
         */
        parseArea(str: string): Point | null;

        //#endregion

        //#region Row Heights

        setRowsHeight(y: RowCoord, count: number, height: number): void;
        clearRowsHeight(y: RowCoord, count: number): void;
        hideRows(y: RowCoord, count: number, hidden: boolean): void;
        getRowHeight(y: RowCoord): LengthInfo;
        forEachRowHeight(y: RowCoord, count: number, func: LengthInfoHandler): boolean;

        //#endregion

        //#region Column Widths

        setColumnsWidth(x: ColumnCoord, count: number, width: number): void;
        clearColumnsWidth(x: ColumnCoord, count: number): void;
        hideColumns(x: ColumnCoord, count: number, hidden: boolean): void;
        getColumnWidth(x: ColumnCoord): LengthInfo;
        forEachColumnWidth(x: ColumnCoord, count: number, func: LengthInfoHandler): boolean;

        //#endregion
    }

    /**
     * Spreader WebAssembly module.
     * 
     * The module is loaded asynchronously and exposes all the types
     * in this library.
     */
    class Module {
        
        ErrorValue: typeof ErrorValue;
        Sheet: typeof Sheet
    }
}

export = Spreader;
/**
 * Obtains an instance of {@link Spreader.Module}
 * 
 * This is the main entry point of this library. Due to the fact that this 
 * library load WebAssembly it is asynchronous.
 * 
 * @param module reserved, do not use
 * @param args  reserved, do not use
 */
declare function Spreader(module?: any, ...args: any[]): Promise<Spreader.Module>;


declare namespace Spreader {
    export { Spreader };
}
