#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


static auto stringizeLengths(std::string & buf)  {
    buf.clear();
    return [&](SizeType startIdx, SizeType endIdx, const Sheet::LengthInfo & info) {
        buf += "[";
        buf += std::to_string(startIdx);
        buf += ", ";
        buf += std::to_string(endIdx);
        buf += ", ";
        if (info.length) {
            buf += std::to_string(*info.length);
            buf += ", ";
        }
        buf += info.hidden ? "true" : "false";
        buf += "]";
    };
}

TEST_CASE( "Row height", "[sheet-insert-delete]" ) {

    {
        Sheet s;
        
        s.setRowHeight(3, 1, 12);
        CHECK(s.size().height == 4);
        CHECK(s.getRowHeight(3) == Sheet::LengthInfo{12, false});
        
        std::string buf;
        s.forEachRowHeight(0, s.size().height, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, 12, false]");
        
        s.clearRowHeight(3, 1);
        s.forEachRowHeight(0, s.size().height, stringizeLengths(buf));
        CHECK(buf == "[0, 4, false]");
        
        s.setRowHeight(3, 1, 12);
        s.deleteRows(3, 1);
        s.forEachRowHeight(0, s.size().height + 1, stringizeLengths(buf));
        CHECK(buf == "[0, 4, false]");
        
        s.setRowHeight(3, 1, 12);
        s.insertRows(3, 5);
        s.forEachRowHeight(0, s.size().height + 1, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 9, 12, false][9, 10, false]");
        
    }
    {
        Sheet s;
        
        s.hideRow(3, 1, false);
        CHECK(s.size().height == 4);
        CHECK(s.getRowHeight(3) == Sheet::LengthInfo{std::nullopt, false});
        
        std::string buf;
        s.forEachRowHeight(0, s.size().height, stringizeLengths(buf));
        CHECK(buf == "[0, 4, false]");
        
        s.hideRow(3, 1, true);
        s.forEachRowHeight(0, s.size().height, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, true]");
        
        s.setRowHeight(3, 1, 12);
        s.forEachRowHeight(0, s.size().height, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, 12, true]");
        
        s.clearRowHeight(3, 1);
        s.forEachRowHeight(0, s.size().height, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, true]");
        
        s.setRowHeight(3, 1, 12);
        s.hideRow(3, 1, false);
        s.forEachRowHeight(0, s.size().height, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, 12, false]");
        
        auto str = stringizeLengths(buf);
        for(auto tup: s.rowHeights(0, s.size().height)) {
            auto & [startIdx, endIdx, info] = tup;
            str(startIdx, endIdx, info);
        }
        CHECK(buf == "[0, 3, false][3, 4, 12, false]");
    }
}

TEST_CASE( "Column width", "[sheet-insert-delete]" ) {

    {
        Sheet s;
        
        s.setColumnWidth(3, 1, 12);
        CHECK(s.size().width == 4);
        CHECK(s.getColumnWidth(3) == Sheet::LengthInfo{12, false});
        
        std::string buf;
        s.forEachColumnWidth(0, s.size().width, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, 12, false]");
        
        s.clearColumnWidth(3, 1);
        s.forEachColumnWidth(0, s.size().width, stringizeLengths(buf));
        CHECK(buf == "[0, 4, false]");
        
        s.setColumnWidth(3, 1, 12);
        s.deleteColumns(3, 1);
        s.forEachColumnWidth(0, s.size().width + 1, stringizeLengths(buf));
        CHECK(buf == "[0, 4, false]");
        
        s.setColumnWidth(3, 1, 12);
        s.insertColumns(3, 5);
        s.forEachColumnWidth(0, s.size().width + 1, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 9, 12, false][9, 10, false]");
        
    }
    {
        Sheet s;
        
        s.hideColumn(3, 1, false);
        CHECK(s.size().width == 4);
        CHECK(s.getColumnWidth(3) == Sheet::LengthInfo{std::nullopt, false});
        
        std::string buf;
        s.forEachColumnWidth(0, s.size().width, stringizeLengths(buf));
        CHECK(buf == "[0, 4, false]");
        
        s.hideColumn(3, 1, true);
        s.forEachColumnWidth(0, s.size().width, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, true]");
        
        s.setColumnWidth(3, 1, 12);
        s.forEachColumnWidth(0, s.size().width, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, 12, true]");
        
        s.clearColumnWidth(3, 1);
        s.forEachColumnWidth(0, s.size().width, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, true]");
        
        s.setColumnWidth(3, 1, 12);
        s.hideColumn(3, 1, false);
        s.forEachColumnWidth(0, s.size().width, stringizeLengths(buf));
        CHECK(buf == "[0, 3, false][3, 4, 12, false]");
    }
}

