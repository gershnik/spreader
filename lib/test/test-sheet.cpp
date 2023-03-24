#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Simple sheet calc", "[sheet]" ) {

    Sheet s;

    s.setValueCell(PT("A1"), 5.);
    s.setValueCell(PT("A2"), 2.);
    s.setFormulaCell(PT("A3"), SPRS("SUM(A1:A2)"));

    CHECK(s.getValue(PT("A3")) == 7.);

    s.setValueCell(PT("A1"), 3.);
    CHECK(s.getValue(PT("A3")) == 5.);

    s.setFormulaCell(PT("B1"), SPRS("A3 - A1"));
    CHECK(s.getValue(PT("B1")) == 2.);

    s.setValueCell(Spreader::Point{0, 1}, 10.);
    CHECK(s.getValue(PT("A3")) == 13.);
    CHECK(s.getValue(PT("B1")) == 10.);
}

TEST_CASE( "Array sheet calc", "[sheet]" ) {

    Sheet s;

    s.setValueCell(PT("A1"), 5.);
    s.setValueCell(PT("A2"), 2.);
    s.setValueCell(PT("B1"), 7.);
    s.setValueCell(PT("B2"), 8.);

    s.setFormulaCell(PT("C4"), SPRS("1 + A1:B2"));
    CHECK(s.getValue(PT("C4")) == 6.);
    CHECK(s.getValue(PT("C5")) == 3.);
    CHECK(s.getValue(PT("D4")) == 8.);
    CHECK(s.getValue(PT("D5")) == 9.);

    s.setValueCell(PT("B2"), 25.);
    CHECK(s.getValue(PT("D5")) == 26.);
}

TEST_CASE( "Simple sheet circular calc", "[sheet]" ) {

    Sheet s;

    s.setValueCell(PT("A2"), 2.);
    s.setFormulaCell(PT("A3"), SPRS("SUM(A1:A2)"));
    s.setFormulaCell(PT("A4"), SPRS("A3 + 1"));
    s.setFormulaCell(PT("A1"), SPRS("A4 / 2"));

    CHECK(s.getValue(PT("A3")) == Error::InvalidReference);
    CHECK(s.getValue(PT("A4")) == Error::InvalidReference);
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    
    s.setFormulaCell(PT("A4"), SPRS("SUM(A3:A1)"));
    CHECK(s.getValue(PT("A3")) == Error::InvalidReference);
    CHECK(s.getValue(PT("A4")) == Error::InvalidReference);
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
}

TEST_CASE( "Array sheet circular calc", "[sheet]" ) {

    Sheet s;

    s.setValueCell(PT("A1"), 5.);
    s.setValueCell(PT("A2"), 2.);
    s.setValueCell(PT("B1"), 7.);
    s.setValueCell(PT("B2"), 8.);

    s.setFormulaCell(PT("C4"), SPRS("1 + A1:B2"));
    s.setFormulaCell(PT("A1"), SPRS("D4 - 1"));
    
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    CHECK(s.getValue(PT("C4")) == Error::InvalidReference);
    CHECK(s.getValue(PT("C5")) == Scalar::Blank{});
    CHECK(s.getValue(PT("D4")) == Scalar::Blank{});
    CHECK(s.getValue(PT("D5")) == Scalar::Blank{});

    s.setValueCell(PT("A1"), 5.);
    CHECK(s.getValue(PT("C4")) == 6.);
    CHECK(s.getValue(PT("C5")) == 3.);
    CHECK(s.getValue(PT("D4")) == 8.);
    CHECK(s.getValue(PT("D5")) == 9.);
}

TEST_CASE( "Array replacement", "[sheet]" ) {
    
    Sheet s;

    s.setValueCell(PT("A1"), 5.);
    s.setValueCell(PT("A2"), 2.);
    s.setValueCell(PT("B1"), 7.);
    s.setValueCell(PT("B2"), 8.);

    s.setFormulaCell(PT("C4"), SPRS("1 + A1:B2"));
    CHECK(s.getValue(PT("C4")) == 6.);
    CHECK(s.getValue(PT("C5")) == 3.);
    CHECK(s.getValue(PT("D4")) == 8.);
    CHECK(s.getValue(PT("D5")) == 9.);
    
    s.setFormulaCell(PT("C4"), SPRS("1 + A1:A2"));
    CHECK(s.getValue(PT("C4")) == 6.);
    CHECK(s.getValue(PT("C5")) == 3.);
    CHECK(s.getValue(PT("D4")) == Scalar::Blank{});
    CHECK(s.getValue(PT("D5")) == Scalar::Blank{});
    
}

TEST_CASE( "Spill", "[sheet]" ) {
    
    Sheet s;

    s.setValueCell(PT("A1"), 5.);
    s.setValueCell(PT("A2"), 2.);
    s.setValueCell(PT("B1"), 7.);
    s.setValueCell(PT("B2"), 8.);

    s.setFormulaCell(PT("C4"), SPRS("1 + A1:B2"));
    CHECK(s.getValue(PT("C4")) == 6.);
    CHECK(s.getValue(PT("C5")) == 3.);
    CHECK(s.getValue(PT("D4")) == 8.);
    CHECK(s.getValue(PT("D5")) == 9.);
    
    s.setValueCell(PT("D4"), 42.);
    CHECK(s.getValue(PT("C4")) == Error::Spill);
    CHECK(s.getValue(PT("C5")) == Scalar::Blank{});
    CHECK(s.getValue(PT("D4")) == 42.);
    CHECK(s.getValue(PT("D5")) == Scalar::Blank{});
}

TEST_CASE( "Array collision", "[sheet]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("B1"), SPRS("D1:D3 + 1"));
    s.setFormulaCell(PT("A2"), SPRS("D1:F1 + 2"));
    
    CHECK(s.getValue(PT("B1")) == 1.);
    CHECK(s.getValue(PT("A2")) == Error::Spill);
    
    s.setFormulaCell(PT("B1"), SPRS("D1:D3 + 3"));
    
    CHECK(s.getValue(PT("B1")) == 3.);
    CHECK(s.getValue(PT("A2")) == Error::Spill);
    
}

TEST_CASE( "Array on edge", "[sheet]" ) {
    
    Sheet s;
    s.setFormulaCell(PT("CRXO2147483647"), "A1:B2 + 1");
    CHECK(s.nonNullCellCount() == 1);
    CHECK(s.getValue(PT("CRXO2147483647")) == 1);
}

TEST_CASE( "Clearing cells", "[sheet]" ) {

    {
        Sheet s;
        
        s.clearCell(PT("A1"));
        CHECK(s.getValue(PT("A1")) == Scalar::Blank{});
        CHECK(s.getFormulaInfo(PT("A1")) == std::nullopt);
        CHECK(s.nonNullCellCount() == 0);

        s.setValueCell(PT("A1"), 42.);
        s.clearCell(PT("A1"));
        CHECK(s.getValue(PT("A1")) == Scalar::Blank{});
        CHECK(s.nonNullCellCount() == 0);

        s.setFormulaCell(PT("A1"), SPRS("1 + 3"));
        s.clearCell(PT("A1"));
        CHECK(s.getValue(PT("A1")) == Scalar::Blank{});
        CHECK(s.nonNullCellCount() == 0);

    }

    {
        Sheet s;

        s.setValueCell(PT("CRXO2147483647"), 42.);
        s.clearCell(PT("CRXO2147483647"));
        CHECK(s.getValue(PT("CRXO2147483647")) == Scalar::Blank{});

        s.setFormulaCell(PT("CRXO2147483647"), SPRS("1 + 3"));
        s.clearCell(PT("CRXO2147483647"));
        CHECK(s.getValue(PT("CRXO2147483647")) == Scalar::Blank{});
    }
} 



#ifdef NDEBUG
TEST_CASE( "Speed test", "[sheet]" ) {
    
    constexpr SizeType Width = 1000;
    constexpr SizeType Height = 10000;
    constexpr double Count = double(Width) * double(Height);

    Sheet s;
    {
        double beg = clock();
        s.setValueCell(PT("A1"), 0.);
        s.setFormulaCell(PT("B1"), "A1 + 1");
        s.copyCell(PT("B1"), Spreader::Rect{.origin = {1, 0}, .size = {Width - 1, Height}});
        s.setFormulaCell(PT("A2"), s.indexToColumn(Width - 1) + SPRS("1 + 1"));
        s.copyCell(PT("A2"), Spreader::Rect{.origin = {0, 1}, .size = {1, Height - 1}});
        double end = clock();
        printf("time: %lf\n", (end - beg) / CLOCKS_PER_SEC);
        CHECK(s.getValue({Width - 1, Height - 1}) == Count - 1);
        
    }
    {
        double beg = clock();
        s.setValueCell(PT("A1"), 1.);
        double end = clock();
        printf("time: %lf\n", (end - beg) / CLOCKS_PER_SEC);
        CHECK(s.getValue({Width - 1, Height - 1}) == Count);
    }
}

TEST_CASE( "Array speed test", "[sheet]" ) {
    
    constexpr SizeType Width = 1000;
    constexpr SizeType Height = 10000;
    //constexpr double Count = double(Width) * double(Height);

    Sheet s;
    {
        double beg = clock();
        s.setFormulaCell(PT("A1"), s.indexToColumn(Width) + SPRS("1:") + s.indexToColumn(2 * Width - 1) + s.indexToRow(Height - 1) + SPRS(" + 1"));
        double end = clock();
        printf("time: %lf\n", (end - beg) / CLOCKS_PER_SEC);
        CHECK(s.getValue({Width - 1, Height - 1}) == 1);
    }
    {
        double beg = clock();
        s.recalculate();
        double end = clock();
        printf("time: %lf\n", (end - beg) / CLOCKS_PER_SEC);
        CHECK(s.getValue({Width - 1, Height - 1}) == 1);
    }
}

#endif
