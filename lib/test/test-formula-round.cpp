#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-round]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "ROUND()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ROUNDUP()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ROUNDDOWN()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ROUND(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ROUNDUP(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ROUNDDOWN(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ROUND(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ROUNDUP(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ROUNDDOWN(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ROUND(1.5, 0)");
    CHECK(s.getValue(PT("A1")) == 2);

    s.setFormulaCell(PT("A1"), "ROUNDUP(1.3, 0)");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "ROUNDDOWN(1.7, 0)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "ROUND(1.53467, 3)");
    CHECK(s.getValue(PT("A1")) == 1.535);
    
    s.setFormulaCell(PT("A1"), "ROUNDUP(1.53437, 3)");
    CHECK(s.getValue(PT("A1")) == 1.535);
    
    s.setFormulaCell(PT("A1"), "ROUNDDOWN(1.53467, 3)");
    CHECK(s.getValue(PT("A1")) == 1.534);
    
    s.setFormulaCell(PT("A1"), "ROUND(256, -2)");
    CHECK(s.getValue(PT("A1")) == 300);
    
    s.setFormulaCell(PT("A1"), "ROUNDUP(223, -2)");
    CHECK(s.getValue(PT("A1")) == 300);
    
    s.setFormulaCell(PT("A1"), "ROUNDDOWN(256, -2)");
    CHECK(s.getValue(PT("A1")) == 200);
}
