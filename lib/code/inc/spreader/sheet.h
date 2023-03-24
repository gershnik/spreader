// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SHEET_H_INCLUDED
#define SPR_HEADER_SHEET_H_INCLUDED

#include <spreader/cell-grid.h>
#include <spreader/linked-list.h>
#include <spreader/interval-map.h>
#include <spreader/name-manager.h>
#include <spreader/coro-generator.h>

namespace Spreader {

    class FormulaEvaluator;

    class Sheet {
        
    public:
        using LengthType = unsigned;

   
        struct LengthInfo {
            std::optional<LengthType> length;
            bool hidden = false;

            friend auto operator==(const LengthInfo & lhs, const LengthInfo & rhs) -> bool
                { return lhs.length == rhs.length && lhs.hidden == rhs.hidden; }
            friend auto operator!=(const LengthInfo & lhs, const LengthInfo & rhs) -> bool
                { return !(lhs == rhs); }
        };

    private:
        using LengthMap = IntervalMap<SizeType, LengthInfo>;

    public:
        auto size() const noexcept -> Size
            { return m_grid.size(); }
        static constexpr auto maxSize() noexcept -> Size
            { return CellGrid::maxSize(); }
        
        void setValueCell(Point coord, const Scalar & value);
        void setFormulaCell(Point coord, const String & formula);
        void clearCell(Point coord);

        void copyCell(Point from, Rect to);
        void copyCells(Rect from, Point to);
        void moveCell(Point from, Point to);
        void moveCells(Rect from, Point to);


        void deleteRows(SizeType y, SizeType count);
        void deleteColumns(SizeType x, SizeType count);
        void insertRows(SizeType y, SizeType count);
        void insertColumns(SizeType x, SizeType count);
        
        void suspendRecalc() noexcept 
            { ++m_recalcSuspendedCount; }
        void resumeRecalc() {
            if (m_recalcSuspendedCount == 0)
                SPR_FATAL_ERROR("mismatched resumeRecalc");
            if (--m_recalcSuspendedCount == 0)
                recalculate();
        }
        void recalculate();


        auto getValue(Point coord) const -> Scalar {
            if (auto * cell = m_grid.getCell(coord))
                return cell->value();
            return Scalar{};
        }

        struct FormulaInfo {
            String text;
            Size extent;
            
            friend auto operator==(const FormulaInfo &, const FormulaInfo &) -> bool = default;
            friend auto operator!=(const FormulaInfo &, const FormulaInfo &) -> bool = default;
            
        };
        auto getFormulaInfo(Point coord) const -> std::optional<FormulaInfo> {
            return applyToCell(m_grid.getCell(coord), [coord](auto ptr) -> std::optional<FormulaInfo> {
                using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

                if constexpr (std::is_same_v<T, FormulaCell>) {
                    auto text = ptr->formula()->reconstructAt(ptr->references(), coord);
                    return FormulaInfo{text, ptr->extent()};
                } else {
                    return std::nullopt;
                }
            });
        }

        auto nonNullCellCount() const noexcept -> uint64_t {
            return m_grid.nonNullCellCount();
        }

        
        auto parseColumn(const String & str) const -> std::optional<SizeType>
            { return m_nameManager.parseColumn(str); }
        auto indexToColumn(SizeType x) const -> String
            { return m_nameManager.indexToColumn(x); }
        auto parseRow(const String & str) const -> std::optional<SizeType>
            { return m_nameManager.parseRow(str); }
        auto indexToRow(SizeType y) const -> String
            { return m_nameManager.indexToRow(y); }
        auto parsePoint(const String & str) const -> std::optional<Point>
            { return m_nameManager.parsePoint(str); }
        auto parseArea(const String & str) const -> std::optional<Rect>
            { return m_nameManager.parseArea(str, size()); }


        void setRowHeight(SizeType y, SizeType count, LengthType height);
        void clearRowHeight(SizeType y, SizeType count);
        void hideRow(SizeType y, SizeType count, bool hidden);
        auto getRowHeight(SizeType y) const noexcept -> const LengthInfo &;
        
        template<class Func>
        requires(std::is_invocable_r_v<void, Func, SizeType, SizeType, const LengthInfo &> ||
                 std::is_invocable_r_v<bool, Func, SizeType, SizeType, const LengthInfo &>)
        auto forEachRowHeight(SizeType y, SizeType count, Func && func) const ->
            std::conditional_t<std::is_invocable_r_v<bool, Func, SizeType, SizeType, const LengthInfo &>, bool, void> { 

            SPR_ASSERT_INPUT(y < maxSize().height && count <= maxSize().height && maxSize().height - count >= y);   
            return m_rowHeights.forEachIntervalWithDefault(y, y + count, s_defaultLengthInfo, std::forward<Func>(func)); 
        }

        auto rowHeights(SizeType y, SizeType count) const -> CoroGenerator<std::tuple<SizeType, SizeType, const LengthInfo &>> {
            SPR_ASSERT_INPUT(y < maxSize().height && count <= maxSize().height && maxSize().height - count >= y);
            return m_rowHeights.intervalsGenerator(y, y + count, s_defaultLengthInfo);
        }

        void setColumnWidth(SizeType x, SizeType count, LengthType width);
        void clearColumnWidth(SizeType x, SizeType count);
        void hideColumn(SizeType x, SizeType count, bool hidden);
        auto getColumnWidth(SizeType x) const noexcept -> const LengthInfo &;

        template<class Func>
        requires(std::is_invocable_r_v<void, Func, SizeType, SizeType, const LengthInfo &> ||
                 std::is_invocable_r_v<bool, Func, SizeType, SizeType, const LengthInfo &>)
        auto forEachColumnWidth(SizeType x, SizeType count, Func && func) const ->
            std::conditional_t<std::is_invocable_r_v<bool, Func, SizeType, SizeType, const LengthInfo &>, bool, void> { 
                
            SPR_ASSERT_INPUT(x < maxSize().width && count <= maxSize().width && maxSize().width - count >= x);
            return m_columnWidths.forEachIntervalWithDefault(x, x + count, s_defaultLengthInfo, std::forward<Func>(func)); 
        }

        auto columnWidths(SizeType x, SizeType count) const -> CoroGenerator<std::tuple<SizeType, SizeType, const LengthInfo &>> {
            SPR_ASSERT_INPUT(x < maxSize().width && count <= maxSize().width && maxSize().width - count >= x);
            return m_columnWidths.intervalsGenerator(x, x + count, s_defaultLengthInfo);
        }
    private:
        struct SetBlankCell;
        struct SetValueCell;
        struct SetFormulaCell;
        struct CopyCell;
        struct MoveCell;
        struct SimpleMoveCell;
        struct InsertionMoveCell;
        struct ReplaceOldExtensionCell;
        struct ReserveNewExtensionCell;
        struct ClearExtensionCell;

        auto evaluate(FormulaCell * formulaCell, FormulaEvaluator & evaluator) -> bool;
        void reserveExtent(FormulaCell * formulaCell, Size extent);
        void applyEvaluationResult(FormulaCell * formulaCell, FormulaEvaluator & evaluator);
        
        void removeFormulaDependents(FormulaCell * formulaCell);

        void recalcIfNotSuspended() {
            if (m_recalcSuspendedCount == 0)
                recalculate();
        }
        
        void setLength(LengthMap & map, SizeType start, SizeType end, LengthType length);
        void clearLength(LengthMap & map, SizeType start, SizeType end);
        void hideLength(LengthMap & map, SizeType start, SizeType end);
        void unhideLength(LengthMap & map, SizeType start, SizeType end);

    private:
        CellGrid m_grid;
        LinkedList<FormulaCell, FormulaCell::LinkedListTraits> m_formulaCells;
        bool m_evalGeneration = false;
        unsigned m_recalcSuspendedCount = 0;
        LengthMap m_rowHeights;
        LengthMap m_columnWidths;
        NameManager m_nameManager;
        
        static inline constexpr LengthInfo s_defaultLengthInfo{std::nullopt, false};
    };
    
#if SPR_TESTING

    inline auto operator<<(std::ostream & str, const Sheet::FormulaInfo & info) -> std::ostream & {
        return str << '{' << info.text << ',' << info.extent << '}';
    }
    
#endif
}

#endif
