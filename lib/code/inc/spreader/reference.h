// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_REFERENCE_H_INCLUDED
#define SPR_HEADER_REFERENCE_H_INCLUDED

#include <spreader/geometry.h>

#include <variant>

namespace Spreader {

    enum class ReferenceType : SizeType{
        Relative = 0,
        Absolute = 1
    };

    enum class ReferenceDimension {
        Column,
        Row
    };

    class ReferenceValue {
    public:
        constexpr ReferenceValue() noexcept = default;
        constexpr ReferenceValue(SizeType size) noexcept:
            m_value(size){
        }
        
        constexpr auto dereference(ReferenceType type, SizeType at) const noexcept -> SizeType {
            return type == ReferenceType::Absolute ? asSize() : at + asDistance();
        }

        constexpr auto asSize() const noexcept -> SizeType {
            return m_value;
        }
        constexpr auto asDistance() const noexcept -> DistanceType {
            return static_cast<DistanceType>(m_value);
        }

        friend constexpr auto operator==(const ReferenceValue & lhs, const ReferenceValue & rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(const ReferenceValue & lhs, const ReferenceValue & rhs) noexcept -> bool = default;

        template<ReferenceDimension Dim>
        void reconstruct(ReferenceType type, SizeType at, StringBuilder & dest) const {
            SizeType val = dereference(type, at);
            if (type == ReferenceType::Absolute)
                dest.append(U'$');
            int count = 0;
            ++val;
            if constexpr (Dim == ReferenceDimension::Column) {
                do {
                    int rem = val % 26;
                    if (rem) {
                        dest.append(U'A' + rem - 1);
                    } else {
                        dest.append('Z');
                        val -= 26;
                    }
                    val /= 26;
                    ++count;
                } while(val != 0);
            } else {
                do {
                    char32_t digit = (val % 10) + U'0';
                    dest.append(digit);
                    val /= 10;
                    ++count;
                } while(val != 0);
            }
            //BUGBUG: this assumes that letters/digits are 1 char each. 
            std::reverse(dest.chars().end() - count, dest.chars().end());
        }

        template<SizeType MaxSize>
        constexpr auto isDereferencable(ReferenceType type, SizeType at) const noexcept -> bool {
            SPR_ASSERT_INPUT(at < MaxSize);
            if (type == ReferenceType::Absolute) {
                return true;
            }
            auto dist = asDistance();
            return dist < 0 ? (at >= SizeType(-dist)) : (MaxSize - dist > at);
        }

        constexpr auto adjustedToMove(ReferenceType type, DistanceType dist) const noexcept -> ReferenceValue {
            if (type == ReferenceType::Relative) {
                return ReferenceValue(asDistance() - dist);
            }
            return *this;
        }

        constexpr auto adjustedToErasure(ReferenceType type, SizeType at, SizeType from, SizeType size) const noexcept -> ReferenceValue {

            SPR_ASSERT_INPUT(at < from || at >= from + size);

            if (type == ReferenceType::Absolute) {
                auto pos = asSize();
                if (pos >= from) {
                    pos -= std::min(pos - from, size);
                    return ReferenceValue(pos);
                }
            } else {
                auto dist = asDistance();
                auto pos = at + dist;
                if (at < from && pos >= from) {
                    dist -= std::min(pos - from, size);
                    return ReferenceValue(dist);
                } else if (at >= from + size && pos < from + size) {
                    dist += std::min(from + size - pos, size);
                    return ReferenceValue(dist);
                }
            }
            return *this;
        }

        template<SizeType MaxSize>
        constexpr auto adjustedToInsertion(ReferenceType type, SizeType at, SizeType before, SizeType size) const noexcept 
            -> std::optional<ReferenceValue> {

            SPR_ASSERT_INPUT(at < MaxSize && before < MaxSize && size <= MaxSize && before <= MaxSize - size);

            if (type == ReferenceType::Absolute) {
                auto pos = asSize();
                if (pos >= before) {
                    if (pos >= MaxSize - size)
                        return std::nullopt;
                    pos += size;
                    return ReferenceValue(pos);
                }
            } else {
                auto dist = asDistance();
                auto pos = at + dist;
                if (at < before && pos >= before) {
                    if (pos >= MaxSize - size)
                        return std::nullopt;
                    dist += size;
                    return ReferenceValue(dist);
                } else if (at >= before && pos < before) {
                    //this is always safe as total negative distance cannot exceed MaxSize since both points 
                    //are guaranteed to be within it
                    dist -= size;
                    return ReferenceValue(dist);
                }
            }
            return *this;
        }

    private:
        SizeType m_value = 0;
    };
    
    class ReferenceTypeAndValue {
    public:
        constexpr ReferenceTypeAndValue() noexcept = default;
        
        constexpr ReferenceTypeAndValue(ReferenceType type, ReferenceValue value) noexcept:
            m_type(type),
            m_value(value) {
            
            SPR_ASSERT_INPUT((type == ReferenceType::Absolute && m_value.asSize() <= MaxSizeType) ||
                             (type == ReferenceType::Relative && SizeType(std::abs(m_value.asDistance())) <= MaxSizeType));
        }
        
        constexpr auto type() const noexcept -> ReferenceType {
            return m_type;
        }
        
        constexpr auto value() const noexcept -> ReferenceValue {
            return m_value;
        }
        
        constexpr auto dereference(SizeType at) const noexcept -> SizeType {
            return m_type == ReferenceType::Absolute ? m_value.asSize() : at + m_value.asDistance();
        }

        friend constexpr auto operator==(const ReferenceTypeAndValue & lhs, const ReferenceTypeAndValue & rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(const ReferenceTypeAndValue & lhs, const ReferenceTypeAndValue & rhs) noexcept -> bool = default;

        template<ReferenceDimension Dim>
        void reconstruct(SizeType at, StringBuilder & dest) const {
            m_value.template reconstruct<Dim>(m_type, at, dest);
        }
        
        template<SizeType MaxSize>
        constexpr auto isDereferencable(SizeType at) const noexcept -> bool {
            return m_value.template isDereferencable<MaxSize>(m_type, at);
        }

        constexpr auto adjustedToMove(DistanceType dist) const noexcept -> ReferenceTypeAndValue {
            return ReferenceTypeAndValue(m_type, m_value.adjustedToMove(m_type, dist));
        }
        
        constexpr auto adjustedToErasure(SizeType at, SizeType from, SizeType size) const noexcept -> ReferenceTypeAndValue {
            return ReferenceTypeAndValue(m_type, m_value.adjustedToErasure(m_type, at, from, size));
        }
        
        template<SizeType MaxSize>
        constexpr auto adjustedToInsertion(SizeType at, SizeType before, SizeType size) const noexcept -> std::optional<ReferenceTypeAndValue> {
            if (auto value = m_value.template adjustedToInsertion<MaxSize>(m_type, at, before, size))
                return ReferenceTypeAndValue(m_type, *value);
            return std::nullopt;
        }
        
    private:
        ReferenceType m_type = ReferenceType::Relative;
        ReferenceValue m_value;
    };

    class CellReference {
    public:
        static inline constexpr unsigned TypeId = 0x10;
    private:
        static inline constexpr unsigned xTypeBit = 1;
        static inline constexpr unsigned yTypeBit = 0;
    public:
        constexpr CellReference() noexcept = default;
        constexpr CellReference(ReferenceTypeAndValue x, ReferenceTypeAndValue y) :
            m_types(TypeId |
                    (unsigned(x.type()) << xTypeBit) | 
                    (unsigned(y.type()) << yTypeBit)),
            m_x(x.value()),
            m_y(y.value())
        {}

        constexpr auto x() const noexcept -> ReferenceTypeAndValue {
            return {xType(), m_x};
        }
        
        constexpr auto y() const noexcept -> ReferenceTypeAndValue {
            return {yType(), m_y};
        }
        
        constexpr auto dereference(Point at) const noexcept -> Point {
            return Point{m_x.dereference(xType(), at.x), m_y.dereference(yType(), at.y)};
        }

        template<Size MaxSize>
        constexpr auto isDereferencable(Point at) const noexcept -> bool {
            return m_x.isDereferencable<MaxSize.width>(xType(), at.x) &&
                   m_y.isDereferencable<MaxSize.height>(yType(), at.y);
        }

        friend constexpr auto operator==(const CellReference & lhs, const CellReference & rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(const CellReference & lhs, const CellReference & rhs) noexcept -> bool = default;

        void reconstruct(Point at, StringBuilder & dest) const {
            m_x.reconstruct<ReferenceDimension::Column>(xType(), at.x, dest);
            m_y.reconstruct<ReferenceDimension::Row>(yType(), at.y, dest);
        }
    private:
        constexpr auto xType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << xTypeBit)) >> xTypeBit);
        }
        constexpr auto yType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << yTypeBit)) >> yTypeBit);
        }
    private:
        unsigned m_types = TypeId;
        ReferenceValue m_x;
        ReferenceValue m_y;
    };

    class AreaReference {
    public:
        static inline constexpr unsigned TypeId = 0x20;
    private:
        static inline constexpr unsigned xStartTypeBit = 3;
        static inline constexpr unsigned yStartTypeBit = 2;
        static inline constexpr unsigned xEndTypeBit   = 1;
        static inline constexpr unsigned yEndTypeBit   = 0;
    public:
        constexpr AreaReference() noexcept = default;
        constexpr AreaReference(ReferenceTypeAndValue xStart,
                                ReferenceTypeAndValue yStart,
                                ReferenceTypeAndValue xEnd,
                                ReferenceTypeAndValue yEnd) noexcept :
            m_types(TypeId |
                    (unsigned(xStart.type()) << xStartTypeBit) |
                    (unsigned(yStart.type()) << yStartTypeBit) |
                    (unsigned(xEnd.type()) << xEndTypeBit) | 
                    (unsigned(yEnd.type()) << yEndTypeBit)),
            m_xStart(xStart.value()),
            m_yStart(yStart.value()),
            m_xEnd(xEnd.value()),
            m_yEnd(yEnd.value())
        {}

        constexpr auto xStart() const noexcept -> ReferenceTypeAndValue {
            return ReferenceTypeAndValue(xStartType(), m_xStart);
        }
    
        constexpr auto yStart() const noexcept -> ReferenceTypeAndValue {
            return ReferenceTypeAndValue(yStartType(), m_yStart);
        }

        constexpr auto xEnd() const noexcept -> ReferenceTypeAndValue {
            return ReferenceTypeAndValue(xEndType(), m_xEnd);
        }
        
        constexpr auto yEnd() const noexcept -> ReferenceTypeAndValue {
            return ReferenceTypeAndValue(yEndType(), m_yEnd);
        }

        constexpr auto dereference(Point at) const noexcept -> Rect {
            Point start = Point{m_xStart.dereference(xStartType(), at.x), m_yStart.dereference(yStartType(), at.y)};
            Point end = Point{m_xEnd.dereference(xEndType(), at.x), m_yEnd.dereference(yEndType(), at.y)};
            Point topLeft { std::min(start.x, end.x), std::min(start.y, end.y) };
            Point rightBottom { std::max(start.x, end.x), std::max(start.y, end.y) };

            return Rect{ 
                .origin = topLeft,
                .size = asSize(rightBottom) - asSize(topLeft) + Size{1, 1}
            };
        }

        template<Size MaxSize>
        constexpr auto isDereferencable(Point at) const noexcept -> bool {
            return m_xStart.isDereferencable<MaxSize.width>(xStartType(), at.x) &&
                   m_yStart.isDereferencable<MaxSize.height>(yStartType(), at.y) &&
                   m_xEnd.isDereferencable<MaxSize.width>(xEndType(), at.x) &&
                   m_yEnd.isDereferencable<MaxSize.height>(yEndType(), at.y);
        }


        friend constexpr auto operator==(const AreaReference & lhs, const AreaReference & rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(const AreaReference & lhs, const AreaReference & rhs) noexcept -> bool = default;

        void reconstruct(Point at, StringBuilder & dest) const {
            m_xStart.reconstruct<ReferenceDimension::Column>(xStartType(), at.x, dest);
            m_yStart.reconstruct<ReferenceDimension::Row>(yStartType(), at.y, dest);
            dest.append(U':');
            m_xEnd.reconstruct<ReferenceDimension::Column>(xEndType(), at.x, dest);
            m_yEnd.reconstruct<ReferenceDimension::Row>(yEndType(), at.y, dest);
        }

    private:
        constexpr auto xStartType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << xStartTypeBit)) >> xStartTypeBit);
        }
        constexpr auto yStartType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << yStartTypeBit)) >> yStartTypeBit);
        }
        constexpr auto xEndType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << xEndTypeBit)) >> xEndTypeBit);
        }
        constexpr auto yEndType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << yEndTypeBit)) >> yEndTypeBit);
        }

    private:
        unsigned m_types = TypeId;
        ReferenceValue m_xStart;
        ReferenceValue m_yStart;
        ReferenceValue m_xEnd;
        ReferenceValue m_yEnd;
    };

    template<ReferenceDimension Dim>
    class DimensionReference {
    public:
        static inline constexpr unsigned TypeId = (0x3u + unsigned(Dim)) << 4;
    private:
        static inline constexpr unsigned startTypeBit = 1;
        static inline constexpr unsigned endTypeBit = 0;
    public:
        constexpr DimensionReference() noexcept = default;
        constexpr DimensionReference(ReferenceTypeAndValue start, ReferenceTypeAndValue end) :
            m_types(TypeId |
                    (unsigned(start.type()) << startTypeBit) |
                    (unsigned(end.type()) << endTypeBit)),
            m_start(start.value()),
            m_end(end.value())
        {}

        constexpr auto start() const noexcept -> ReferenceTypeAndValue {
            return {startType(), m_start};
        }
        
        constexpr auto end() const noexcept -> ReferenceTypeAndValue {
            return {endType(), m_end};
        }
        
        constexpr auto dereference(Point at) const noexcept -> std::pair<SizeType, SizeType> /* start, size */{
            SizeType start = m_start.dereference(startType(), dimOf(at));
            SizeType end = m_end.dereference(endType(), dimOf(at));
            return start <= end ? std::make_pair(start, end - start + 1) : std::make_pair(end, start - end + 1);
        }

        template<Size MaxSize>
        constexpr auto isDereferencable(Point at) const noexcept -> bool {
            return m_start.isDereferencable<dimOf(MaxSize)>(startType(), dimOf(at)) &&
                   m_end.isDereferencable<dimOf(MaxSize)>(endType(), dimOf(at));
        }

        void reconstruct(Point at, StringBuilder & dest) const {
            m_start.reconstruct<Dim>(startType(), dimOf(at), dest);
            dest.append(U':');
            m_end.reconstruct<Dim>(endType(), dimOf(at), dest);
        }

        friend constexpr auto operator==(const DimensionReference & lhs, const DimensionReference & rhs) noexcept -> bool = default;
        friend constexpr auto operator!=(const DimensionReference & lhs, const DimensionReference & rhs) noexcept -> bool = default;

    private:
        constexpr auto startType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << startTypeBit)) >> startTypeBit);
        }
        constexpr auto endType() const noexcept -> ReferenceType {
            return ReferenceType((m_types & (1u << endTypeBit)) >> endTypeBit);
        }
        
        static constexpr auto dimOf(Point pt) noexcept -> SizeType {
            if constexpr (Dim == ReferenceDimension::Column)
                return pt.x;
            else
                return pt.y;
        }
        static constexpr auto dimOf(Size size) noexcept -> SizeType {
            if constexpr (Dim == ReferenceDimension::Column)
                return size.width;
            else
                return size.height;
        }
    private:
        unsigned m_types = TypeId;
        ReferenceValue m_start;
        ReferenceValue m_end;
    };

    using ColumnReference = DimensionReference<ReferenceDimension::Column>;
    using RowReference = DimensionReference<ReferenceDimension::Row>;

    class IllegalReference {
    public:
        static inline constexpr unsigned TypeId = 0x0;

        friend constexpr auto operator==(const IllegalReference & , const IllegalReference & ) noexcept -> bool {
            return true;
        }
        friend constexpr auto operator!=(const IllegalReference & , const IllegalReference & ) noexcept -> bool {
            return false;
        }

    private:
        unsigned m_types = TypeId;
    };

    union AnyReference {
    private:
        static inline constexpr unsigned TypeMask = IllegalReference::TypeId |
                                                    CellReference::TypeId |
                                                    AreaReference::TypeId |
                                                    ColumnReference::TypeId |
                                                    RowReference::TypeId;
    public:
        constexpr AnyReference() noexcept : m_illegal()
        {}
        constexpr AnyReference(const CellReference & src) noexcept: m_cell(src)
        {}
        constexpr AnyReference(CellReference && src) noexcept: m_cell(std::move(src))
        {}
        constexpr AnyReference(const AreaReference & src) noexcept : m_area(src)
        {}
        constexpr AnyReference(AreaReference && src) noexcept : m_area(std::move(src))
        {}
        constexpr AnyReference(const ColumnReference & src) noexcept : m_column(src)
        {}
        constexpr AnyReference(ColumnReference & src) noexcept : m_column(std::move(src))
        {}
        constexpr AnyReference(const RowReference & src) noexcept : m_row(src)
        {}
        constexpr AnyReference(RowReference & src) noexcept : m_row(std::move(src))
        {}

        constexpr auto typeId() const noexcept -> unsigned {
            static_assert(offsetof(AnyReference, m_type) == offsetof(AnyReference, m_illegal));
            static_assert(offsetof(AnyReference, m_illegal) == offsetof(AnyReference, m_cell));
            static_assert(offsetof(AnyReference, m_cell) == offsetof(AnyReference, m_area));
            static_assert(offsetof(AnyReference, m_area) == offsetof(AnyReference, m_column));
            static_assert(offsetof(AnyReference, m_column) == offsetof(AnyReference, m_row));

            return (m_type & TypeMask);
        }

        template <class Visitor, class Ref>
        requires(std::is_same_v<std::remove_cvref_t<Ref>, AnyReference>)
        friend auto visit(Visitor && visitor, Ref && ref) {
            unsigned typeId = std::forward<Ref>(ref).typeId();
            switch(typeId) {
                case IllegalReference::TypeId:
                    return std::forward<Visitor>(visitor)(std::forward<Ref>(ref).m_illegal);
                case CellReference::TypeId:
                    return std::forward<Visitor>(visitor)(std::forward<Ref>(ref).m_cell);
                case AreaReference::TypeId:
                    return std::forward<Visitor>(visitor)(std::forward<Ref>(ref).m_area);
                case ColumnReference::TypeId:
                    return std::forward<Visitor>(visitor)(std::forward<Ref>(ref).m_column);
                case RowReference::TypeId:
                    return std::forward<Visitor>(visitor)(std::forward<Ref>(ref).m_row);
                default:
                    SPR_ASSERT_LOGIC(false);
                    SPR_FATAL_ERROR("invalid reference type");
            }
        }

        friend constexpr auto operator==(const AnyReference & lhs, const AnyReference & rhs) noexcept -> bool {

            if (lhs.typeId() != rhs.typeId())
                return false;
            switch(lhs.typeId()) {
                case IllegalReference::TypeId:
                    return lhs.m_illegal == rhs.m_illegal;
                case CellReference::TypeId:
                    return lhs.m_cell == rhs.m_cell;
                case AreaReference::TypeId:
                    return lhs.m_area == rhs.m_area;
                case ColumnReference::TypeId:
                    return lhs.m_column == rhs.m_column;
                case RowReference::TypeId:
                    return lhs.m_row == rhs.m_row;
                default:
                    SPR_ASSERT_LOGIC(false);
                    SPR_FATAL_ERROR("invalid reference type");
            }
        }
        friend constexpr auto operator!=(const AnyReference & lhs, const AnyReference & rhs) noexcept -> bool  {
            return !(lhs == rhs);
        }

    private:
        unsigned m_type;
        IllegalReference m_illegal;
        CellReference m_cell;
        AreaReference m_area;
        ColumnReference m_column;
        RowReference m_row;
    };
    
}

#endif 
