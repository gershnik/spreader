#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-row-column]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "ROW(1, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "COLUMN(1, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A2"), "ROW()");
    CHECK(s.getValue(PT("A2")) == 2);

    s.setFormulaCell(PT("B1"), "COLUMN()");
    CHECK(s.getValue(PT("B1")) == 2);

    s.setFormulaCell(PT("A1"), "ROW(C7)");
    CHECK(s.getValue(PT("A1")) == 7);

    s.setFormulaCell(PT("A1"), "COLUMN(C7)");
    CHECK(s.getValue(PT("A1")) == 3);

}

TEST_CASE( "Array arguments", "[formula][formula-row-column]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "ROW(A3:F5)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getValue(PT("A2")) == 4);
    CHECK(s.getValue(PT("A3")) == 5);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 3});

    s.setFormulaCell(PT("A1"), "COLUMN(D7:F12)");
    CHECK(s.getValue(PT("A1")) == 4);
    CHECK(s.getValue(PT("B1")) == 5);
    CHECK(s.getValue(PT("C1")) == 6);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});
}
