#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-substitute]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "SUBSTITUTE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"a\", \"b\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"a\", \"b\", \"c\", 1, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"a\", \"\", \"b\")");
    CHECK(s.getValue(PT("A1")) == "a");

    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"a\", \"a\", \"b\")");
    CHECK(s.getValue(PT("A1")) == "b");
    
    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"aa\", \"a\", \"b\")");
    CHECK(s.getValue(PT("A1")) == "bb");
    
    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"aa\", \"a\", \"b\", 1)");
    CHECK(s.getValue(PT("A1")) == "ba");
    
    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"aa\", \"a\", \"b\", 2)");
    CHECK(s.getValue(PT("A1")) == "ab");
    
    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"aa\", \"a\", \"b\", 3)");
    CHECK(s.getValue(PT("A1")) == "aa");
    
    s.setFormulaCell(PT("A1"), "SUBSTITUTE(\"aa\", \"a\", \"b\", 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
}
