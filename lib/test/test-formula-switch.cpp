#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Invalid arguments", "[formula][formula-switch]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "SWITCH()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "SWITCH(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "SWITCH(1, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
}

TEST_CASE( "Scalar arguments", "[formula][formula-switch]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "SWITCH(\"a\", \"a\", 5)");
    CHECK(s.getValue(PT("A1")) == 5);

    s.setFormulaCell(PT("A1"), "SWITCH(\"a\", \"a\", 5, \"b\", 6)");
    CHECK(s.getValue(PT("A1")) == 5);
    
    s.setFormulaCell(PT("A1"), "SWITCH(\"7\", \"a\", 5, 7, 6)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    
    s.setFormulaCell(PT("A1"), "SWITCH(7, \"a\", 5, 7, 6)");
    CHECK(s.getValue(PT("A1")) == 6);

    s.setFormulaCell(PT("A1"), "SWITCH(4, 1, 11, 2, 12, 3, 13, 4, 14, 5, 15, 6, 16, 7, 17, 8, 18)");
    CHECK(s.getValue(PT("A1")) == 14);
    
    s.setFormulaCell(PT("A1"), "SWITCH(\"a\", #REF!, 5, \"b\", 6)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);

    s.setFormulaCell(PT("A1"), "SWITCH(#N/A, 5, 6)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    
    s.setFormulaCell(PT("A1"), "SWITCH(7, 6, #REF!, 7, 2)");
    CHECK(s.getValue(PT("A1")) == 2);
}

TEST_CASE( "Array arguments", "[formula][formula-switch]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "SWITCH({1;2;3}, {1;2;3}, 5)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getValue(PT("A2")) == 5);
    CHECK(s.getValue(PT("A3")) == 5);
    
    s.setFormulaCell(PT("A1"), "SWITCH({1,2,3}, {1;2;3}, 5)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getValue(PT("A2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A3")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B2")) == 5);
    CHECK(s.getValue(PT("B3")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("C1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("C2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("C3")) == 5);
    
    s.setFormulaCell(PT("A1"), "SWITCH({1;2;3},{1;2;3},{1,2,3,4})");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("B2")) == 2);
    CHECK(s.getValue(PT("B3")) == 2);
    CHECK(s.getValue(PT("C1")) == 3);
    CHECK(s.getValue(PT("C2")) == 3);
    CHECK(s.getValue(PT("C3")) == 3);
    CHECK(s.getValue(PT("D1")) == 4);
    CHECK(s.getValue(PT("D2")) == 4);
    CHECK(s.getValue(PT("D3")) == 4);
}
