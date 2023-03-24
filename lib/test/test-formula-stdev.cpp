#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-stdev]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "STDEV()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "STDEV.S()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "STDEVA()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "STDEVP()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "STDEV.P()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "STDEVPA()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "STDEV(1)");
    CHECK(s.getValue(PT("A1")) == Error::DivisionByZero);
    
    s.setFormulaCell(PT("A1"), "STDEV.S(1)");
    CHECK(s.getValue(PT("A1")) == Error::DivisionByZero);
    
    s.setFormulaCell(PT("A1"), "STDEVP(1)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "stdev.p(1)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "STDEVA(1)");
    CHECK(s.getValue(PT("A1")) == Error::DivisionByZero);
    
    s.setFormulaCell(PT("A1"), "STDEVPA(1)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "STDEV(1, 2)");
    CHECK(s.getValue(PT("A1")) == 0.7071067811865476);
    
    s.setFormulaCell(PT("A1"), "STDEV.S(1, 2)");
    CHECK(s.getValue(PT("A1")) == 0.7071067811865476);

    s.setFormulaCell(PT("A1"), "STDEVA(1, 2)");
    CHECK(s.getValue(PT("A1")) == 0.7071067811865476);
    
    s.setFormulaCell(PT("A1"), "STDEVP(1, 2)");
    CHECK(s.getValue(PT("A1")) == 0.5);
    
    s.setFormulaCell(PT("A1"), "STDEV.P(1, 2)");
    CHECK(s.getValue(PT("A1")) == 0.5);
    
    s.setFormulaCell(PT("A1"), "STDEVPA(1, 2)");
    CHECK(s.getValue(PT("A1")) == 0.5);

}

TEST_CASE( "Edge cases", "[formula][formula-stdev]" ) {

    Sheet s;
    
    s.setValueCell(PT("A1"), 9e307);
    s.setValueCell(PT("A2"), 9e307);
    s.setValueCell(PT("B1"), 9e307);
    s.setValueCell(PT("B2"), 9e307);
    
    s.setFormulaCell(PT("A3"), "STDEV(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 0);
    
    s.setFormulaCell(PT("A3"), "STDEV.S(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 0);
    
    s.setFormulaCell(PT("A3"), "STDEVA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 0);
    
    s.setFormulaCell(PT("A3"), "STDEVP(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 0);
    
    s.setFormulaCell(PT("A3"), "STDEV.P(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 0);
    
    s.setFormulaCell(PT("A3"), "STDEVPA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 0);

}
