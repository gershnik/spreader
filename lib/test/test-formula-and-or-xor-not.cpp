#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "AND", "[formula][formula-and-or-xor]" ) {
 
    Sheet s;

    s.setFormulaCell(PT("A1"), "AND()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "AND(1)");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "AND(1, \"a\")");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "AND(0, \"a\")");
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setFormulaCell(PT("A1"), "AND(\"a\", \"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "AND(true, false, true)");
    CHECK(s.getValue(PT("A1")) == false);

}

TEST_CASE( "OR", "[formula][formula-and-or-xor]" ) {
 
    Sheet s;

    s.setFormulaCell(PT("A1"), "OR()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "OR(1)");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "OR(1, \"a\")");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "OR(0, \"a\")");
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setFormulaCell(PT("A1"), "OR(\"a\", \"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "OR(true, false, true)");
    CHECK(s.getValue(PT("A1")) == true);

}

TEST_CASE( "XOR", "[formula][formula-and-or-xor]" ) {
 
    Sheet s;

    s.setFormulaCell(PT("A1"), "XOR()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "XOR(1)");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "XOR(1, \"a\")");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "XOR(0, \"a\")");
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setFormulaCell(PT("A1"), "XOR(\"a\", \"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "XOR(true, false, true)");
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setFormulaCell(PT("A1"), "XOR(true, true, true)");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "XOR(true, true, true, true)");
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setFormulaCell(PT("A1"), "XOR(false, false, false)");
    CHECK(s.getValue(PT("A1")) == false);

}

TEST_CASE( "NOT", "[formula][formula-and-or-xor]" ) {
 
    Sheet s;

    s.setFormulaCell(PT("A1"), "NOT()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "NOT(1)");
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setFormulaCell(PT("A1"), "NOT(1, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "NOT(0)");
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setFormulaCell(PT("A1"), "NOT(true)");
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setFormulaCell(PT("A1"), "NOT(false)");
    CHECK(s.getValue(PT("A1")) == true);
}
