// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/sheet.h>
//#include <spreader/mru-cache.h>

#include "formula-evaluator.h"

#include <list>
#include <map>

using namespace Spreader;

struct Sheet::ReplaceOldExtensionCell {

    #ifdef NDEBUG
    constexpr
    #endif
    auto modifiesMissing() noexcept -> bool { SPR_ASSERT_LOGIC(false); return true; }

    SPR_ALWAYS_INLINE auto operator()(CellPtr & cell) -> int {
        SPR_ASSERT_LOGIC(cell && cell->getType() == CellType::FormulaExtension);
        
        cell->setValue(std::move(value));
        return 0;
    }

    Scalar value;
};

struct Sheet::ReserveNewExtensionCell {
    constexpr auto modifiesMissing() noexcept -> bool { return true; }

    SPR_ALWAYS_INLINE auto operator()(CellPtr & cell) -> int {

        SPR_ASSERT_LOGIC(!cell ||
                            (cell->getType() == CellType::Value &&
                            get<Scalar::Blank>( &static_cast<ValueCell *>(cell.get())->value() )));
        
        int ret = -int(bool(cell));
        cell = FormulaCellExtension::create(formulaCell, Scalar::Blank{});
        return ret + 1;
    }
    
    FormulaCell * formulaCell;
};

struct Sheet::ClearExtensionCell {
    #ifdef NDEBUG
    constexpr
    #endif
    auto modifiesMissing() noexcept -> bool { SPR_ASSERT_LOGIC(false); return false; }

    auto operator()(CellPtr & cell) noexcept -> int {
        
        SPR_ASSERT_LOGIC(cell && cell->getType() == CellType::FormulaExtension);
        int ret = -int(bool(cell));
        cell = nullptr;
        return ret;
    }
};


void Sheet::recalculate() {

    m_evalGeneration = !m_evalGeneration;

    AstNode::ExecutionMemoryResource memory;

    //using EvaluatorCache = MRUCache<FormulaCell *, FormulaEvaluator>;
    //EvaluatorCache cachedEvaluators(EvaluatorCache::MaxItemCount, 1024);
    for(auto formulaCellIt = m_formulaCells.begin(), formulaCellEnd = m_formulaCells.end(); formulaCellIt != formulaCellEnd; ) {

        auto formulaCell = &*formulaCellIt;
        
        if (!formulaCell->needsRecalc(m_evalGeneration)) {
            ++formulaCellIt;
            continue;
        }
        
        
//        auto evaluatorIt = cachedEvaluators.emplace(formulaCell,
//                                                    memory,
//                                                    m_grid,
//                                                    formulaCell->formula(),
//                                                    formulaCell->references(),
//                                                    formulaCell->location(),
//                                                    m_evalGeneration).first;

        
        formulaCell->setBeingCalculated(true);
        
        //auto & evaluator = evaluatorIt->second;
        auto evaluator = FormulaEvaluator(memory,
                                          m_grid,
                                          m_nameManager,
                                          formulaCell->formula(),
                                          formulaCell->references(),
                                          formulaCell->location(),
                                          m_evalGeneration);
        
        ExecutionContext::InvocableHandler handler([&](FormulaCell * dependency) {
            m_formulaCells.erase(*dependency);
            formulaCellIt = m_formulaCells.insert(formulaCellIt, *dependency);
        });

        evaluator.setDependencyHandler(&handler);
        
        if (evaluate(formulaCell, evaluator)) {
            formulaCell->finishCalculation(m_evalGeneration, evaluator.isCircularDependency());
            //cachedEvaluators.erase(evaluatorIt);
        } /*else {
            evaluator.setDependencyHandler(nullptr);
        }*/
        if (&*formulaCellIt == formulaCell)
            ++formulaCellIt;
    }

    //SPR_ASSERT_LOGIC(cachedEvaluators.empty());
}

auto Sheet::evaluate(FormulaCell * formulaCell, FormulaEvaluator & evaluator) -> bool {
    
    do {
        
        if (!evaluator.eval())
            return false;
        
        if (evaluator.isCircularDependency()) {
            removeFormulaDependents(formulaCell);
            formulaCell->setValue(Error::InvalidReference);
            return true;
        }
        
        if (auto offset = evaluator.offset(); offset == Point{0, 0}) {
            reserveExtent(formulaCell, evaluator.extent());
            formulaCell->setValue(std::move(evaluator.result()));
        } else {
            m_grid.modifyCell(formulaCell->location() + asSize(offset), ReplaceOldExtensionCell{std::move(evaluator.result())});
        }
    } while (evaluator.nextOffset());
    
    return true;
}

void Sheet::reserveExtent(FormulaCell * formulaCell, Size newExtent) {

    const Point at = formulaCell->location();
    Size oldExtent = formulaCell->extent();
    if (oldExtent == newExtent)
        return;
    
    formulaCell->setExtent(newExtent);
    
    auto commonHeight = std::min(newExtent.height, oldExtent.height);
    auto commonWidth = std::min(newExtent.width, oldExtent.width);
    
    Point pt;
    for (pt.y = 0; pt.y < commonHeight; ++pt.y) {
        for(pt.x = commonWidth; pt.x < newExtent.width; ++pt.x) {
            m_grid.modifyCell(at + asSize(pt), ReserveNewExtensionCell{formulaCell});
        }
        for( ; pt.x < oldExtent.width; ++pt.x) {
            m_grid.modifyCell(at + asSize(pt), ClearExtensionCell{});
        }
    }
    for( ; pt.y < newExtent.height; ++pt.y) {
        for (pt.x = 0; pt.x < newExtent.width; ++pt.x) {
            m_grid.modifyCell(at + asSize(pt), ReserveNewExtensionCell{formulaCell});
        }
    }
    for( ; pt.y < oldExtent.height; ++pt.y) {
        for(pt.x = 0 ; pt.x < oldExtent.width; ++pt.x) {
            m_grid.modifyCell(at + asSize(pt), ClearExtensionCell{});
        }
    }
}


void Sheet::removeFormulaDependents(FormulaCell * formulaCell) {
    const Point at = formulaCell->location();
    Size extent = formulaCell->extent();
    if (extent == Size{1, 1})
        return;
    if (maxSize().width - at.x < extent.width)
        extent.width = maxSize().width - at.x;
    if (maxSize().height - at.y < extent.height)
        extent.height = maxSize().height - at.y;

    m_grid.modifyCells({.origin= at + Size{1, 0}, .size = {extent.width - 1, 1}}, ClearExtensionCell{});
    m_grid.modifyCells({.origin= at + Size{0, 1}, .size = {extent.width, extent.height - 1}}, ClearExtensionCell{});
    formulaCell->setExtent(Size{1, 1});
}


struct Sheet::SetBlankCell {
    constexpr auto modifiesMissing() noexcept -> bool { return false; }

    auto operator()(CellPtr & cell) -> int {
        
        return applyToCell(cell, [this, &cell](auto ptr)  {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                return 0;
            } else {
                if constexpr (std::is_same_v<T, FormulaCell>) {
                    me->m_formulaCells.erase(*ptr);
                    me->removeFormulaDependents(ptr);
                } 

                cell = nullptr;
                return -int(!std::is_same_v<T, nullptr_t>);
            }
        });
    }

    Sheet * me;
};

struct Sheet::SetValueCell {
    constexpr auto modifiesMissing() noexcept -> bool { return true; }

    auto operator()(CellPtr & cell) -> int {
        
        return applyToCell(cell, [this, &cell](auto ptr)  {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, ValueCell>) {
                ptr->setValue(value);
                return 0;
            } else {
                if constexpr (std::is_same_v<T, FormulaCell>) {
                    me->m_formulaCells.erase(*ptr);
                    me->removeFormulaDependents(ptr);
                } else if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                    me->removeFormulaDependents(ptr->parent());
                } 
                cell = ValueCell::create(value);
                return -int(!std::is_same_v<T, nullptr_t>) + 1;
            }
        });
    }

    Sheet * me;
    const Scalar & value;
};

struct Sheet::SetFormulaCell {
    constexpr auto modifiesMissing() noexcept -> bool { return true; }

    auto operator()(CellPtr & cell) -> int {
        
        return applyToCell(cell, [this, &cell](auto ptr) {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, FormulaCell>) {
                ptr->replaceFormula(std::move(code), std::move(references));
                return 0;
            } else {
                if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                    me->removeFormulaDependents(ptr->parent());
                }

                auto newCell = FormulaCell::create(std::move(code), std::move(references), coord, me->m_evalGeneration);
                me->m_formulaCells.push_back(*newCell);
                cell = std::move(newCell);
                return -int(!std::is_same_v<T, nullptr_t>) + 1;
            }
        });
    }

    Sheet * me;
    Point coord;
    FormulaPtr & code;
    FormulaReferencesPtr & references;
};


void Sheet::setValueCell(Point coord, const Scalar & value) {
    if (get<Scalar::Blank>(&value)) {
        m_grid.modifyCell(coord, SetBlankCell{this});
    } else {
        m_grid.modifyCell(coord, SetValueCell{this, value});
    }
    recalcIfNotSuspended();
}

void Sheet::clearCell(Point coord) {
    m_grid.modifyCell(coord, SetBlankCell{this});
    recalcIfNotSuspended();
}

void Sheet::setFormulaCell(Point coord, const String & formula) {

    auto [code, references] = Formula::parse(formula, coord);
    m_grid.modifyCell(coord, SetFormulaCell{this, coord, code, references});
    recalcIfNotSuspended();
}

struct Sheet::CopyCell {

    constexpr auto modifiesMissingSource() noexcept -> bool { return false; }
    auto modifiesMissingDestination() noexcept -> bool { return bool(copiedCell); }

    void setPoints(Point /*src*/, Point dst) noexcept {
        destination = dst;
    }

    auto get(const CellPtr & cell) -> int {
        applyToCell(cell, [this](auto ptr) {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, ValueCell>) {
                copiedCell = ptr->copy();
            } else if constexpr (std::is_same_v<T, FormulaCell>) {
                copiedCell = ptr->copy(destination);
                me->m_formulaCells.push_back(static_cast<FormulaCell &>(*copiedCell));
            }
        });
        return 0;
    }

    auto set(CellPtr & cell) noexcept -> int {
        return applyToCell(cell, [this, &cell](auto ptr) {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, FormulaCell>) {
                me->m_formulaCells.erase(*ptr);
                //this is safe because extension cells are ignored when reading
                me->removeFormulaDependents(ptr);
            } else if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                if (!copiedCell)
                    return 0;
                me->removeFormulaDependents(ptr->parent());
            }

            int ret = -int(bool(cell)) + int(bool(copiedCell));
            cell = std::move(copiedCell);
            return ret;
        });
    }

    Sheet * me;
    CellPtr copiedCell{}; 
    Point destination{};
};


void Sheet::copyCell(Point from, Rect to) {

    m_grid.transformCell(from, to, CopyCell{this});
    recalcIfNotSuspended();
}

void Sheet::copyCells(Rect from, Point to) {

    m_grid.transformCells(from, to, CopyCell{this});
    recalcIfNotSuspended();
}

struct Sheet::MoveCell {

    constexpr auto modifiesMissingSource() noexcept -> bool { return false; }
    auto modifiesMissingDestination() noexcept -> bool { return bool(movedCell); }

    void setPoints(Point /*src*/, Point dst) noexcept {
        destination = dst;
    }

    auto get(CellPtr & cell) noexcept -> int {

        return applyToCell(cell, [this, &cell](auto ptr) {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                return 0;
            } else {
                if constexpr (std::is_same_v<T, FormulaCell>) {
                    ptr->move(destination);
                    //this is safe because extension cells are ignored when reading
                    me->removeFormulaDependents(ptr);
                }
                movedCell = std::move(cell);
                return -int(!std::is_same_v<T, nullptr_t>);
            }
        });
    }

    auto set(CellPtr & cell) noexcept -> int {

        return applyToCell(cell, [this, &cell](auto ptr) {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                if (movedCell) {
                    me->removeFormulaDependents(ptr->parent());
                    cell = std::move(movedCell);
                }
                return 0;
            } else {
                if constexpr (std::is_same_v<T, FormulaCell>) {
                    me->m_formulaCells.erase(*ptr);
                    //this is safe because extension cells are ignored when reading
                    me->removeFormulaDependents(ptr);
                } 
                cell = std::move(movedCell);
                return -int(!std::is_same_v<T, nullptr_t>) + int(bool(cell));
            }
        });
    }


    Sheet * me;
    CellPtr movedCell{}; 
    Point destination{};
};

void Sheet::moveCell(Point from, Point to) {

    m_grid.transformCell(from, Rect{to, Size{1, 1}}, MoveCell{this});
    //TODO: adjust incoming references
    recalcIfNotSuspended();
}

void Sheet::moveCells(Rect from, Point to) {

    m_grid.transformCells(from, to, MoveCell{this});
    //TODO: adjust incoming references
    recalcIfNotSuspended();
}

struct Sheet::SimpleMoveCell {
    constexpr auto modifiesMissingSource() noexcept -> bool { return false; }
    auto modifiesMissingDestination() noexcept -> bool { return bool(movedCell); }
    
    void setPoints(Point /*src*/, Point /*dst*/) noexcept {}
    auto get(CellPtr & cell) noexcept -> int {
        int ret = -int(bool(cell));
        movedCell = std::move(cell);
        return ret;
    }
    auto set(CellPtr & cell) noexcept -> int {
        int ret = -int(bool(cell)) + int(bool(movedCell));
        cell = std::move(movedCell);
        return ret;
    }

    CellPtr movedCell;
};

void Sheet::deleteRows(SizeType y, SizeType count) {

    SPR_ASSERT_INPUT(y < maxSize().height && count <= maxSize().height && maxSize().height - count >= y);

    if (count == 0)
        return;

    for(auto it = m_formulaCells.begin(), end = m_formulaCells.end(); it != end; ) {
        auto formula = &*it;
        auto formulaLoc = formula->location();
        if (formulaLoc.y >= y && formulaLoc.y < y + count) {
            //if formula is in the deletion area get rid of all extra records for it
            removeFormulaDependents(formula);
            it = m_formulaCells.erase(it);
        } else {
            
            if (formulaLoc.y < y) {
                //formula is above deletion area
                auto extent = formula->extent();
                if (formulaLoc.y + extent.height > y) {
                    //if extent intersects the deletion area, adjust extent
                    extent.height -= std::min(count, formulaLoc.y + extent.height - y);
                    formula->setExtent(extent);
                }
            } else {
                //since "above" and "inside deletion area" is covered above this leaves "below deletion area" for this else
                //adjust formula location
                formula->setLocation(Point{formulaLoc.x, formulaLoc.y - count});
            }
            
            //adjust references, if any
            if (auto refs = formula->references()) {
                if (auto adjusted = refs->adjustToRowDeletion(formulaLoc.y, y, count); adjusted != refs) {
                    formula->replaceReferences(std::move(adjusted));
                }
            }
            
            ++it;
        }
    }

    m_grid.deleteRows(y, count, SimpleMoveCell{});
    m_rowHeights.eraseIndices(y, y + count);

    recalcIfNotSuspended();
}

void Sheet::deleteColumns(SizeType x, SizeType count) {

    SPR_ASSERT_INPUT(x < maxSize().width && count <= maxSize().width && maxSize().width - count >= x);

    if (count == 0)
        return;

    for(auto it = m_formulaCells.begin(), end = m_formulaCells.end(); it != end; ) {
        auto formula = &*it;
        auto formulaLoc = formula->location();
        if (formulaLoc.x >= x && formulaLoc.x < x + count) {
            //if formula is in the deletion area get rid of all extra records for it
            removeFormulaDependents(formula);
            it = m_formulaCells.erase(it);
        } else {
            
            if (formulaLoc.x < x) {
                //formula is to the left of deletion area
                auto extent = formula->extent();
                if (formulaLoc.x + extent.width > x) {
                    //if extent intersects the deletion area, adjust extent
                    extent.width -= std::min(count, formulaLoc.x + extent.width - x);
                    formula->setExtent(extent);
                }
            } else {
                //since "to the left" and "inside deletion area" is covered above this leaves "to the right og deletion area" for this else
                //adjust formula location
                formula->setLocation(Point{formulaLoc.x - count, formulaLoc.y});
            }
            
            //adjust references, if any
            if (auto refs = formula->references()) {
                if (auto adjusted = refs->adjustToColumnDeletion(formulaLoc.x, x, count); adjusted != refs) {
                    formula->replaceReferences(std::move(adjusted));
                }
            }
            
            ++it;
        }
    }
    m_grid.deleteColumns(x, count, SimpleMoveCell{});
    m_columnWidths.eraseIndices(x, x + count);

    recalcIfNotSuspended();
}

struct Sheet::InsertionMoveCell {
    constexpr auto modifiesMissingSource() noexcept -> bool { return false; }
    auto modifiesMissingDestination() noexcept -> bool { return bool(movedCell); }
    
    void setPoints(Point /*src*/, Point /*dst*/) noexcept {}
    auto get(CellPtr & cell) noexcept -> int {
        return applyToCell(cell, [this, &cell](auto ptr) {

            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, FormulaCellExtension>) {
                return 0;
            } else {
                movedCell = std::move(cell);
                return -int(!std::is_same_v<T, nullptr_t>);
            }
        });
    }
    auto set(CellPtr & cell) noexcept -> int {
        int ret = -int(bool(cell)) + int(bool(movedCell));
        cell = std::move(movedCell);
        return ret;
    }

    CellPtr movedCell;
};

void Sheet::insertRows(SizeType y, SizeType count) {

    //Unlike deletion here we first insert (skipping extensions), then adjust.  
    m_grid.insertRows(y, count, InsertionMoveCell{});

    for(auto it = m_formulaCells.begin(), end = m_formulaCells.end(); it != end; ++it) {
        auto formula = &*it;
        auto formulaLoc = formula->location();

        if (formulaLoc.y >= y) {
            //formula is below insertion area
            formula->setLocation(Point{formulaLoc.x, formulaLoc.y + count});
        }
        
        //adjust references, if any
        if (auto refs = formula->references()) {
            if (auto adjusted = refs->adjustToRowInsertion(formulaLoc.y, y, count); adjusted != refs) {
                formula->replaceReferences(std::move(adjusted));
            }
        }
    }
    
    m_rowHeights.insertIndices(y, count);

    recalcIfNotSuspended();
}

void Sheet::insertColumns(SizeType x, SizeType count) {

    //Unlike deletion here we first insert (skipping extensions), then adjust.  
    m_grid.insertColumns(x, count, InsertionMoveCell{});

    for(auto it = m_formulaCells.begin(), end = m_formulaCells.end(); it != end; ++it) {
        auto formula = &*it;
        auto formulaLoc = formula->location();

        if (formulaLoc.x >= x) {
            //formula is below insertion area
            formula->setLocation(Point{formulaLoc.x + count, formulaLoc.y});
        }
        
        //adjust references, if any
        if (auto refs = formula->references()) {
            if (auto adjusted = refs->adjustToColumnInsertion(formulaLoc.x, x, count); adjusted != refs) {
                formula->replaceReferences(std::move(adjusted));
            }
        }
    }

    m_columnWidths.insertIndices(x, count);
    
    recalcIfNotSuspended();
}

inline void Sheet::setLength(LengthMap & map, SizeType start, SizeType end, LengthType length) {
    map.modifyValue(start, end, [length](const LengthInfo * ptr) -> std::optional<LengthInfo> {
        return LengthInfo{length, ptr ? ptr->hidden: false};
    });
}

inline void Sheet::clearLength(LengthMap & map, SizeType start, SizeType end) {
    map.modifyValue(start, end, [](const LengthInfo * ptr) -> std::optional<LengthInfo> {
        
        if (ptr && ptr->hidden)
            return LengthInfo{std::nullopt, true};
        
        return std::nullopt;
    });
}

inline void Sheet::hideLength(LengthMap & map, SizeType start, SizeType end) {
    map.modifyValue(start, end, [](const LengthInfo * ptr) -> std::optional<LengthInfo> {
        return LengthInfo{ptr ? ptr->length : std::nullopt, true};
    });
}

inline void Sheet::unhideLength(LengthMap & map, SizeType start, SizeType end) {
    map.modifyValue(start, end, [](const LengthInfo * ptr) -> std::optional<LengthInfo> {
        
        if (ptr && ptr->length) {
            return LengthInfo{ptr->length, false};
        }
        
        return std::nullopt;
    });
}

void Sheet::setRowHeight(SizeType y, SizeType count, LengthType height) {
    SPR_ASSERT_INPUT(y < maxSize().height && count <= maxSize().height && maxSize().height - count >= y);
    m_grid.expandHeightToAtLeast(y + count);
    setLength(m_rowHeights, y, y + count, height);
}

void Sheet::clearRowHeight(SizeType y, SizeType count) {
    SPR_ASSERT_INPUT(y < maxSize().height && count <= maxSize().height && maxSize().height - count >= y);
    m_grid.expandHeightToAtLeast(y + count);
    clearLength(m_rowHeights, y, y + count);
}

void Sheet::hideRow(SizeType y, SizeType count, bool hidden) {
    SPR_ASSERT_INPUT(y < maxSize().height && count <= maxSize().height && maxSize().height - count >= y);
    m_grid.expandHeightToAtLeast(y + count);
    hidden ? hideLength(m_rowHeights, y, y + count) : unhideLength(m_rowHeights, y, y + count);
}

auto Sheet::getRowHeight(SizeType y) const noexcept -> const LengthInfo & {
    SPR_ASSERT_INPUT(y < maxSize().height);
    return m_rowHeights.getValueWithDefault(y, s_defaultLengthInfo);
}

void Sheet::setColumnWidth(SizeType x, SizeType count, LengthType width) {
    SPR_ASSERT_INPUT(x < maxSize().width && count <= maxSize().width && maxSize().width - count >= x);
    m_grid.expandWidthToAtLeast(x + count);
    setLength(m_columnWidths, x, x + count, width);
}

void Sheet::clearColumnWidth(SizeType x, SizeType count) {
    SPR_ASSERT_INPUT(x < maxSize().width && count <= maxSize().width && maxSize().width - count >= x);
    m_grid.expandWidthToAtLeast(x + count);
    clearLength(m_columnWidths, x, x + count);
}

void Sheet::hideColumn(SizeType x, SizeType count, bool hidden) {
    SPR_ASSERT_INPUT(x < maxSize().width && count <= maxSize().width && maxSize().width - count >= x);
    m_grid.expandWidthToAtLeast(x + count);
    hidden ? hideLength(m_columnWidths, x, x + count) : unhideLength(m_columnWidths, x, x + count);
}

auto Sheet::getColumnWidth(SizeType x) const noexcept -> const LengthInfo & {
    SPR_ASSERT_INPUT(x < maxSize().width);
    return m_columnWidths.getValueWithDefault(x, s_defaultLengthInfo);
}


