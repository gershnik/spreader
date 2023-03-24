#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-ceil-floor]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "CEIL()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "FLOOR()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "MROUND()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "CEIL(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "FLOOR(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "MROUND(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "CEIL(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "FLOOR(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "MROUND(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "CEIL(1.3, 1)");
    CHECK(s.getValue(PT("A1")) == 2);

    s.setFormulaCell(PT("A1"), "FLOOR(1.3, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "MROUND(1.3, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "MROUND(1.5, 1)");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "CEIL(1.3, 0)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "FLOOR(1.3, 0)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "MROUND(1.3, 0)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "CEIL(0, 1)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "FLOOR(0, 1)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "MROUND(0, 1)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "CEIL(0, -1)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "FLOOR(0, -1)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "MROUND(0, -1)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "CEIL(0, 0)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "FLOOR(0, 0)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "MROUND(0, 0)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "CEIL(1.3, 5)");
    CHECK(s.getValue(PT("A1")) == 5);

    s.setFormulaCell(PT("A1"), "FLOOR(2.3, 5)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "FLOOR(2.3, 2)");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "MROUND(2.3, 5)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "MROUND(2.5, 5)");
    CHECK(s.getValue(PT("A1")) == 5);
    
    s.setFormulaCell(PT("A1"), "CEIL(-1.3, 1)");
    CHECK(s.getValue(PT("A1")) == -1);

    s.setFormulaCell(PT("A1"), "FLOOR(-1.3, 1)");
    CHECK(s.getValue(PT("A1")) == -2);
    
    s.setFormulaCell(PT("A1"), "MROUND(-1.3, 1)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "CEIL(-1.3, -1)");
    CHECK(s.getValue(PT("A1")) == -2);

    s.setFormulaCell(PT("A1"), "FLOOR(-1.3, -1)");
    CHECK(s.getValue(PT("A1")) == -1);
    
    s.setFormulaCell(PT("A1"), "MROUND(-1.3, -1)");
    CHECK(s.getValue(PT("A1")) == -1);
    
    s.setFormulaCell(PT("A1"), "MROUND(-1.5, -1)");
    CHECK(s.getValue(PT("A1")) == -2);
    
    s.setFormulaCell(PT("A1"), "CEIL(1.3, -1)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "FLOOR(1.3, -1)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "MROUND(1.3, -1)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
}
