#include <spreader/sheet.h>
#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Index to column", "[sheet-parsing]" ) {

    Sheet s;

    CHECK(s.indexToColumn(0) == SPRS("A"));
    CHECK(s.indexToColumn(1) == SPRS("B"));
    CHECK(s.indexToColumn(25) == SPRS("Z"));
    CHECK(s.indexToColumn(26) == SPRS("AA"));
    CHECK(s.indexToColumn(27) == SPRS("AB"));
    CHECK(s.indexToColumn(675) == SPRS("YZ"));
    CHECK(s.indexToColumn(676) == SPRS("ZA"));
    CHECK(s.indexToColumn(677) == SPRS("ZB"));
    CHECK(s.indexToColumn(s.maxSize().width - 1) == SPRS("CRXO"));
}

TEST_CASE( "Column to index", "[sheet-parsing]" ) {

    Sheet s;

    CHECK(s.parseColumn(SPRS("A")) == 0u);
    CHECK(s.parseColumn(SPRS("B")) == 1u);
    CHECK(s.parseColumn(SPRS("Z")) == 25u);
    CHECK(s.parseColumn(SPRS("AA")) == 26u);
    CHECK(s.parseColumn(SPRS("AB")) == 27u);
    CHECK(s.parseColumn(SPRS("YZ")) == 675u);
    CHECK(s.parseColumn(SPRS("ZA")) == 676u);
    CHECK(s.parseColumn(SPRS("ZB")) == 677u);
    CHECK(s.parseColumn(SPRS("CRXO")) == s.maxSize().width - 1);
    
    CHECK(s.parseColumn(SPRS("")) == std::nullopt);
    CHECK(s.parseColumn(SPRS("1")) == std::nullopt);
    CHECK(s.parseColumn(SPRS("CRXP")) == std::nullopt);
}

TEST_CASE( "Index to row", "[sheet-parsing]" ) {

    Sheet s;

    CHECK(s.indexToRow(0) == SPRS("1"));
    CHECK(s.indexToRow(1) == SPRS("2"));
    CHECK(s.indexToRow(9) == SPRS("10"));
    CHECK(s.indexToRow(10) == SPRS("11"));
    CHECK(s.indexToRow(11) == SPRS("12"));
    CHECK(s.indexToRow(99) == SPRS("100"));
    CHECK(s.indexToRow(s.maxSize().height - 1) == SPRS("2147483647"));
}

TEST_CASE( "Row to index", "[sheet-parsing]" ) {

    Sheet s;

    CHECK(s.parseRow(SPRS("1")) == 0u);
    CHECK(s.parseRow(SPRS("2")) == 1u);
    CHECK(s.parseRow(SPRS("10")) == 9u);
    CHECK(s.parseRow(SPRS("11")) == 10u);
    CHECK(s.parseRow(SPRS("12")) == 11u);
    CHECK(s.parseRow(SPRS("100")) == 99u);
    CHECK(s.parseRow(SPRS("2147483647")) == s.maxSize().height - 1);
    
    CHECK(s.parseRow(SPRS("")) == std::nullopt);
    CHECK(s.parseRow(SPRS("-1")) == std::nullopt);
    CHECK(s.parseRow(SPRS("A")) == std::nullopt);
    CHECK(s.parseRow(SPRS("2147483648")) == std::nullopt);
}

TEST_CASE( "Parse point", "[sheet-parsing]" ) {
    
    Sheet s;
    
    CHECK(s.parsePoint(SPRS("A1")) == Spreader::Point{0, 0});
    CHECK(s.parsePoint(SPRS("CRXO2147483647")) == Spreader::Point{s.maxSize().width - 1, s.maxSize().height - 1});
    
    CHECK(s.parsePoint(SPRS("A")) == std::nullopt);
    CHECK(s.parsePoint(SPRS("A1:")) == std::nullopt);
    CHECK(s.parsePoint(SPRS(" A1")) == std::nullopt);
}

TEST_CASE( "Parse rect", "[sheet-parsing]" ) {
    
    Sheet s;
    s.setValueCell({21, 32}, "0");
    
    CHECK(s.parseArea(SPRS("A1")) == Spreader::Rect{Spreader::Point{0, 0}, Spreader::Size{1, 1}});
    CHECK(s.parseArea(SPRS("A:B")) == Spreader::Rect{Spreader::Point{0, 0}, Spreader::Size{2, 33}});
    CHECK(s.parseArea(SPRS("2:3")) == Spreader::Rect{Spreader::Point{0, 1}, Spreader::Size{22, 2}});
    CHECK(s.parseArea(SPRS("B2:C3")) == Spreader::Rect{Spreader::Point{1, 1}, Spreader::Size{2, 2}});
    CHECK(s.parseArea(SPRS("C3:B2")) == Spreader::Rect{Spreader::Point{1, 1}, Spreader::Size{2, 2}});
    CHECK(s.parseArea(SPRS("A1:CRXO2147483647")) == Spreader::Rect{Spreader::Point{0, 0}, Spreader::Size{s.maxSize().width, s.maxSize().height}});
    
    CHECK(s.parseArea(SPRS("A0")) == std::nullopt);
    CHECK(s.parseArea(SPRS("A1:")) == std::nullopt);
    CHECK(s.parseArea(SPRS("A1 ")) == std::nullopt);
    CHECK(s.parseArea(SPRS(" A1")) == std::nullopt);
    
    CHECK(s.parseArea(SPRS("A")) == std::nullopt);
    CHECK(s.parseArea(SPRS(" A")) == std::nullopt);
    CHECK(s.parseArea(SPRS("A:")) == std::nullopt);
    CHECK(s.parseArea(SPRS("A:1")) == std::nullopt);
    
    CHECK(s.parseArea(SPRS("1")) == std::nullopt);
    CHECK(s.parseArea(SPRS("1 ")) == std::nullopt);
    CHECK(s.parseArea(SPRS(" 1")) == std::nullopt);
    CHECK(s.parseArea(SPRS("1:")) == std::nullopt);
    CHECK(s.parseArea(SPRS("1:A")) == std::nullopt);
    
    CHECK(s.parseArea(SPRS("A1:B")) == std::nullopt);
    CHECK(s.parseArea(SPRS("A1: B1")) == std::nullopt);
    CHECK(s.parseArea(SPRS("A1:B1 ")) == std::nullopt);
    CHECK(s.parseArea(SPRS(" A1:B1")) == std::nullopt);
}

