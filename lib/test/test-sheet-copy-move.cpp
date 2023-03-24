#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Copy single cell", "[sheet-copy-move]" ) {

    {
        Sheet s;
        s.setValueCell(PT("A1"), true);
        s.copyCell(PT("A1"), AREA("A1:A1"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.size() == Spreader::Size{1, 1});
        CHECK(s.nonNullCellCount() == 1);
    }

    {
        Sheet s;
        s.setValueCell(PT("A1"), true);
        s.copyCell(PT("A1"), AREA("A2:A2"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.getValue(PT("A2")) == true);
        CHECK(s.size() == Spreader::Size{1, 2});
        CHECK(s.nonNullCellCount() == 2);
    }

    {
        Sheet s;
        s.setValueCell(PT("B2"), true);
        s.copyCell(PT("B2"), AREA("A1:A1"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.size() == Spreader::Size{2, 2});
        CHECK(s.nonNullCellCount() == 2);
    }

    {
        Sheet s;
        s.setValueCell(PT("CRXO2147483647"), true);
        s.copyCell(PT("CRXO2147483647"), AREA("A1:A1"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.size() == Spreader::Size{65535,2147483647});
        CHECK(s.nonNullCellCount() == 2);
    }

    {
        Sheet s;
        s.setFormulaCell(PT("C3"), "D4 + B3 + 1");
        s.copyCell(PT("C3"), AREA("A1:A1"));
        CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A1")) == Sheet::FormulaInfo{SPRS("B2 + #REF! + 1"), Spreader::Size{1, 1}});
        CHECK(s.nonNullCellCount() == 2);
    }
    
    {
        Sheet s;
        s.setFormulaCell(PT("A1"), "CRXO2147483647 + 1");
        s.copyCell(PT("A1"), AREA("A2"));
        CHECK(s.getValue(PT("A2")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A2")) == Sheet::FormulaInfo{SPRS("#REF! + 1"), Spreader::Size{1, 1}});
        CHECK(s.nonNullCellCount() == 2);
    }
    
    {
        Sheet s;
        s.setFormulaCell(PT("A1"), "C1:D2 + 1");
        s.setValueCell(PT("E4"), SPRS("hello"));
        s.copyCell(PT("F4"), AREA("A2"));
        CHECK(s.getValue(PT("A1")) == 1);
        CHECK(s.nonNullCellCount() == 5);
        
        CHECK(s.getValue(PT("A2")) == 1);
        s.copyCell(PT("A2"), AREA("F4"));
        CHECK(s.getValue(PT("F4")) == Scalar::Blank{});
        CHECK(s.nonNullCellCount() == 5);
        
        s.copyCell(PT("E4"), AREA("A2"));
        CHECK(s.getValue(PT("A2")) == SPRS("hello"));
        CHECK(s.getValue(PT("A1")) == Error::Spill);
        CHECK(s.getValue(PT("E4")) == SPRS("hello"));
        CHECK(s.nonNullCellCount() == 3);
        
        s.copyCell(PT("M4"), AREA("A2"));
        CHECK(s.getValue(PT("A2")) == 1);
        CHECK(s.getValue(PT("A1")) == 1);
        CHECK(s.nonNullCellCount() == 5);
        
        s.setFormulaCell(PT("E4"), SPRS("M5:N6 + 2"));
        CHECK(s.nonNullCellCount() == 8);
        s.copyCell(PT("E4"), AREA("A1"));
        CHECK(s.getValue(PT("A1")) == 2);
        CHECK(s.getValue(PT("A2")) == 2);
        CHECK(s.getValue(PT("B1")) == 2);
        CHECK(s.getValue(PT("B2")) == 2);
        CHECK(s.nonNullCellCount() == 8);
        
        s.copyCell(PT("E5"), AREA("A2"));
        CHECK(s.getValue(PT("A2")) == 2);
        CHECK(s.nonNullCellCount() == 8);
        
        s.setValueCell(PT("E4"), true);
        s.copyCell(PT("E4"), AREA("A1"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.nonNullCellCount() == 2);
    }
}

TEST_CASE( "Copy cell area", "[sheet-copy-move]" ) {

    {
        Sheet s;
        s.setValueCell(PT("A1"), true);
        s.copyCells(AREA("A1"), PT("A1"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.size() == Spreader::Size{1, 1});
        CHECK(s.nonNullCellCount() == 1);
    }
    
    {
        Sheet s;
        s.setValueCell(PT("B2"), true);
        s.copyCells(AREA("B2"), PT("A1"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.size() == Spreader::Size{2, 2});
        CHECK(s.nonNullCellCount() == 2);
    }
    
    {
        Sheet s;
        s.setValueCell(PT("A1"), true);
        s.setValueCell(PT("A2"), SPRS("hello"));
        s.setValueCell(PT("B1"), 1.);
        s.setValueCell(PT("B2"), Error::InvalidName);
        s.copyCells(AREA("A1:B2"), PT("C1"));
        CHECK(s.getValue(PT("C1")) == true);
        CHECK(s.getValue(PT("C2")) == SPRS("hello"));
        CHECK(s.getValue(PT("D1")) == 1.);
        CHECK(s.getValue(PT("D2")) == Error::InvalidName);
        CHECK(s.size() == Spreader::Size{4, 2});
        CHECK(s.nonNullCellCount() == 8);
    }
}

TEST_CASE( "Copy boundaries", "[sheet-copy-move]" ) {
    
    //exhausts allocator page
    {
        constexpr SizeType Width = 512;
        constexpr SizeType Height = 2;
        constexpr double Count = double(Width) * double(Height);
        
        Sheet s;
        s.setValueCell(PT("A1"), 0.);
        s.setFormulaCell(PT("B1"), "A1 + 1");
        s.copyCell(PT("B1"), Spreader::Rect{.origin = {1, 0}, .size = {Width - 1, Height}});
        s.setFormulaCell(PT("A2"), s.indexToColumn(Width - 1) + SPRS("1 + 1"));
        s.copyCell(PT("A2"), Spreader::Rect{.origin = {0, 1}, .size = {1, Height - 1}});
        CHECK(s.getValue({Width - 1, Height - 1}) == Count - 1);
    }
    
    //over more than two tiles
    {
        constexpr SizeType Width = 515;
        constexpr SizeType Height = 2;
        constexpr double Count = double(Width) * double(Height);
        
        Sheet s;
        s.setValueCell(PT("A1"), 0.);
        s.setFormulaCell(PT("B1"), "A1 + 1");
        s.copyCell(PT("B1"), Spreader::Rect{.origin = {1, 0}, .size = {Width - 1, Height}});
        s.setFormulaCell(PT("A2"), s.indexToColumn(Width - 1) + SPRS("1 + 1"));
        s.copyCell(PT("A2"), Spreader::Rect{.origin = {0, 1}, .size = {1, Height - 1}});
        CHECK(s.getValue({Width - 1, Height - 1}) == Count - 1);
    }
}

TEST_CASE( "Move single cell", "[sheet-copy-move]" ) {
    
    {
        Sheet s;
        s.setValueCell(PT("A1"), true);
        s.moveCell(PT("A1"), PT("A1"));
        CHECK(s.getValue(PT("A1")) == true);
        CHECK(s.size() == Spreader::Size{1, 1});
        CHECK(s.nonNullCellCount() == 1);
    }
    
    {
        Sheet s;
        s.setValueCell(PT("A1"), true);
        s.moveCell(PT("A1"), PT("A2"));
        CHECK(s.getValue(PT("A2")) == true);
        CHECK(s.getValue(PT("A1")) == Scalar::Blank{});
        CHECK(s.size() == Spreader::Size{1, 2});
        CHECK(s.nonNullCellCount() == 1);
    }
    
    {
        Sheet s;
        s.setFormulaCell(PT("CRXO2147483647"), "CRXO2147483646 + 1");
        s.moveCell(PT("CRXO2147483647"), PT("A2"));
        CHECK(s.getValue(PT("A2")) == 1);
        CHECK(s.getFormulaInfo(PT("A2")) == Sheet::FormulaInfo{SPRS("CRXO2147483646 + 1"), Spreader::Size{1, 1}});
        CHECK(s.nonNullCellCount() == 1);
    }
    
}
