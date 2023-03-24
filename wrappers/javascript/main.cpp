// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/sheet.h>

#include <emscripten.h>
#include <emscripten/val.h>

using namespace Spreader;
using namespace emscripten;

typedef struct _StringAsPtr * StringAsPtr;

static auto makeJSSize(Size size) -> EM_VAL {
    return (EM_VAL)EM_ASM_PTR({
        return Emval.toHandle({width: $0, height: $1});
    }, size.width, size.height);
}

static auto makeJSPoint(Point pt) -> EM_VAL {
    return (EM_VAL)EM_ASM_PTR({
        return Emval.toHandle({x: $0, y: $1});
    }, pt.x, pt.y);
}

static auto makeJSRect(Rect rect) -> EM_VAL {
    return (EM_VAL)EM_ASM_PTR({
        return Emval.toHandle({
            origin: {x: $0, y: $1},
            size: {width: $2, height: $3}
        });
    }, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

static auto makeJSNull() -> EM_VAL {
    return (EM_VAL)EM_ASM_PTR({
        return Emval.toHandle(null);
    }, 0);
}

static auto makeJSBigInt(uint64_t val) -> EM_VAL {
    return (EM_VAL)EM_ASM_PTR({
        return Emval.toHandle((BigInt($0) << 32n) | BigInt($1));
    }, unsigned(val >> 32), unsigned(val));
}

static auto makeLengthInfo(const Sheet::LengthInfo & info) -> EM_VAL {
    if (info.length) {
        return (EM_VAL)EM_ASM_PTR({
            return Emval.toHandle({
                length: $0,
                hidden: $1 != 0 ? true: false
            });
        }, *info.length, int(info.hidden));
    } else {
        return (EM_VAL)EM_ASM_PTR({
            return Emval.toHandle({
                hidden: $0 != 0 ? true: false
            });
            }, int(info.hidden));
    }
}

static auto makeLengthInfoHandler(EM_VAL handler) {

    return [handler](SizeType startIdx, SizeType endIdx, const Sheet::LengthInfo & info) {
        if (info.length) {
            return EM_ASM_INT({
                const func = Emval.toValue($0);
                const res = func($1, $2, {length: $3, hidden: $4 != 0 ? true: false});
                return res !== undefined ? Boolean(res) : true;
            }, handler, startIdx, endIdx, *info.length, info.hidden) != 0;
        } else {
            return EM_ASM_INT({
                const func = Emval.toValue($0);
                const res = func($1, $2, {hidden: $3 != 0 ? true: false});
                return res !== undefined ? Boolean(res) : true;
            }, handler, startIdx, endIdx, info.hidden) != 0;
        }
    };
}

extern "C" {

    EMSCRIPTEN_KEEPALIVE auto makeSheet() -> Sheet * {
        return new Sheet;
    }

    EMSCRIPTEN_KEEPALIVE void deleteSheet(Sheet * self) noexcept {
        delete self;
    }

    EMSCRIPTEN_KEEPALIVE auto sheetSize(const Sheet * self) noexcept -> EM_VAL { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        return makeJSSize(self->size());
    }

    EMSCRIPTEN_KEEPALIVE auto sheetMaxSize() noexcept -> EM_VAL { 
        return makeJSSize(Sheet::maxSize());
    }

    EMSCRIPTEN_KEEPALIVE auto sheetNonNullCellCount(const Sheet * self) noexcept -> EM_VAL { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        return makeJSBigInt(self->nonNullCellCount());
    }

    EMSCRIPTEN_KEEPALIVE void sheetSetValueCellBool(Sheet * self, SizeType x, SizeType y, bool val) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->setValueCell({x, y}, val);
    }

    EMSCRIPTEN_KEEPALIVE void sheetSetValueCellDouble(Sheet * self, SizeType x, SizeType y, double val) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->setValueCell({x, y}, val);
    }

    EMSCRIPTEN_KEEPALIVE void sheetSetValueCellString(Sheet * self, SizeType x, SizeType y, EM_VAL val) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->setValueCell({x, y}, String(val));
    }

    EMSCRIPTEN_KEEPALIVE void sheetSetValueCellError(Sheet * self, SizeType x, SizeType y, int code) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->setValueCell({x, y}, Error(code));
    }

    EMSCRIPTEN_KEEPALIVE void sheetClearCell(Sheet * self, SizeType x, SizeType y) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->clearCell({x, y});
    }

    EMSCRIPTEN_KEEPALIVE void sheetSetFormulaCell(Sheet * self, SizeType x, SizeType y, EM_VAL val) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->setFormulaCell({x, y}, String(val));
    }

    EMSCRIPTEN_KEEPALIVE auto sheetGetValue(const Sheet * self, SizeType x, SizeType y) -> EM_VAL {
        
        if (!self) SPR_FATAL_ERROR("null sheet handle");

        return applyVisitor([](const auto & val) {

            using T = std::remove_cvref_t<decltype(val)>;

            if constexpr (std::is_same_v<T, Scalar::Blank>) {
                return (EM_VAL)EM_ASM_PTR({
                    return Emval.toHandle({value: null});
                }, 0);
            } else if constexpr (std::is_same_v<T, bool>) {
                return (EM_VAL)EM_ASM_PTR({
                    return Emval.toHandle({value: $0 ? true : false});
                }, val);
            } else if constexpr (std::is_same_v<T, Number>) {
                return (EM_VAL)EM_ASM_PTR({
                    return Emval.toHandle({value: $0});
                }, val.value());
            } else if constexpr (std::is_same_v<T, String>) {
                const auto & wrapped = val::take_ownership(val.make_js_string());
                return (EM_VAL)EM_ASM_PTR({
                    const str = Emval.toValue($0);
                    return Emval.toHandle({value: str});
                }, wrapped.as_handle());
            } else if constexpr (std::is_same_v<T, Error>) {
                return (EM_VAL)EM_ASM_PTR({
                    return Emval.toHandle({value: ErrorValue.fromCode($0)});
                }, val);
            }

        }, self->getValue({x, y}));
    }

    EMSCRIPTEN_KEEPALIVE auto sheetGetEditInfo(const Sheet * self, SizeType x, SizeType y) -> EM_VAL {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        auto maybeInfo = self->getFormulaInfo({x, y});
        if (!maybeInfo) {
            return (EM_VAL)EM_ASM_PTR({
                return Emval.toHandle({});
            }, 0);
        }
        auto & info = *maybeInfo;

        const auto & text = val::take_ownership(info.text.make_js_string());
        return (EM_VAL)EM_ASM_PTR({
            return Emval.toHandle({
                formula: {
                    text: Emval.toValue($0),
                    extent: { width: $1, height: $2 }
                }
            });
        }, text.as_handle(), info.extent.width, info.extent.height);
    }

    EMSCRIPTEN_KEEPALIVE void sheetCopyCell(Sheet * self, 
                                            SizeType x, SizeType y, 
                                            SizeType destX, SizeType destY, SizeType destWidth, SizeType destHeight) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->copyCell({x, y}, {.origin = {destX, destY}, .size = {destWidth, destHeight}});
    }

    EMSCRIPTEN_KEEPALIVE void sheetCopyCells(Sheet * self, 
                                            SizeType x, SizeType y, SizeType width, SizeType height,
                                            SizeType destX, SizeType destY) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->copyCells({.origin = {x, y}, .size = {width, height}}, {destX, destY});
    }

    EMSCRIPTEN_KEEPALIVE void sheetMoveCell(Sheet * self, 
                                            SizeType x, SizeType y,
                                            SizeType destX, SizeType destY) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->moveCell({x, y}, {destX, destY});
    }

    EMSCRIPTEN_KEEPALIVE void sheetMoveCells(Sheet * self, 
                                            SizeType x, SizeType y, SizeType width, SizeType height,
                                            SizeType destX, SizeType destY) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->moveCells({.origin = {x, y}, .size = {width, height}}, {destX, destY});
    }

    EMSCRIPTEN_KEEPALIVE void sheetDeleteRows(Sheet * self, SizeType y, SizeType count) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->deleteRows(y, count);
    }

    EMSCRIPTEN_KEEPALIVE void sheetDeleteColumns(Sheet * self, SizeType x, SizeType count) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->deleteColumns(x, count);
    }

    EMSCRIPTEN_KEEPALIVE void sheetInsertRows(Sheet * self, SizeType y, SizeType count) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->insertRows(y, count);
    }

    EMSCRIPTEN_KEEPALIVE void sheetInsertColumns(Sheet * self, SizeType x, SizeType count) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->insertColumns(x, count);
    }

    EMSCRIPTEN_KEEPALIVE void sheetSuspendRecalc(Sheet * self) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->suspendRecalc();
    }

    EMSCRIPTEN_KEEPALIVE void sheetResumeRecalc(Sheet * self) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->resumeRecalc();
    }

    EMSCRIPTEN_KEEPALIVE void sheetRecalculate(Sheet * self) { 
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->recalculate();
    }

    EMSCRIPTEN_KEEPALIVE auto sheetParseColumn(const Sheet * self, EM_VAL str) -> SizeType {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        auto res = self->parseColumn(String(str));
        return res ? *res : -1;
    }

    EMSCRIPTEN_KEEPALIVE auto sheetParseRow(const Sheet * self, EM_VAL str) -> SizeType {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        auto res = self->parseRow(String(str));
        return res ? *res : -1;
    }

    EMSCRIPTEN_KEEPALIVE auto sheetIndexToColumn(const Sheet * self, SizeType x) -> EM_VAL {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        return self->indexToColumn(x).make_js_string();
    }

    EMSCRIPTEN_KEEPALIVE auto sheetIndexToRow(const Sheet * self, SizeType y) -> EM_VAL {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        return self->indexToRow(y).make_js_string();
    }

    EMSCRIPTEN_KEEPALIVE auto sheetParsePoint(const Sheet * self, EM_VAL str) -> EM_VAL {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        auto pt = self->parsePoint(String(str));
        return pt ? makeJSPoint(*pt) : makeJSNull();
    }

    EMSCRIPTEN_KEEPALIVE auto sheetParseArea(const Sheet * self, EM_VAL str) -> EM_VAL {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        auto rect = self->parseArea(String(str));
        return rect ? makeJSRect(*rect) : makeJSNull();
    }

    EMSCRIPTEN_KEEPALIVE void sheetSetRowHeight(Sheet * self, SizeType y, SizeType count, Sheet::LengthType height) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->setRowHeight(y, count, height);
    }
    EMSCRIPTEN_KEEPALIVE void sheetClearRowHeight(Sheet * self, SizeType y, SizeType count) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->clearRowHeight(y, count);
    }
    EMSCRIPTEN_KEEPALIVE void sheetHideRow(Sheet * self, SizeType y, SizeType count, bool hidden) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->hideRow(y, count, hidden);
    }
    EMSCRIPTEN_KEEPALIVE auto sheetGetRowHeight(const Sheet * self, SizeType y) -> EM_VAL {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        auto & ret = self->getRowHeight(y);
        return makeLengthInfo(ret);
    }
    EMSCRIPTEN_KEEPALIVE auto sheetForEachRowHeight(const Sheet * self, SizeType y, SizeType count, EM_VAL handler) -> bool {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        return self->forEachRowHeight(y, count, makeLengthInfoHandler(handler));
    }

    EMSCRIPTEN_KEEPALIVE void sheetSetColumnWidth(Sheet * self, SizeType x, SizeType count, Sheet::LengthType width) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->setColumnWidth(x, count, width);
    }
    EMSCRIPTEN_KEEPALIVE void sheetClearColumnWidth(Sheet * self, SizeType x, SizeType count) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->clearColumnWidth(x, count);
    }
    EMSCRIPTEN_KEEPALIVE void sheetHideColumn(Sheet * self, SizeType x, SizeType count, bool hidden) {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        self->hideColumn(x, count, hidden);
    }
    EMSCRIPTEN_KEEPALIVE auto sheetGetColumnWidth(const Sheet * self, SizeType x) -> EM_VAL {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        auto & ret = self->getColumnWidth(x);
        return makeLengthInfo(ret);
    }
    EMSCRIPTEN_KEEPALIVE auto sheetForEachColumnWidth(const Sheet * self, SizeType x, SizeType count, EM_VAL handler) -> bool {
        if (!self) SPR_FATAL_ERROR("null sheet handle");
        return self->forEachColumnWidth(x, count, makeLengthInfoHandler(handler));
    }

}

