// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_NAME_MANAGER_H_INCLUDED
#define SPR_HEADER_NAME_MANAGER_H_INCLUDED

#include <spreader/geometry.h>

namespace Spreader {

    class NameManager {
    private:
        using Char = String::storage_type;
        using Cursor = String::char_access::cursor;
    public:

        auto parseColumn(const String & str) const -> std::optional<SizeType>;
        auto indexToColumn(SizeType x) const -> String;
        void indexToColumn(SizeType x, StringBuilder & dest) const;
        auto parseRow(const String & str) const -> std::optional<SizeType>;
        auto indexToRow(SizeType y) const -> String;
        void indexToRow(SizeType y, StringBuilder & dest) const;
        auto parsePoint(const String & str) const -> std::optional<Point>;
        auto parseArea(const String & str, Size bounds) const -> std::optional<Rect>;


    private:
        template<class Accumulator>
        auto indexToColumn(SizeType x, Accumulator acc) const;
        template<class Accumulator>
        auto indexToRow(SizeType y, Accumulator acc) const;
        
        static auto parseColumnNumber(Cursor & cur) noexcept -> std::optional<SizeType>;
        static auto parseRowNumber(Cursor & cur) noexcept -> std::optional<SizeType>;
        
        static auto parseColumnImpl(Cursor & cur) noexcept -> std::optional<SizeType>;
        static auto parseRowImpl(Cursor & cur) noexcept -> std::optional<SizeType>;
    };

}


#endif 

