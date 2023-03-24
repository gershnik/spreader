#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Normal cases", "[formula][formula-sumif]" ) {

    Sheet s;

    s.setValueCell(PT("A1"), 1);
    s.setValueCell(PT("A2"), 10);
    s.setValueCell(PT("B1"), 100);
    s.setValueCell(PT("B2"), 1000);
    
    s.setFormulaCell(PT("A3"), "SUMIF(A1, \">10\")");
    CHECK(s.getValue(PT("A3")) == 0);

    s.setFormulaCell(PT("A3"), "SUMIF(A1:B2, \">10\")");

    CHECK(s.getValue(PT("A3")) == 1100);

    s.setValueCell(PT("A11"), 1);
    s.setValueCell(PT("A12"), 15);
    s.setValueCell(PT("B11"), 15);
    s.setValueCell(PT("B12"), 15);

    s.setFormulaCell(PT("A3"), "SUMIF(A11:B12, \">10\", A1:B2)");

    CHECK(s.getValue(PT("A3")) == 1110);


    s.setValueCell(PT("E1"), 1);
    s.setValueCell(PT("E2"), 10);
    s.setValueCell(PT("E3"), 100);
    s.setValueCell(PT("E4"), 1000);


    s.setFormulaCell(PT("A3"), "SUMIF(A1:B2, \">=\" & E1:E4)");

    CHECK(s.getValue(PT("A3")) == 1111);
    CHECK(s.getValue(PT("A4")) == 1110);
    CHECK(s.getValue(PT("A5")) == 1100);
    CHECK(s.getValue(PT("A6")) == 1000);
}

TEST_CASE( "Bad argument types", "[formula][formula-sumif]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A3"), "SUMIF(7, \">10\")");
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A3"), "SUMIF(\"abc\", \">10\")");
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A3"), "SUMIF(A1:B2, \">10\", 7)");
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
}
