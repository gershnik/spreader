#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-date]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A2"), "DAY(A1)");
    s.setFormulaCell(PT("A3"), "MONTH(A1)");
    s.setFormulaCell(PT("A4"), "YEAR(A1)");
    
    
    s.setFormulaCell(PT("A1"), "DATE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A2")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A4")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DATE(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A2")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A4")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "DATE(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A2")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A4")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "DATE(1,2,3,4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A2")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A4")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "DATE(-1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    CHECK(s.getValue(PT("A2")) == Error::NotANumber);
    CHECK(s.getValue(PT("A3")) == Error::NotANumber);
    CHECK(s.getValue(PT("A4")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "DATE(10000,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    CHECK(s.getValue(PT("A2")) == Error::NotANumber);
    CHECK(s.getValue(PT("A3")) == Error::NotANumber);
    CHECK(s.getValue(PT("A4")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "DATE(0,0,0)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    CHECK(s.getValue(PT("A2")) == Error::NotANumber);
    CHECK(s.getValue(PT("A3")) == Error::NotANumber);
    CHECK(s.getValue(PT("A4")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "DATE(1,0,0)");
    CHECK_DATE(s.getValue(PT("A1")), 1900, 11, 30);
    CHECK(s.getValue(PT("A2")) == 30);
    CHECK(s.getValue(PT("A3")) == 11);
    CHECK(s.getValue(PT("A4")) == 1900);
    
    s.setFormulaCell(PT("A1"), "DATE(1901,0,0)");
    CHECK_DATE(s.getValue(PT("A1")), 1900, 11, 30);
    CHECK(s.getValue(PT("A2")) == 30);
    CHECK(s.getValue(PT("A3")) == 11);
    CHECK(s.getValue(PT("A4")) == 1900);
    
    s.setFormulaCell(PT("A1"), "DATE(-2620, 32766, 1)");
    CHECK_DATE(s.getValue(PT("A1")), 2010, 6, 1);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 6);
    CHECK(s.getValue(PT("A4")) == 2010);
    
    s.setFormulaCell(PT("A1"), "DATE(2023, 1, 28)");
    CHECK_DATE(s.getValue(PT("A1")), 2023, 1, 28);
    CHECK(s.getValue(PT("A2")) == 28);
    CHECK(s.getValue(PT("A3")) == 1);
    CHECK(s.getValue(PT("A4")) == 2023);
}
