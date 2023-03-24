#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Direct arguments", "[formula][formula-trim]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "TRIM()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "TRIM(\"a\")");
    CHECK(s.getValue(PT("A1")) == SPRS("a"));

    s.setFormulaCell(PT("A1"), "TRIM(\"  Hello World!  \")");
    CHECK(s.getValue(PT("A1")) == SPRS("Hello World!"));

    s.setFormulaCell(PT("A1"), "TRIM(\"a\", \"b\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
}