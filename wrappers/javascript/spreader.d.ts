// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

declare namespace Spreader {
    type Point = {
        x: number;
        y: number;
    };
    type Size = {
        width: number;
        height: number;
    };
    type Rect = {
        origin: Point;
        size: Size;
    };
    type CellCoord = string | number[] | Point;
    type AreaCoord = CellCoord | Rect;
    type Scalar = null | boolean | number | string | ErrorValue;

    type EditInfo = {
        formula?: {
            text: string;
            extent: Size
        }
    };

    type LengthInfo = {
        length?: number, 
        hidden: boolean
    };

    type LengthInfoHandler = (startIdx: number, endIdx: number, info: LengthInfo) => boolean|void;

    class ErrorValue {
        code: number;

        constructor(code: number);

        static InvalidValue: ErrorValue;
        static InvalidName: ErrorValue;
        static InvalidArgs: ErrorValue;
        static InvalidFormula: ErrorValue;
        static InvalidReference: ErrorValue;
        static NotANumber: ErrorValue;
        static DivisionByZero: ErrorValue;
        static NullRange: ErrorValue;
        static Spill: ErrorValue;
    }

    class Sheet {
        constructor();

        /**
         * Destroys Sheet object
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
         * in a variable rather than call this method repeatedly (unless you cange the size).
         */
        size(): Size;

        /**
         * Returns maxium possible size of a sheet
         */
        static maxSize: Size;

        /**
         * Returns maxium possible size of a sheet. Same as {@link Sheet.maxSize}
         * 
         * This instance property is provided for convenience as an alias to static property
         */
        maxSize: Size;

        /**
         * Returns the number of non-null cells in the sheet
         */
        nonNullCellCount(): bigint;
        
        setValueCell(coord: CellCoord, value: Scalar): void;
        setFormulaCell(coord: CellCoord, formula: string): void;
        clearCellValue(coord: CellCoord): void;

        getValue(coord: CellCoord): Scalar
        getEditInfo(coord: CellCoord): EditInfo;

        copyCell(from: CellCoord, to: AreaCoord): void;
        copyCells(from: AreaCoord, to: CellCoord): void;
        moveCell(from: CellCoord, to: CellCoord): void;
        moveCells(from: AreaCoord, to: CellCoord): void;

        deleteRows(y: string|number, count: number): void;
        deleteColumns(x: string|number, count: number): void;
        insertRows(y: string|number, count: number): void;
        insertColumns(x: string|number, count: number): void;

        suspendRecalc(): void;
        resumeRecalc(): void;
        recalculate(): void;

        indexToColumn(x: number): string;
        indexToRow(x: number): string;

        parseColumn(str:string): number | null;
        parseRow(str:string): number | null;

        parsePoint(str: string): Point | null;
        parseArea(str: string): Point | null;

        setRowsHeight(y: string|number, count: number, height: number): void;
        clearRowsHeight(y: string|number, count: number): void;
        hideRows(y: string|number, count: number, hidden: boolean): void;
        getRowHeight(y: string|number): LengthInfo;
        forEachRowHeight(y: string|number, count: number, func: LengthInfoHandler): boolean;

        setColumnsWidth(x: string|number, count: number, width: number): void;
        clearColumnsWidth(x: string|number, count: number): void;
        hideColumns(x: string|number, count: number, hidden: boolean): void;
        getColumnWidth(x: string|number): LengthInfo;
        forEachColumnWidth(x: string|number, count: number, func: LengthInfoHandler): boolean;
    }

    class Module {
        
        ErrorValue: typeof ErrorValue;
        Sheet: typeof Sheet
    }
}

export = Spreader;
declare function Spreader(module?: any, ...args: any[]): Promise<Spreader.Module>;


declare namespace Spreader {
    export { Spreader };
}
