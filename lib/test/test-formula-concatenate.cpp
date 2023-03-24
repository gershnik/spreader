#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-concatenate]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "CONCATENATE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "CONCAT()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "CONCATENATE(\"a\")");
    CHECK(s.getValue(PT("A1")) == SPRS("a"));
    
    s.setFormulaCell(PT("A1"), "CONCAT(\"a\")");
    CHECK(s.getValue(PT("A1")) == SPRS("a"));
    
    s.setFormulaCell(PT("A1"), "CONCATENATE(\"a\", \"b\")");
    CHECK(s.getValue(PT("A1")) == SPRS("ab"));
    
    s.setFormulaCell(PT("A1"), "CONCAT(\"a\", \"b\")");
    CHECK(s.getValue(PT("A1")) == SPRS("ab"));
    
    s.setFormulaCell(PT("A1"), "CONCATENATE(\"a\", \"b\", \"c\")");
    CHECK(s.getValue(PT("A1")) == SPRS("abc"));
    
    s.setFormulaCell(PT("A1"), "CONCAT(\"a\", \"b\", \"c\")");
    CHECK(s.getValue(PT("A1")) == SPRS("abc"));
    
    s.setFormulaCell(PT("A1"), "CONCATENATE(1, 2)");
    CHECK(s.getValue(PT("A1")) == SPRS("12"));
    
    s.setFormulaCell(PT("A1"), "CONCAT(1, 2)");
    CHECK(s.getValue(PT("A1")) == SPRS("12"));
    
    s.setFormulaCell(PT("A1"), "CONCATENATE(1, true)");
    CHECK(s.getValue(PT("A1")) == SPRS("1TRUE"));
    
    s.setFormulaCell(PT("A1"), "CONCAT(1, true)");
    CHECK(s.getValue(PT("A1")) == SPRS("1TRUE"));
    
    s.setFormulaCell(PT("A1"), "CONCATENATE(1, #NUM!)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "CONCAT(1, #NUM!)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "CONCATENATE(1, ,, \"a\")");
    CHECK(s.getValue(PT("A1")) == SPRS("1a"));
    
    s.setFormulaCell(PT("A1"), "CONCAT(1, ,, \"a\")");
    CHECK(s.getValue(PT("A1")) == SPRS("1a"));
}

TEST_CASE( "Arrays", "[formula][formula-concatenate]" ) {
 
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "CONCATENATE({1,2,3}, \"a\")");
    CHECK(s.getValue(PT("A1")) == SPRS("1a"));
    CHECK(s.getValue(PT("B1")) == SPRS("2a"));
    CHECK(s.getValue(PT("C1")) == SPRS("3a"));
    
    s.setFormulaCell(PT("A1"), "CONCAT({1,2,3}, \"a\")");
    CHECK(s.getValue(PT("A1")) == SPRS("123a"));
    
    s.setFormulaCell(PT("A1"), "CONCATENATE({1;2;3}, {\"a\", \"b\", \"c\"})");
    CHECK(s.getValue(PT("A1")) == SPRS("1a"));
    CHECK(s.getValue(PT("B1")) == SPRS("1b"));
    CHECK(s.getValue(PT("C1")) == SPRS("1c"));
    CHECK(s.getValue(PT("A2")) == SPRS("2a"));
    CHECK(s.getValue(PT("B2")) == SPRS("2b"));
    CHECK(s.getValue(PT("C2")) == SPRS("2c"));
    CHECK(s.getValue(PT("A3")) == SPRS("3a"));
    CHECK(s.getValue(PT("B3")) == SPRS("3b"));
    CHECK(s.getValue(PT("C3")) == SPRS("3c"));
    
    s.setFormulaCell(PT("A1"), "CONCAT({1;2;3}, {\"a\", \"b\", \"c\"})");
    CHECK(s.getValue(PT("A1")) == SPRS("123abc"));
    
}

TEST_CASE( "Rectangles", "[formula][formula-concatenate]" ) {
    
    Sheet s;
    
    s.setValueCell(PT("A1"), SPRS("a"));
    s.setValueCell(PT("B1"), SPRS("b"));
    s.setValueCell(PT("C1"), SPRS("c"));
    s.setValueCell(PT("A2"), SPRS("1"));
    s.setValueCell(PT("A3"), SPRS("2"));
    s.setValueCell(PT("A4"), SPRS("3"));
    s.setFormulaCell(PT("A5"), "CONCATENATE(A2:A4, A1:C1)");
    CHECK(s.getValue(PT("A5")) == SPRS("1a"));
    CHECK(s.getValue(PT("B5")) == SPRS("1b"));
    CHECK(s.getValue(PT("C5")) == SPRS("1c"));
    CHECK(s.getValue(PT("A6")) == SPRS("2a"));
    CHECK(s.getValue(PT("B6")) == SPRS("2b"));
    CHECK(s.getValue(PT("C6")) == SPRS("2c"));
    CHECK(s.getValue(PT("A7")) == SPRS("3a"));
    CHECK(s.getValue(PT("B7")) == SPRS("3b"));
    CHECK(s.getValue(PT("C7")) == SPRS("3c"));
    
    s.setFormulaCell(PT("A5"), "CONCAT(A2:A4, A1:C1)");
    CHECK(s.getValue(PT("A5")) == SPRS("123abc"));
    
}
