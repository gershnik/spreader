#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-len]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "LEN()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "LEN(\"a\")");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "LEN(\"  Hello World!  \")");
    CHECK(s.getValue(PT("A1")) == 16);

    s.setFormulaCell(PT("A1"), "LEN(33)");
    CHECK(s.getValue(PT("A1")) == 2);

    s.setFormulaCell(PT("A1"), "LEN(\"a水𐀀𝄞\")");
    CHECK(s.getValue(PT("A1")) == 6);

    s.setFormulaCell(PT("A1"), "LEN(\"a\", \"b\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
}