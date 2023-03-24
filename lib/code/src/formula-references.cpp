// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/formula-references.h>
#include <spreader/cell-grid.h>

using namespace Spreader;

class FormulaReferences::Adjuster {

public:
    Adjuster(const FormulaReferences * owner) noexcept : m_owner(owner)
    {}

    void setIndex(size_t idx) noexcept 
        { m_idx = idx; }

    auto getEntry() -> const Entry & 
        { return m_owner->entries()[m_idx]; }

    void setError() 
        { getAdjustedEntry() = Entry{}; }

    void setValue(const auto & origValue, auto newValue) {
        using RefType = std::remove_cvref_t<decltype(origValue)>;

        auto & adjustedEntry = getAdjustedEntry();
        auto & origEntry = getEntry();
        auto refOffset = reinterpret_cast<const std::byte *>(&origValue) - reinterpret_cast<const std::byte *>(&origEntry);
        RefType & dest = *reinterpret_cast<RefType *>(reinterpret_cast<std::byte *>(&adjustedEntry) + refOffset);
        dest = newValue;
    }

    auto result() const noexcept -> refcnt_ptr<const FormulaReferences> 
        { return m_adjusted ? m_adjusted : refcnt_retain(m_owner); }
private:
    auto getAdjustedEntry() -> Entry & {
        if (!m_adjusted)
            m_adjusted = m_owner->clone();
        return m_adjusted->entries()[m_idx];
    }
private:
    const FormulaReferences * m_owner;
    refcnt_ptr<FormulaReferences> m_adjusted;
    size_t m_idx;
};

template<class Func>
SPR_ALWAYS_INLINE auto FormulaReferences::adjust(Func func) const -> refcnt_ptr<const FormulaReferences> {

    Adjuster adjuster{this};

    for(size_t i = 0; i < m_count; ++i) {

        adjuster.setIndex(i);

        visit([&adjuster, func](const auto & ref) {
            func(adjuster, ref);
        }, adjuster.getEntry());
    }
    return adjuster.result();
}

auto FormulaReferences::adjustToCopy(Point dest) const -> refcnt_ptr<const FormulaReferences> {

    return adjust([dest](Adjuster & adjuster, const auto & ref) {
        using RefType = std::remove_cvref_t<decltype(ref)>;

        if constexpr (!std::is_same_v<RefType, IllegalReference>) {

            if (!ref.template isDereferencable<CellGrid::maxSize()>(dest))
                adjuster.setError();
        }

    });
}

auto FormulaReferences::adjustToMove(Point from, Point to) const -> refcnt_ptr<const FormulaReferences> {

    DistanceType dx = DistanceType(to.x - from.x);
    DistanceType dy = DistanceType(to.y - from.y);

    if (dx == 0 && dy == 0)
        return refcnt_retain(this);

    return adjust([dx, dy](Adjuster & adjuster, const auto & ref) {
        using RefType = std::remove_cvref_t<decltype(ref)>;

        if constexpr (std::is_same_v<RefType, CellReference>) {
            auto refX = ref.x();
            auto adjustedX = refX.adjustedToMove(dx);
            auto refY = ref.y();
            auto adjustedY = refY.adjustedToMove(dy);
            if (refX != adjustedX || refY != adjustedY) {
                adjuster.setValue(ref, CellReference(adjustedX, adjustedY));
            }

        } else if constexpr (std::is_same_v<RefType, AreaReference>) {
            auto startX = ref.xStart();
            auto adjustedStartX = startX.adjustedToMove(dx);
            auto startY = ref.yStart();
            auto adjustedStartY = startY.adjustedToMove(dy);
            auto endX = ref.xEnd();
            auto adjustedEndX = endX.adjustedToMove(dx);
            auto endY = ref.yEnd();
            auto adjustedEndY = endY.adjustedToMove(dy);

            if (startX != adjustedStartX || startY != adjustedStartY || endX != adjustedEndX || endY != adjustedEndY) {
                adjuster.setValue(ref, AreaReference(adjustedStartX, adjustedStartY, adjustedEndX, adjustedEndY));
            }
        } else if constexpr (std::is_same_v<RefType, ColumnReference>) {
            auto start = ref.start();
            auto adjustedStart = start.adjustedToMove(dx);
            auto end = ref.end();
            auto adjustedEnd = end.adjustedToMove(dx);

            if (start != adjustedStart || end != adjustedEnd) {
                adjuster.setValue(ref, ColumnReference(adjustedStart, adjustedEnd));
            }
        } else if constexpr (std::is_same_v<RefType, RowReference>) {
            auto start = ref.start();
            auto adjustedStart = start.adjustedToMove(dy);
            auto end = ref.end();
            auto adjustedEnd = end.adjustedToMove(dy);

            if (start != adjustedStart || end != adjustedEnd) {
                adjuster.setValue(ref, RowReference(adjustedStart, adjustedEnd));
            }
        }

    });
}

auto FormulaReferences::adjustToRowDeletion(SizeType at, SizeType y, SizeType count) const -> refcnt_ptr<const FormulaReferences> {

    return adjust([at, y, count](Adjuster & adjuster, const auto & ref) {

        using RefType = std::remove_cvref_t<decltype(ref)>;

        if constexpr (std::is_same_v<RefType, CellReference>) {
            auto refY = ref.y();
            SizeType dest = refY.dereference(at);
            if (dest >= y && dest < y + count) {
                adjuster.setError();
            } else if (auto adjustedY = refY.adjustedToErasure(at, y, count); adjustedY != refY) {
                adjuster.setValue(ref, CellReference(ref.x(), adjustedY));
            }
        } else if constexpr (std::is_same_v<RefType, AreaReference>) {
            auto startY = ref.yStart();
            auto endY = ref.yEnd();
            SizeType startDest = startY.dereference(at);
            SizeType endDest = endY.dereference(at);
            if (startDest >= y && startDest < y + count && endDest >= y && endDest < y + count) {
                adjuster.setError();
            } else {
                auto adjustedStartY = startY.adjustedToErasure(at, y, count);
                auto adjustedEndY = endY.adjustedToErasure(at, y, count);
                if (adjustedStartY != startY || adjustedEndY != endY) {
                    adjuster.setValue(ref, AreaReference(ref.xStart(), adjustedStartY,
                                                            ref.xEnd(), adjustedEndY));
                }
            }
        } else if constexpr (std::is_same_v<RefType, RowReference>) {
            auto start = ref.start();
            auto end = ref.end();
            SizeType startDest = start.dereference(at);
            SizeType endDest = end.dereference(at);
            if (startDest >= y && startDest < y + count && endDest >= y && endDest < y + count) {
                adjuster.setError();
            } else {
                auto adjustedStart = start.adjustedToErasure(at, y, count);
                auto adjustedEnd = end.adjustedToErasure(at, y, count);
                if (adjustedStart != start || adjustedEnd != end) {
                    adjuster.setValue(ref, RowReference(adjustedStart, adjustedEnd));
                }
            }
        } 
    });
}

auto FormulaReferences::adjustToColumnDeletion(SizeType at, SizeType x, SizeType count) const -> refcnt_ptr<const FormulaReferences> {

    return adjust([at, x, count](Adjuster & adjuster, const auto & ref) {

        using RefType = std::remove_cvref_t<decltype(ref)>;

        if constexpr (std::is_same_v<RefType, CellReference>) {
            auto refX = ref.x();
            SizeType dest = refX.dereference(at);
            if (dest >= x && dest < x + count) {
                adjuster.setError();
            } else if (auto adjustedX = refX.adjustedToErasure(at, x, count); adjustedX != refX) {
                adjuster.setValue(ref, CellReference(adjustedX, ref.y()));
            }
        } else if constexpr (std::is_same_v<RefType, AreaReference>) {
            auto startX = ref.xStart();
            auto endX = ref.xEnd();
            SizeType startDest = startX.dereference(at);
            SizeType endDest = endX.dereference(at);
            if (startDest >= x && startDest < x + count && endDest >= x && endDest < x + count) {
                adjuster.setError();
            } else {
                auto adjustedStartX = startX.adjustedToErasure(at, x, count);
                auto adjustedEndX = endX.adjustedToErasure(at, x, count);
                if (adjustedStartX != startX || adjustedEndX != endX) {
                    adjuster.setValue(ref, AreaReference(adjustedStartX, ref.yStart(),
                                                            adjustedEndX, ref.yEnd()));
                }
            }
        } else if constexpr (std::is_same_v<RefType, ColumnReference>) {
            auto start = ref.start();
            auto end = ref.end();
            SizeType startDest = start.dereference(at);
            SizeType endDest = end.dereference(at);
            if (startDest >= x && startDest < x + count && endDest >= x && endDest < x + count) {
                adjuster.setError();
            } else {
                auto adjustedStart = start.adjustedToErasure(at, x, count);
                auto adjustedEnd = end.adjustedToErasure(at, x, count);
                if (adjustedStart != start || adjustedEnd != end) {
                    adjuster.setValue(ref, ColumnReference(adjustedStart, adjustedEnd));
                }
            }
        }
    });
}

auto FormulaReferences::adjustToRowInsertion(SizeType at, SizeType y, SizeType count) const -> refcnt_ptr<const FormulaReferences> {

    [[maybe_unused]] constexpr auto MaxHeight = CellGrid::maxSize().height;
    
    return adjust([at, y, count](Adjuster & adjuster, const auto & ref) { 

        using RefType = std::remove_cvref_t<decltype(ref)>;

        if constexpr (std::is_same_v<RefType, CellReference>) {
            auto refY = ref.y();
            auto adjustedY = refY.template adjustedToInsertion<MaxHeight>(at, y, count);
            if (!adjustedY) {
                adjuster.setError();
            } else if ( *adjustedY != refY) {
                adjuster.setValue(ref, CellReference(ref.x(), *adjustedY));
            }
        } else if constexpr (std::is_same_v<RefType, AreaReference>) {
            auto startY = ref.yStart();
            auto endY = ref.yEnd();
            auto adjustedStartY = startY.template adjustedToInsertion<MaxHeight>(at, y, count);
            auto adjustedEndY = endY.template adjustedToInsertion<MaxHeight>(at, y, count);
            if (!adjustedStartY || !adjustedEndY) {
                adjuster.setError();
            } else if (*adjustedStartY != startY || *adjustedEndY != endY) {
                adjuster.setValue(ref, AreaReference(ref.xStart(), *adjustedStartY,
                                                     ref.xEnd(), *adjustedEndY));
            }
        } else if constexpr (std::is_same_v<RefType, RowReference>) {
            auto start = ref.start();
            auto end = ref.end();
            auto adjustedStart = start.template adjustedToInsertion<MaxHeight>(at, y, count);
            auto adjustedEnd = end.template adjustedToInsertion<MaxHeight>(at, y, count);
            if (!adjustedStart || !adjustedEnd) {
                adjuster.setError();
            } else if (*adjustedStart != start || *adjustedEnd != end) {
                adjuster.setValue(ref, RowReference(*adjustedStart, *adjustedEnd));
            }
        } 
    });
}

auto FormulaReferences::adjustToColumnInsertion(SizeType at, SizeType x, SizeType count) const -> refcnt_ptr<const FormulaReferences> {

    [[maybe_unused]] constexpr auto MaxWidth = CellGrid::maxSize().width;
    
    return adjust([at, x, count](Adjuster & adjuster, const auto & ref) { 

        using RefType = std::remove_cvref_t<decltype(ref)>;

        if constexpr (std::is_same_v<RefType, CellReference>) {
            auto refX = ref.x();
            auto adjustedX = refX.template adjustedToInsertion<MaxWidth>(at, x, count);
            if (!adjustedX) {
                adjuster.setError();
            } else if (*adjustedX != refX) {
                adjuster.setValue(ref, CellReference(*adjustedX, ref.y()));
            }
        } else if constexpr (std::is_same_v<RefType, AreaReference>) {
            auto startX = ref.xStart();
            auto endX = ref.xEnd();
            auto adjustedStartX = startX.template adjustedToInsertion<MaxWidth>(at, x, count);
            auto adjustedEndX = endX.template adjustedToInsertion<MaxWidth>(at, x, count);
            if (!adjustedStartX || !adjustedEndX) {
                adjuster.setError();
            } else if (*adjustedStartX != startX || *adjustedEndX != endX) {
                adjuster.setValue(ref, AreaReference(*adjustedStartX, ref.yStart(),
                                                     *adjustedEndX, ref.yEnd()));
            }
        } else if constexpr (std::is_same_v<RefType, ColumnReference>) {
            auto start = ref.start();
            auto end = ref.end();
            auto adjustedStart = start.template adjustedToInsertion<MaxWidth>(at, x, count);
            auto adjustedEnd = end.template adjustedToInsertion<MaxWidth>(at, x, count);
            if (!adjustedStart || !adjustedEnd) {
                adjuster.setError();
            } else if (*adjustedStart != start || *adjustedEnd != end) {
                adjuster.setValue(ref, ColumnReference(*adjustedStart, *adjustedEnd));
            }
        } 
    });
}
