#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-sign]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "SIGN()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "SIGN(2)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "SIGN(\"2\")");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "SIGN(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "SIGN(-3)");
    CHECK(s.getValue(PT("A1")) == -1);

    s.setFormulaCell(PT("A1"), "SIGN(true)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "SIGN(false)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "SIGN(0)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "SIGN(\"ab\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
}
