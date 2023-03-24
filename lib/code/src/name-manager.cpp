// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/name-manager.h>
#include <spreader/cell-grid.h>

using namespace Spreader;

using sysstr::util::cursor_begin;
using sysstr::util::cursor_direction;

auto NameManager::parseColumnNumber(Cursor & cur) noexcept -> std::optional<SizeType> {

    Cursor p = cur;

    bool hasColumn = false;
    SizeType column = 0;
    for( ; p; ++p) {
        Char c = *p;
        SizeType digit;
        if (c >= Char('A') && c <= Char('Z')) {
            digit = c - Char('A') + 1;
        } else if (c >= Char('a') && c <= Char('z')) {
            digit = c - Char('a') + 1;
        } else {
            break;
        }

        if ((CellGrid::maxSize().width - digit) / 26 < column) {
            return std::nullopt;
        }
        column = (column * 26) + digit;
        hasColumn = true;
    }
    if (!hasColumn)
        return std::nullopt;
    SPR_ASSERT_LOGIC(column > 0);
    --column;

    cur = std::move(p);
    return column;
}

auto NameManager::parseRowNumber(Cursor & cur) noexcept -> std::optional<SizeType> {

    Cursor p = cur;

    if (!p)
        return std::nullopt;
    
    Char c = *p;
    if (c < Char('1') || c > Char('9'))
        return std::nullopt;

    SizeType row = c - Char('0');
    for(++p ; p; ++p) {
        c = *p;
        if (c < Char('0') || c > Char('9'))
            break;

        SizeType digit = c - Char('0');
        if ((CellGrid::maxSize().height - digit) / 10 < row) {
            return std::nullopt;
        }
        row = (row * 10) + digit;
    }
    SPR_ASSERT_LOGIC(row > 0);
    --row;

    cur = std::move(p);
    return row;
}

template<class Accumulator>
auto NameManager::indexToColumn(SizeType x, Accumulator acc) const {
    SPR_ASSERT_INPUT(x < CellGrid::maxSize().width);

    //constexpr size_t maxDigits = log2(maxSize().width) / log2(26) + 1;
    constexpr auto maxDigits = size_t((sizeof(SizeType) * CHAR_BIT) / 4.700439718141092 + 1);

    using Char = String::storage_type;
    
    ++x;
    Char buf[maxDigits];
    int count = 0;
    do {
        int rem = x % 26;
        if (rem) {
            buf[count++] = Char('A' + rem - 1);
        } else {
            buf[count++] = Char('Z');
            x -= 26;
        }
        x /= 26;
        SPR_ASSERT_LOGIC(size_t(count) <= maxDigits);
    } while (x > 0);
    std::reverse(buf, buf + count);
    return acc(buf, count);
}

template<class Accumulator>
auto NameManager::indexToRow(SizeType y, Accumulator acc) const {
    SPR_ASSERT_INPUT(y < CellGrid::maxSize().height);

    //constexpr size_t maxDigits = log2(maxSize().height) / log2(10) + 1;
    constexpr auto maxDigits = size_t((sizeof(SizeType) * CHAR_BIT) / 3.321928094887362 + 1);

    ++y;
    Char buf[maxDigits];
    int count = 0;
    do {
        int rem = y % 10;
        y /= 10;
        buf[count++] = Char('0' + rem);
        SPR_ASSERT_LOGIC(size_t(count) <= maxDigits);
    } while (y > 0);
    std::reverse(buf, buf + count);
    return acc(buf, count);
}

auto NameManager::indexToColumn(SizeType x) const -> String {

    return indexToColumn(x, [](const String::storage_type * buf, int count) {
        return String(buf, count);
    });
}

void NameManager::indexToColumn(SizeType x, StringBuilder & dest) const {

    indexToColumn(x, [&](const String::storage_type * buf, int count) {
        dest.append(buf, count);
    });
}

auto NameManager::indexToRow(SizeType y) const -> String {
    return indexToRow(y, [](const String::storage_type * buf, int count) {
        return String(buf, count);
    });
}

void NameManager::indexToRow(SizeType y, StringBuilder & dest) const {

    indexToRow(y, [&](const String::storage_type * buf, int count) {
        dest.append(buf, count);
    });
}

auto NameManager::parseColumnImpl(Cursor & it) noexcept -> std::optional<SizeType> {
 
    if (!it)
        return std::nullopt;
    if (*it == Char('$'))
        ++it;
    return parseColumnNumber(it);
}

auto NameManager::parseRowImpl(Cursor & it) noexcept -> std::optional<SizeType> {
 
    if (!it)
        return std::nullopt;
    if (*it == Char('$'))
        ++it;
    return parseRowNumber(it);
}

auto NameManager::parseColumn(const String & str) const -> std::optional<SizeType> {
    auto str_acces = String::char_access(str);
    auto p = cursor_begin<cursor_direction::forward>(str_acces);

    auto column = parseColumnImpl(p);
    if (!column || bool(p))
        return std::nullopt;

    return *column;
}

auto NameManager::parseRow(const String & str) const -> std::optional<SizeType> {
    auto str_acces = String::char_access(str);
    auto p = cursor_begin<cursor_direction::forward>(str_acces);

    auto row = parseRowImpl(p);
    if (!row || bool(p))
        return std::nullopt;

    return *row;
}

auto NameManager::parsePoint(const String & str) const -> std::optional<Point> {

    auto str_acces = String::char_access(str);
    auto p = cursor_begin<cursor_direction::forward>(str_acces);

    auto column = parseColumnImpl(p);
    if (!column)
        return std::nullopt;
    auto row = parseRowImpl(p);
    if (!row)
        return std::nullopt;
    if (p)
        return std::nullopt;
    return Point{*column, *row};
}

auto NameManager::parseArea(const String & str, Size bounds) const -> std::optional<Rect> {

    auto str_acces = String::char_access(str);
    auto p = cursor_begin<cursor_direction::forward>(str_acces);

    auto startColumn = parseColumnImpl(p);
    if (!startColumn) {
        auto startRow = parseRowImpl(p);
        if (!startRow || !p || *p != ':')
            return std::nullopt;
        ++p;
        auto endRow = parseRowImpl(p);
        if (!endRow || p)
            return std::nullopt;
        auto start = *startRow;
        auto end = *endRow;
        if (start > end)
            std::swap(start, end);
        return Rect {
            .origin = { 0, start },
            .size = { bounds.width, end - start + 1 }
        };
    }
    if (!p)
        return std::nullopt;

    if (*p == ':') {
        ++p;
        auto endColumn = parseColumnImpl(p);
        if (!endColumn || p)
            return std::nullopt;
        auto start = *startColumn;
        auto end = *endColumn;
        if (start > end)
            std::swap(start, end);
        return Rect {
            .origin = { start, 0 },
            .size = { end - start + 1, bounds.height }
        };
    }

    auto startRow = parseRowImpl(p);
    if (!startRow)
        return std::nullopt;
    if (!p) {
        return Rect {
            .origin = { *startColumn, *startRow },
            .size = { 1, 1 }
        };
    }
    if (*p != ':')
        return std::nullopt;
    ++p;
    auto endColumn = parseColumnImpl(p);
    if (!endColumn || !p)
        return std::nullopt;
    auto endRow = parseRowImpl(p);
    if (!endRow || p)
        return std::nullopt;

    auto startX = *startColumn;
    auto endX = *endColumn;
    if (startX > endX)
        std::swap(startX, endX);
    auto startY = *startRow;
    auto endY = *endRow;
    if (startY > endY)
        std::swap(startY, endY);
    
    return Rect{
        .origin = { startX, startY},
        .size = { endX - startX + 1, endY - startY + 1 }
    };
}
