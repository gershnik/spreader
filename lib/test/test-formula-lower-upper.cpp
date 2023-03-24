#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-lower-upper]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "LOWER()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "UPPER()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "LOWER(\"\")");
    CHECK(s.getValue(PT("A1")) == "");
    
    s.setFormulaCell(PT("A1"), "UPPER(\"\")");
    CHECK(s.getValue(PT("A1")) == "");

    s.setFormulaCell(PT("A1"), "LOWER(\"a\")");
    CHECK(s.getValue(PT("A1")) == "a");
    
    s.setFormulaCell(PT("A1"), "UPPER(\"a\")");
    CHECK(s.getValue(PT("A1")) == "A");

    s.setFormulaCell(PT("A1"), "LOWER(\"A\")");
    CHECK(s.getValue(PT("A1")) == "a");
    
    s.setFormulaCell(PT("A1"), "UPPER(\"A\")");
    CHECK(s.getValue(PT("A1")) == "A");
    
    s.setFormulaCell(PT("A1"), "LOWER(\"MAẞE\")");
    CHECK(s.getValue(PT("A1")) == "maße");

    s.setFormulaCell(PT("A1"), "UPPER(\"maße\")");
    CHECK(s.getValue(PT("A1")) == "MASSE"); // Excel has: MAẞE, google and numbers MASSE

    s.setFormulaCell(PT("A1"), "LOWER(\"ΒΟΥΣ\")");
    CHECK(s.getValue(PT("A1")) == "βους");

    s.setFormulaCell(PT("A1"), "UPPER(\"βους\")");
    CHECK(s.getValue(PT("A1")) == "ΒΟΥΣ");
}
