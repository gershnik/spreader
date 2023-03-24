#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct input", "[formula][formula-days]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "DAYS()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DAYS(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DAYS(1,2, 3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DAYS(DATE(2014, 7, 7), DATE(2023, 1, 28))");
    CHECK(s.getValue(PT("A1")) == -3127);
    
    s.setFormulaCell(PT("A1"), "DAYS(DATE(2023, 1, 28), DATE(2004, 11, 12))");
    CHECK(s.getValue(PT("A1")) == 6651);
    
    s.setFormulaCell(PT("A1"), "DAYS(DATE(2021, 1, 29), DATE(2021, 3, 28))");
    CHECK(s.getValue(PT("A1")) == -58);
}

