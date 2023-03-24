#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Errors", "[formula][formula-weekday]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "WEEKDAY()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "WEEKDAY(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "WEEKDAY(1,4)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "WEEKDAY(1,18)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "WEEKDAY(1,0)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
}

TEST_CASE( "Exhaustive", "[formula][formula-weekday]" ) {

    Sheet s;

    s.setFormulaCell(PT("A2"), "DATE(2023, 1, 29)");
    s.setFormulaCell(PT("A3"), "DATE(2023, 1, 30)");
    s.setFormulaCell(PT("A4"), "DATE(2023, 1, 31)");
    s.setFormulaCell(PT("A5"), "DATE(2023, 2, 1)");
    s.setFormulaCell(PT("A6"), "DATE(2023, 2, 2)");
    s.setFormulaCell(PT("A7"), "DATE(2023, 2, 3)");
    s.setFormulaCell(PT("A8"), "DATE(2023, 2, 4)");

    s.setValueCell(PT("B1"), 1);
    s.setValueCell(PT("C1"), 2);
    s.setValueCell(PT("D1"), 3);
    s.setValueCell(PT("E1"), 11);
    s.setValueCell(PT("F1"), 12);
    s.setValueCell(PT("G1"), 13);
    s.setValueCell(PT("H1"), 14);
    s.setValueCell(PT("I1"), 15);
    s.setValueCell(PT("J1"), 16);
    s.setValueCell(PT("K1"), 17);

    s.setFormulaCell(PT("B2"), "WEEKDAY($A2, B$1)");
    s.copyCell(PT("B2"), AREA("B2:K8"));

    s.setFormulaCell(PT("L2"), "WEEKDAY($A2)");
    s.copyCell(PT("L2"), AREA("L2:L8"));

    Scalar res[7][101];

    for(SizeType y = 0; y < 7; ++y)
        for(SizeType x = 0; x < 11; ++x)
            res[y][x] = s.getValue({x + 1, y + 1});

    Scalar expected[7][11] = {
        {1, 7, 6, 7, 6, 5, 4, 3, 2, 1, 1},
        {2, 1, 0, 1, 7, 6, 5, 4, 3, 2, 2},
        {3, 2, 1, 2, 1, 7, 6, 5, 4, 3, 3},
        {4, 3, 2, 3, 2, 1, 7, 6, 5, 4, 4},
        {5, 4, 3, 4, 3, 2, 1, 7, 6, 5, 5},
        {6, 5, 4, 5, 4, 3, 2, 1, 7, 6, 6},
        {7, 6, 5, 6, 5, 4, 3, 2, 1, 7, 7},
    };
    
    for(SizeType y = 0; y < 7; ++y) {
        for(SizeType x = 0; x < 11; ++x) {
            INFO("x: " << x << ", y: " << y);
            CHECK(res[y][x] == expected[y][x]);
        }
    }


}
