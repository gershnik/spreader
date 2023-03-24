// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_CELL_GRID_H_INCLUDED
#define SPR_HEADER_CELL_GRID_H_INCLUDED

#include <spreader/error-handling.h>
#include <spreader/geometry.h>
#include <spreader/cell.h>

#include <numeric>
#include <limits>
#include <cstring>
#include <array>

namespace Spreader {

    class CellGrid {

    private:
        inline static constexpr Size s_sizePowers[] = {
            Size{4, 10}, // 2^14 =>  16kB *  sizeof(void *) = 128kb(64bit)
            Size{4, 10},
            Size{8, 11}  // 2^19 => 0.5MB * sizeof(void *) = 4MB(64bit)
        };
        inline static constexpr unsigned s_maxLevel = static_cast<unsigned>(std::size(s_sizePowers)) - 1;
        

        template<int Level> 
        struct LayerTraits {
            inline static constexpr Size sizePowers = s_sizePowers[Level];
            
            inline static constexpr Size tileCoveragePowers = []() constexpr {
                Size ret = Size{0, 0};
                for (int l = s_maxLevel; l != Level; --l)
                    ret += s_sizePowers[l];
                return ret;
            }();
            inline static constexpr Size remainderMasks = ~(Size{SizeType(-1), SizeType(-1)} << tileCoveragePowers);
        };

        inline static constexpr Size s_maxSize = []() consteval {
            constexpr Size maxSizePower = LayerTraits<0>::tileCoveragePowers + LayerTraits<0>::sizePowers;
            return Size {
                (maxSizePower.width  < sizeof(SizeType) * CHAR_BIT) ? ((1u << maxSizePower.width)  - 1u) : std::numeric_limits<SizeType>::max(),
                (maxSizePower.height < sizeof(SizeType) * CHAR_BIT) ? ((1u << maxSizePower.height) - 1u) : std::numeric_limits<SizeType>::max()
            };
        }();
        static_assert(s_maxSize.width <= MaxSizeType);
        static_assert(s_maxSize.height <= MaxSizeType);


        inline static constexpr uint64_t s_maxCellCount = uint64_t(s_maxSize.width) * uint64_t(s_maxSize.height);


    private:
        template<class Mover>
        struct Eraser {
            constexpr auto modifiesMissing() noexcept -> bool { return false; }
            auto operator()(CellPtr & cell) -> int {
                int ret = this->mover.get(cell);
                SPR_ASSERT_LOGIC(!cell);
                return ret;
            }

            Mover mover;
        };
        template<class Mover>
        static auto moverAsEraser(Mover && tr) noexcept -> Eraser<Mover> {
            return Eraser<Mover>{std::forward<Mover>(tr)};
        }

        template<class Transform>
        struct TransformAsGetter {

            constexpr auto modifiesMissing() -> bool { return std::forward<Transform>(transform).modifiesMissingSource(); }
            auto operator()(CellPtr & cell) -> int { return std::forward<Transform>(transform).get(cell); }

            Transform && transform;
        };
        template<class Transform>
        static auto transformAsGetter(Transform && tr) noexcept -> TransformAsGetter<Transform> {
            return TransformAsGetter<Transform>{std::forward<Transform>(tr)};
        }

        template<class Transform>
        struct TransformAsSetter {

            constexpr auto modifiesMissing() -> bool { return std::forward<Transform>(transform).modifiesMissingDestination(); }
            auto operator()(CellPtr & cell) -> int { return std::forward<Transform>(transform).set(cell); }

            Transform && transform;
        };
        template<class Transform>
        static auto transformAsSetter(Transform && tr) noexcept -> TransformAsSetter<Transform> {
            return TransformAsSetter<Transform>{std::forward<Transform>(tr)};
        }

        template<int Level> 
        class Tile {
        private:
            using Traits = LayerTraits<Level>;
            using ValuePtr = std::conditional_t<Level == s_maxLevel, Cell *, Tile<Level + 1> *>;

            inline static constexpr size_t s_cellCount = (size_t(1) << (Traits::sizePowers.width + Traits::sizePowers.height));
        public:
            Tile() noexcept = default;
            Tile(const Tile & ) = delete;
            Tile(Tile && ) = delete;
            ~Tile() noexcept {
                if (m_nonNullCount) {
                    for(auto p: m_data) {
                        if constexpr (Level != s_maxLevel)
                            delete p;
                        else
                            reinterpret_cast<CellPtr *>(&p)->reset();
                    }
                }
            }

            SPR_ALWAYS_INLINE auto get(Point coord) const noexcept -> Cell * {

                if constexpr (Level != s_maxLevel) {
                    Point tileCoord = coord >> Traits::tileCoveragePowers; //e.g. remaining / tileCoverage
                    auto & next = m_data[(tileCoord.y << Traits::sizePowers.width) + tileCoord.x];
                    if (!next) [[unlikely]]
                        return nullptr;
                    coord &= Traits::remainderMasks; //e.g. coord %= tileCoverage
                    return next->get(coord);
                } else {
                    auto & cell = m_data[(coord.y << Traits::sizePowers.width) + coord.x];
                    return cell;
                }
            }

            template<class Op>
            SPR_ALWAYS_INLINE auto actOn(Point coord, Op && op) -> int {
                if constexpr (Level != s_maxLevel) {
                    Point tileCoord = coord >> Traits::tileCoveragePowers; //e.g. coord / tileCoverage
                    auto & next = m_data[(tileCoord.y << Traits::sizePowers.width) + tileCoord.x];
                    if (!next) [[unlikely]] {
                        if (!std::forward<Op>(op).modifiesMissing())
                            return 0;
                        next = new Tile<Level + 1>;
                    }
                    coord &= Traits::remainderMasks; //e.g. coord %= tileCoverage
                    startModifications(next);
                    auto nonNullDelta = next->actOn(coord, std::forward<Op>(op));
                    endModifications(next);
                    addNonEmptyCount(nonNullDelta);
                    return nonNullDelta;
                } else {
                    auto & cell = m_data[(coord.y << Traits::sizePowers.width) + coord.x];
                    int nonNullDelta = std::forward<Op>(op)(reinterpret_cast<CellPtr &>(cell));
                    addNonEmptyCount(nonNullDelta);
                    return nonNullDelta;
                }
            }
            
            template<class Op>
            SPR_ALWAYS_INLINE auto actOnChunk(Point coord, Size & size, Op && op) -> int64_t {
                if constexpr (Level != s_maxLevel) {
                    Point tileCoord = coord >> Traits::tileCoveragePowers; //e.g. coord / tileCoverage
                    coord &= Traits::remainderMasks; //e.g. coord %= tileCoverage
                    auto & next = m_data[(tileCoord.y << Traits::sizePowers.width) + tileCoord.x];
                    if (!next) [[unlikely]] {
                        if (!std::forward<Op>(op).modifiesMissing()) {
                            Size remainingTileSize = (Size{1, 1} << Traits::tileCoveragePowers) - asSize(coord);
                            if (size.width > remainingTileSize.width)
                                size.width = remainingTileSize.width;
                            if (size.height > remainingTileSize.height)
                                size.height = remainingTileSize.height;
                            return 0;
                        }
                        next = new Tile<Level + 1>;
                    }
                    startModifications(next);
                    auto nonNullDelta = next->actOnChunk(coord, size, std::forward<Op>(op));
                    endModifications(next);
                    addNonEmptyCount(nonNullDelta);
                    return nonNullDelta;
                } else {
                    constexpr Size tileSize = Size{1, 1} << Traits::sizePowers;
                    const Size consumed = {
                        std::min(size.width, tileSize.width - coord.x),
                        std::min(size.height, tileSize.height - coord.y)
                    };
                    Point pt, ptEnd = coord + consumed;
                    int64_t nonNullDelta = 0;
                    for (pt.y = coord.y; pt.y != ptEnd.y; ++pt.y) {
                        for (pt.x = coord.x; pt.x != ptEnd.x; ++pt.x) {
                            auto & cell = m_data[(pt.y << Traits::sizePowers.width) + pt.x];
                            nonNullDelta += std::forward<Op>(op)(reinterpret_cast<CellPtr &>(cell));
                        }
                    }
                    size = consumed;
                    addNonEmptyCount(nonNullDelta);
                    return nonNullDelta;
                }
            }
            
            template<class Transformer>
            SPR_ALWAYS_INLINE auto transform(Point from, Point to, Transformer && transformer) -> int {
                
                int nonNullDelta = 0;

                if constexpr (Level != s_maxLevel) {
                    Point fromTileCoord = from >> Traits::tileCoveragePowers; //e.g. from / tileCoverage
                    auto & nextFrom = m_data[(fromTileCoord.y << Traits::sizePowers.width) + fromTileCoord.x];
                    Point toTileCoord = to >> Traits::tileCoveragePowers; //e.g. to / tileCoverage
                    auto & nextTo = m_data[(toTileCoord.y << Traits::sizePowers.width) + toTileCoord.x];

                    if (nextFrom == nextTo) {
                        if (!nextTo) {
                            if (!transformer.modifiesMissingSource() && !transformer.modifiesMissingDestination())
                                return 0;
                            nextTo = new Tile<Level + 1>;
                        }

                        from &= Traits::remainderMasks; //e.g. from %= tileCoverage
                        to &= Traits::remainderMasks; //e.g. to %= tileCoverage
                        startModifications(nextTo);
                        nonNullDelta = nextTo->transform(from, to, std::forward<Transformer>(transformer));

                    } else if (!nextFrom) {

                        to &= Traits::remainderMasks; //e.g. to %= tileCoverage;
                        startModifications(nextTo);
                        nonNullDelta = nextTo->actOn(to, transformAsSetter(std::forward<Transformer>(transformer)));

                    } else {

                        from &= Traits::remainderMasks; //e.g. from %= tileCoverage
                        startModifications(nextFrom);
                        nonNullDelta = nextFrom->actOn(from, transformAsGetter(std::forward<Transformer>(transformer)));
                        endModifications(nextFrom);

                        if (!nextTo) {
                            if (!std::forward<Transformer>(transformer).modifiesMissingDestination()) {
                                return nonNullDelta;
                            }
                            nextTo = new Tile<Level + 1>;
                        }
                        to &= Traits::remainderMasks; //e.g. to %= tileCoverage;
                        startModifications(nextTo);
                        nonNullDelta += nextTo->actOn(to, transformAsSetter(std::forward<Transformer>(transformer)));
                    }
                    
                    endModifications(nextTo);
                    
                } else {
                    auto & cellFrom = m_data[(from.y << Traits::sizePowers.width) + from.x];
                    auto & cellTo = m_data[(to.y << Traits::sizePowers.width) + to.x];
                    nonNullDelta = std::forward<Transformer>(transformer).get(reinterpret_cast<CellPtr &>(cellFrom));
                    nonNullDelta += std::forward<Transformer>(transformer).set(reinterpret_cast<CellPtr &>(cellTo));
                }
                addNonEmptyCount(nonNullDelta);
                return nonNullDelta;
            }
            
            SPR_ALWAYS_INLINE friend void startModifications(Tile *& tile) {
                //artificially bump non-null count to temporarily prevent tile deletion if the real count goes to 0
                //this avoids having a separate modifications count
                ++tile->m_nonNullCount;
            }
            
            SPR_ALWAYS_INLINE friend void endModifications(Tile *& tile) {
                //remove artificial bump and see if the tile is empty now
                if (--tile->m_nonNullCount == 0) [[unlikely]] {
                    delete tile;
                    tile = nullptr;
                }
            }

            SPR_ALWAYS_INLINE auto isEmpty() const noexcept -> bool {
                return m_nonNullCount == 0;
            }

            SPR_ALWAYS_INLINE auto nonNullCellCount() const noexcept -> uint64_t {
                return m_nonNullCount;
            }

        private:
            SPR_ALWAYS_INLINE void addNonEmptyCount(int64_t val) noexcept {
                SPR_ASSERT_LOGIC((val >= 0 && uint64_t(val) <= s_maxCellCount - m_nonNullCount) || (val < 0 && uint64_t(-val) <= m_nonNullCount));
                m_nonNullCount += val;
            } 
        private:
            uint64_t m_nonNullCount = 0;
            std::array<ValuePtr, s_cellCount> m_data{};
        };
        
    public:
        auto size() const noexcept -> Size {
            return m_size;
        }

        static constexpr auto maxSize() -> Size {
            return s_maxSize;
        }

        /** 
         * Get cell at a given coordinate.
         * Preconditions:
         * - coord must be within size()
         */
        auto getCell(Point coord) const noexcept -> Cell *;

        /**
         * Modifies cell at a given coordinate.
         * If coord is outside size() grid size is extended to include it.
         * Preconditions:
         * - coord must be within maxSize()
         * Throws: std::bad_alloc on internal allocation failures.
         */
        template<class Op>
        void modifyCell(Point coord, Op && op);

        template<class Op>
        void modifyCells(Rect rect, Op && op);

        template<class Transformer>
        void transformCells(Rect src, Point dest, Transformer && transformer);

        template<class Transformer>
        void transformCell(Point src, Rect dest, Transformer && transformer);
        
        //Mover must erase on get without set!
        template<class Mover>
        void deleteRows(SizeType y, SizeType count, Mover && cleaner);

        //Mover must erase on get without set!
        template<class Mover>
        void deleteColumns(SizeType x, SizeType count, Mover && mover);

        template<class Mover>
        void insertRows(SizeType y, SizeType count, Mover && mover);

        template<class Mover>
        void insertColumns(SizeType x, SizeType count, Mover && mover);


        auto nonNullCellCount() const noexcept -> uint64_t {
            return m_topTile ? m_topTile->nonNullCellCount() : 0;
        }
        
        void expandWidthToAtLeast(SizeType width) {
            if (width > m_size.width)
                m_size.width = width;
        }
        
        void expandHeightToAtLeast(SizeType height) {
            if (height > m_size.height)
                m_size.height = height;
        }
        
        void expandToAtLeast(Size size) {
            expandWidthToAtLeast(size.width);
            expandHeightToAtLeast(size.height);
        }

    private:
        std::unique_ptr<Tile<0>> m_topTile = nullptr;
        Size m_size = {0, 0};

    };

    template<class Op>
    void CellGrid::modifyCell(Point coord, Op && op) {
        SPR_ASSERT_INPUT(coord.x < s_maxSize.width && coord.y < s_maxSize.height);
    
        expandToAtLeast(asSize(coord) + Size{1, 1});

        if (!m_topTile) [[unlikely]] {
            if (!std::forward<Op>(op).modifiesMissing())
                return;
            m_topTile.reset(new Tile<0>);
        }
        
        m_topTile->actOn(coord, std::forward<Op>(op));
        if (m_topTile->isEmpty()) [[unlikely]] {
            m_topTile.reset();
        }
    }

    template<class Op>
    void CellGrid::modifyCells(Rect rect, Op && op) {
    
        SPR_ASSERT_INPUT(rect.origin.x < s_maxSize.width && rect.origin.y < s_maxSize.height);
        SPR_ASSERT_INPUT(rect.size.width <= s_maxSize.width && rect.size.height <= s_maxSize.height);
        SPR_ASSERT_INPUT(rect.origin.x <= s_maxSize.width - rect.size.width && rect.origin.y <= s_maxSize.height - rect.size.height);

        if (rect.size == Size{0, 0}) [[unlikely]]
            return;
        
        if (!m_topTile) [[unlikely]] {
            if (!std::forward<Op>(op).modifiesMissing())
                return;
            m_topTile.reset(new Tile<0>);
        }
        
        Point pt, ptEnd = rect.end();
        Size consumed;
        for(pt.y = rect.origin.y; pt.y < ptEnd.y; pt.y += consumed.height) {
            consumed.height = ptEnd.y - pt.y;
            for(pt.x = rect.origin.x; pt.x < ptEnd.x; pt.x += consumed.width) {
                consumed.width = ptEnd.x - pt.x;
                m_topTile->actOnChunk(pt, consumed, std::forward<Op>(op));
            }
        }
        
        
        if (m_topTile->isEmpty()) [[unlikely]] {
            m_topTile.reset();
        }
    }


    template<class Transformer>
    void CellGrid::transformCells(Rect src, Point dest, Transformer && transformer) {
        SPR_ASSERT_INPUT(src.origin.x < s_maxSize.width && src.origin.y < s_maxSize.height);
        SPR_ASSERT_INPUT(src.size.width <= s_maxSize.width && src.size.height <= s_maxSize.height);
        SPR_ASSERT_INPUT(src.origin.x <= s_maxSize.width - src.size.width && src.origin.y <= s_maxSize.height - src.size.height);
        SPR_ASSERT_INPUT(dest.x <= s_maxSize.width - src.size.width && dest.y <= s_maxSize.height - src.size.height);

        if (src.size.width == 0 || src.size.height == 0) [[unlikely]]
            return;
        
        expandToAtLeast(asSize(dest) + src.size);

        if (!m_topTile) [[unlikely]]
            return;
        
        SizeType srcX = src.origin.x, destX = dest.x;
        DistanceType incrX;
        if (dest.x < src.origin.x) {
            incrX = 1;
        } else {
            srcX += src.size.width - 1;
            destX += src.size.width - 1;
            incrX = -1;
        }

        SizeType srcY = src.origin.y, destY = dest.y;
        DistanceType incrY;
        if (dest.y < src.origin.y) {
            incrY = 1;
        } else {
            srcY += src.size.height - 1;
            destY += src.size.height - 1;
            incrY = -1;
        }

        SizeType currentSrcY = srcY, currentDestY = destY;
        for(SizeType countY = 0; countY < src.size.height; ++countY, currentSrcY += incrY, currentDestY += incrY) {
            SizeType currentSrcX = srcX, currentDestX = destX;
            for(SizeType countX = 0; countX < src.size.width; ++countX, currentSrcX += incrX, currentDestX += incrX) {
                
                Point srcPt{currentSrcX, currentSrcY};
                Point destPt{currentDestX, currentDestY};
                
                std::forward<Transformer>(transformer).setPoints(srcPt, destPt);
                m_topTile->transform(srcPt, destPt, std::forward<Transformer>(transformer));
            }
        }

        if (m_topTile->isEmpty()) [[unlikely]] {
            m_topTile.reset();
        }
    }

    template<class Transformer>
    void CellGrid::transformCell(Point src, Rect dest, Transformer && transformer) {
        SPR_ASSERT_INPUT(src.x < s_maxSize.width && src.y < s_maxSize.height);
        SPR_ASSERT_INPUT(dest.origin.x < s_maxSize.width && dest.origin.y < s_maxSize.height);
        SPR_ASSERT_INPUT(dest.size.width <= s_maxSize.width && dest.size.height <= s_maxSize.height);
        SPR_ASSERT_INPUT(dest.origin.x <= s_maxSize.width - dest.size.width && dest.origin.y <= s_maxSize.height - dest.size.height);

        expandToAtLeast(asSize(dest.end()));

        if (!m_topTile) [[unlikely]] {
            if (!transformer.modifiesMissingSource() && !transformer.modifiesMissingDestination())
                return;
            m_topTile.reset(new Tile<0>);
        }

        Point pt;
        for(pt.y = dest.origin.y; pt.y < dest.origin.y + dest.size.height; ++pt.y) {
            for(pt.x = dest.origin.x; pt.x < dest.origin.x + dest.size.width; ++pt.x) {
                std::forward<Transformer>(transformer).setPoints(src, pt);
                m_topTile->actOn(src, transformAsGetter(std::forward<Transformer>(transformer)));
                m_topTile->actOn(pt, transformAsSetter(std::forward<Transformer>(transformer)));
            }
        }

        if (m_topTile->isEmpty()) [[unlikely]] {
            m_topTile.reset();
        }
    }

    template<class Mover>
    void CellGrid::deleteRows(SizeType y, SizeType count, Mover && mover) {
    
        SPR_ASSERT_INPUT(y < s_maxSize.height && count <= s_maxSize.height && s_maxSize.height - count >= y);

        if (y >= m_size.height || count == 0)
            return;
        
        auto existingOverY = m_size.height - y;
        SizeType shiftedBottomSize;
        if (existingOverY > count) {
            shiftedBottomSize = existingOverY - count;
            Rect moveRect{.origin = {0, y + count}, .size = {m_size.width, shiftedBottomSize}};
            this->transformCells(moveRect, {0, y}, std::forward<Mover>(mover));
        } else {
            count = existingOverY;
            shiftedBottomSize = 0;
        }
        if (shiftedBottomSize < count) {
            this->modifyCells({.origin = {0, y + shiftedBottomSize}, .size = {m_size.width, count - shiftedBottomSize}},
                              moverAsEraser(std::forward<Mover>(mover)));
        }
        m_size.height -= count;
    }

    template<class Mover>
    void CellGrid::deleteColumns(SizeType x, SizeType count, Mover && mover) {

        SPR_ASSERT_INPUT(x < s_maxSize.width && count <= s_maxSize.width && s_maxSize.width - count >= x);

        if (x >= m_size.width || count == 0)
            return;
        
        auto existingOverX = m_size.width - x;
        SizeType shiftedRightSize;
        if (existingOverX > count) {
            shiftedRightSize = existingOverX - count;
            Rect moveRect{.origin = {x + count, 0}, .size = {shiftedRightSize, m_size.height}};
            this->transformCells(moveRect, {x, 0}, std::forward<Mover>(mover));
        } else {
            count = existingOverX;
            shiftedRightSize = 0;
        }
        if (shiftedRightSize < count) {
            this->modifyCells({.origin = {x + shiftedRightSize, 0}, .size = {count - shiftedRightSize, m_size.height}},
                              moverAsEraser(std::forward<Mover>(mover)));
        }
        m_size.width -= count;
    }

    template<class Mover>
    void CellGrid::insertRows(SizeType y, SizeType count, Mover && mover) {
        SPR_ASSERT_INPUT(y < s_maxSize.height && count <= s_maxSize.height);
        SPR_ASSERT_INPUT(s_maxSize.height - count >= std::max(y, m_size.height));

        if (y < m_size.height) {
            auto newHeight = m_size.height + count;
            
            this->transformCells({.origin = {0, y}, .size = {m_size.width, m_size.height - y}}, 
                                 {0, y + count}, 
                                 std::forward<Mover>(mover));
            m_size.height = newHeight;
        } else {
            m_size.height = y + count;
        }
    }

    template<class Mover>
    void CellGrid::insertColumns(SizeType x, SizeType count, Mover && mover) {
        SPR_ASSERT_INPUT(x < s_maxSize.width && count <= s_maxSize.width);
        SPR_ASSERT_INPUT(s_maxSize.width - count >= std::max(x, m_size.width));

        if (x < m_size.width) {
            auto newWidth = m_size.width + count;
            this->transformCells({.origin = {x, 0}, .size = {m_size.width - x, m_size.height}}, 
                                 {x + count, 0}, 
                                 std::forward<Mover>(mover));
            m_size.width = newWidth;
        } else {
            m_size.width = x + count;
        }
    }
}

#endif
