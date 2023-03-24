#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-left-right]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "LEFT()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "RIGHT()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "LEFT(\"\")");
    CHECK(s.getValue(PT("A1")) == "");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"\")");
    CHECK(s.getValue(PT("A1")) == "");

    s.setFormulaCell(PT("A1"), "LEFT(\"a\")");
    CHECK(s.getValue(PT("A1")) == "a");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"a\")");
    CHECK(s.getValue(PT("A1")) == "a");

    s.setFormulaCell(PT("A1"), "LEFT(\"ab\")");
    CHECK(s.getValue(PT("A1")) == "a");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"ab\")");
    CHECK(s.getValue(PT("A1")) == "b");
    
    s.setFormulaCell(PT("A1"), "LEFT(\"水b\")");
    CHECK(s.getValue(PT("A1")) == "水");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"b水\")");
    CHECK(s.getValue(PT("A1")) == "水");
    
    s.setFormulaCell(PT("A1"), "LEFT(\"𐀀b\")");
    CHECK(s.getValue(PT("A1")) == "𐀀");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"b𐀀\")");
    CHECK(s.getValue(PT("A1")) == "𐀀");
    
    s.setFormulaCell(PT("A1"), "LEFT(\"a\", 0)");
    CHECK(s.getValue(PT("A1")) == "");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"a\", 0)");
    CHECK(s.getValue(PT("A1")) == "");
    
    s.setFormulaCell(PT("A1"), "LEFT(\"a\", 3)");
    CHECK(s.getValue(PT("A1")) == "a");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"a\", 3)");
    CHECK(s.getValue(PT("A1")) == "a");
    
    s.setFormulaCell(PT("A1"), "LEFT(\"水b\", 2)");
    CHECK(s.getValue(PT("A1")) == "水b");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"b水\", 2)");
    CHECK(s.getValue(PT("A1")) == "b水");
    
    s.setFormulaCell(PT("A1"), "LEFT(\"𐀀b\", 2)");
    CHECK(s.getValue(PT("A1")) == "𐀀b");
    
    s.setFormulaCell(PT("A1"), "RIGHT(\"b𐀀\", 2)");
    CHECK(s.getValue(PT("A1")) == "b𐀀");
}
