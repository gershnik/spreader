// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_CELL_H_INCLUDED
#define SPR_HEADER_CELL_H_INCLUDED

#include <spreader/types.h>
#include <spreader/scalar.h>
#include <spreader/formula.h>

#include <memory>

namespace Spreader {

    class Cell;
    class FormulaCell;
    class ValueCell;
    class FormulaCellExtension;
    
    enum class CellType {
        Value,
        Formula,
        FormulaExtension
    };

    class Cell  {
    public:
        struct Deleter {
            void operator()(Cell * cell) const noexcept;
        };
    public:
        auto value() const noexcept -> const Scalar &
            { return m_value; }

        void setValue(Scalar && val) noexcept
            { m_value = std::move(val); }
        void setValue(const Scalar & val) noexcept
            { m_value = val; }

        auto getType() const noexcept -> CellType 
            { return m_type; }

    protected:
        Cell(CellType type) noexcept : m_type(type)
        {}

        template<class T>
        requires(std::is_constructible_v<Scalar, T &&>)
        Cell(CellType type, T && val) noexcept(std::is_nothrow_constructible_v<Scalar, T &&>) : 
            m_type(type),
            m_value(std::forward<T>(val)) {
        }

        ~Cell() noexcept = default;
    
    private:
        Cell(const Cell &) = delete;
        Cell(Cell &&) = delete;
        Cell & operator=(const Cell &) = delete;
        Cell & operator=(Cell &&) = delete;

        
    private:
        const CellType m_type;
        Scalar m_value;
    };

    using CellPtr = std::unique_ptr<Cell, Cell::Deleter>;
    static_assert(sizeof(CellPtr) == sizeof(Cell *), "your standard library has defective unique_ptr that is not pointer sized with 0-size deleter");

    using ValueCellPtr = std::unique_ptr<ValueCell, Cell::Deleter>;
    using FormulaCellPtr = std::unique_ptr<FormulaCell, Cell::Deleter>;
    using FormulaCellExtensionPtr = std::unique_ptr<FormulaCellExtension, Cell::Deleter>;
    
    class ValueCell final : public Cell {
        friend Cell::Deleter;
    public:
        template<class T>
        requires(std::is_constructible_v<Scalar, T &&>)
        static auto create(T && val) noexcept(std::is_nothrow_constructible_v<Scalar, T &&>) -> ValueCellPtr {
            return ValueCellPtr(new ValueCell(std::forward<T>(val)));
        }

        void * operator new(size_t size);
        void operator delete(void * p, size_t size) noexcept;

        auto isBlank() const noexcept {
            return value().isBlank();
        }

        auto copy() const -> CellPtr {
            return CellPtr(new ValueCell(value()));
        }
    private:
        template<class T>
        requires(std::is_constructible_v<Scalar, T &&>)
        ValueCell(T && val) noexcept(std::is_nothrow_constructible_v<Scalar, T &&>) : 
            Cell(CellType::Value, std::forward<T>(val)) {
        }
        ~ValueCell() noexcept = default;
    };

    class FormulaCell final : public Cell {
        friend Cell::Deleter;
    public:
        static auto create(ConstFormulaPtr && formula, ConstFormulaReferencesPtr && refs, Point location, bool generation) noexcept -> FormulaCellPtr
            { return FormulaCellPtr(new FormulaCell(std::move(formula), std::move(refs), location, generation)); }

        void * operator new(size_t size);
        void operator delete(void * p, size_t size) noexcept;

        auto getRecalcDependency(bool generation) const noexcept -> FormulaCell * {
            return (m_generation != generation || m_blocked) ? const_cast<FormulaCell *>(this) : nullptr;
        }
        
        auto copy(Point to) const -> CellPtr {
            auto refs = m_references->adjustToCopy(to);
            return CellPtr(new FormulaCell(m_formula, refs, to, m_generation));
        }

        void move(Point to)  {
            m_references = m_references->adjustToMove(m_location, to);
            m_location = to;
        }

        auto formula() const noexcept -> const ConstFormulaPtr &
            { return m_formula; }
        auto references() const noexcept -> const ConstFormulaReferencesPtr &
            { return m_references; }
        auto location() const noexcept -> Point
            { return m_location; }
        void setLocation(Point value) noexcept
            { m_location = value; }
        auto extent() const noexcept -> Size
            { return m_extent; }
        void setExtent(Size val) noexcept 
            { m_extent = val; }
        
        void replaceFormula(ConstFormulaPtr && formula, ConstFormulaReferencesPtr && refs) {
            m_formula = std::move(formula);
            m_references = std::move(refs);
            //m_blocked = 0;
        }
        void replaceReferences(ConstFormulaReferencesPtr && refs) {
            m_references = std::move(refs);
            //m_blocked = 0;
        }
        

        auto needsRecalc(bool generation) const noexcept -> bool 
            { return m_generation != generation; }
        auto setBeingCalculated(bool val) noexcept
            { m_inProgress = val; }
        auto isCircularDependency() const noexcept -> bool
            { return m_inProgress || m_blocked; }
        auto finishCalculation(bool generation, bool blocked) noexcept {
            m_generation = generation;
            m_inProgress = false;
            m_blocked = blocked;
        }

        struct LinkedListTraits {
            static auto next(FormulaCell * cell) noexcept -> FormulaCell *
                { return cell->m_nextFormulaCell; }
            static void setNext(FormulaCell * cell, FormulaCell * next) noexcept 
                { cell->m_nextFormulaCell = next; }
            static auto prev(FormulaCell * cell) noexcept -> FormulaCell *
                { return cell->m_prevFormulaCell; } 
            static void setPrev(FormulaCell * cell, FormulaCell * prev) noexcept 
                { cell->m_prevFormulaCell = prev; }
        };

    private:
        template<class FPtr, class RPtr>
        FormulaCell(FPtr && formula, RPtr && refs, Point location, bool generation) noexcept :
            Cell(CellType::Formula),
            m_formula(std::forward<FPtr>(formula)),
            m_references(std::forward<RPtr>(refs)),
            m_location(location),
            m_generation(generation)
        {}
        ~FormulaCell() noexcept = default;

    private:
        ConstFormulaPtr m_formula;
        ConstFormulaReferencesPtr m_references;
        Point m_location;
        Size m_extent = {1, 1};
        FormulaCell * m_prevFormulaCell = nullptr;
        FormulaCell * m_nextFormulaCell = nullptr;
        ///Boolean. If unequal to sheet recalc generation means that cell is stale
        uint8_t m_generation:1 = 0;  
        ///Boolean. If true signifies that the cell is being calculated. When observed during calc of another 
        ///cell means circular dependency.
        uint8_t m_inProgress:1 = 0;  
        ///Boolean. Signifies that last evaluation of the cell resulted in circular dependency. 
        uint8_t m_blocked:1 = 0;
    };

    class FormulaCellExtension final : public Cell {
        friend Cell::Deleter;
    public:
        template<class T>
        requires(std::is_constructible_v<Scalar, T &&>)
        static auto create(FormulaCell * parent, T && val) noexcept(std::is_nothrow_constructible_v<Scalar, T &&>) 
            -> FormulaCellExtensionPtr { 
            
            return FormulaCellExtensionPtr(new FormulaCellExtension(parent, std::forward<T>(val))); 
        }

        void * operator new(size_t size);
        void operator delete(void * p, size_t size) noexcept;

        auto getRecalcDependency(bool generation) const noexcept -> FormulaCell * {
            return m_parentFormulaCell->getRecalcDependency(generation);
        }
        
        FormulaCell * parent() noexcept {
            return m_parentFormulaCell;
        }
    private:
        template<class T>
        requires(std::is_constructible_v<Scalar, T &&>)
        FormulaCellExtension(FormulaCell * parent, T && val) noexcept(std::is_nothrow_constructible_v<Scalar, T &&>) :
            Cell(CellType::FormulaExtension, std::forward<T>(val)),
            m_parentFormulaCell(parent)
        {}
        ~FormulaCellExtension() noexcept = default;

    private:
        FormulaCell * m_parentFormulaCell;
    };

    template<class T, class CellPtrLike>
    SPR_ALWAYS_INLINE auto downcastCell(CellPtrLike && cell) {

        if constexpr (std::is_const_v<std::remove_pointer_t<decltype(&*cell)>>)
            return static_cast<const T *>(&*cell);
        else
            return static_cast<T *>(&*cell);
    }

    template<class CellPtrLike, class Func>
    SPR_ALWAYS_INLINE auto applyToCell(CellPtrLike && cell, Func func) {
        if (!cell)
            return func(nullptr);

        
        switch(cell->getType()) {
            case CellType::Value: return func(downcastCell<ValueCell>(std::forward<CellPtrLike>(cell)));
            case CellType::Formula: return func(downcastCell<FormulaCell>(std::forward<CellPtrLike>(cell)));
            case CellType::FormulaExtension: return func(downcastCell<FormulaCellExtension>(std::forward<CellPtrLike>(cell)));
        }
        
        SPR_ASSERT_LOGIC(false);
        SPR_FATAL_ERROR("unexpected cell type");
    }

    inline void Cell::Deleter::operator()(Cell * cell) const noexcept {
        applyToCell(cell, [](auto ptr) {

            if constexpr (!std::is_same_v<decltype(ptr), std::nullptr_t>)
                delete ptr;
        });
    }

    inline auto getRecalcDependency(const Cell * cell, bool generation) noexcept -> FormulaCell * {
        return applyToCell(cell, [generation](auto ptr) -> FormulaCell * {
            using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

            if constexpr (std::is_same_v<T, FormulaCell> || std::is_same_v<T, FormulaCellExtension>) {
                return ptr->getRecalcDependency(generation);
            }
            else {
                return nullptr;
            }
        });
    }

}


#endif
