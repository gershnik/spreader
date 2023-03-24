#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-address]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "ADDRESS()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ADDRESS(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ADDRESS(-1, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "ADDRESS(1, -1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "ADDRESS(1, 1, 5)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "ADDRESS(1,1)");
    CHECK(s.getValue(PT("A1")) == SPRS("$A$1"));

}