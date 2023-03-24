#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct input", "[formula][formula-edate-eomonth]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "EDATE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "EOMONTH()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "EDATE(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "EOMONTH(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "EDATE(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "EOMONTH(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "EDATE(DATE(2023,1,15), 32769)");
    CHECK_DATE(s.getValue(PT("A1")), 4753, 10, 15);
    
    s.setFormulaCell(PT("A1"), "EOMONTH(DATE(2023,1,31), 32769)");
    CHECK_DATE(s.getValue(PT("A1")), 4753, 10, 31);

    s.setFormulaCell(PT("A1"), "EDATE(DATE(2023,1,31), 1000000)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "EOMONTH(DATE(2023,1,31), 1000000)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "EDATE(DATE(2023,1,31), 1)");
    CHECK_DATE(s.getValue(PT("A1")), 2023, 2, 28);
    
    s.setFormulaCell(PT("A1"), "EOMONTH(DATE(2023,1,31), 1)");
    CHECK_DATE(s.getValue(PT("A1")), 2023, 2, 28);

    s.setFormulaCell(PT("A1"), "EDATE(DATE(2023,1,31), -4)");
    CHECK_DATE(s.getValue(PT("A1")), 2022, 9, 30);
    
    s.setFormulaCell(PT("A1"), "EOMONTH(DATE(2023,1,31), -4)");
    CHECK_DATE(s.getValue(PT("A1")), 2022, 9, 30);
}
