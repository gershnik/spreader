#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-count]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "COUNT()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "COUNTA()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "COUNT(1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "COUNTA(1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "COUNT(5, 6)");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "COUNTA(5, 6)");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "COUNT(1, 2, \"a\")");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "COUNTA(1, 2, \"a\")");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "COUNT(1, 2, \"3\")");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "COUNTA(1, 2, \"3\")");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "COUNT(1, 2, true)");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "COUNTA(1, 2, true)");
    CHECK(s.getValue(PT("A1")) == 3);
    
}

TEST_CASE( "Arrays", "[formula][formula-count]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "COUNT({100, 10, true, \"a\", \"3\"})");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "COUNTA({100, 10, true, \"a\", \"3\"})");
    CHECK(s.getValue(PT("A1")) == 5);
    
    s.setFormulaCell(PT("A1"), "COUNT({100, 10, true, \"a\", \"3\", #REF!})");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "COUNTA({100, 10, true, \"a\", \"3\", #REF!})");
    CHECK(s.getValue(PT("A1")) == 6);
}

TEST_CASE( "Rectangles", "[formula][formula-count]" ) {

    Sheet s;

    s.setValueCell(PT("A1"), 1);
    s.setValueCell(PT("A2"), 10);
    s.setValueCell(PT("B1"), 100);
    s.setValueCell(PT("B2"), SPRS("a"));
    
    s.setFormulaCell(PT("A3"), "COUNT(A1)");
    CHECK(s.getValue(PT("A3")) == 1);
    
    s.setFormulaCell(PT("A3"), "COUNTA(A1)");
    CHECK(s.getValue(PT("A3")) == 1);

    s.setFormulaCell(PT("A3"), "COUNT(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 3);
    
    s.setFormulaCell(PT("A3"), "COUNTA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 4);

    s.setFormulaCell(PT("A3"), "COUNT(A1:B2, 5)");
    CHECK(s.getValue(PT("A3")) == 4);
    
    s.setFormulaCell(PT("A3"), "COUNTA(A1:B2, 5)");
    CHECK(s.getValue(PT("A3")) == 5);
    
    s.setFormulaCell(PT("A3"), "COUNT(A1:B2, true)");
    CHECK(s.getValue(PT("A3")) == 4);
    
    s.setFormulaCell(PT("A3"), "COUNTA(A1:B2, true)");
    CHECK(s.getValue(PT("A3")) == 5);
    
    s.setFormulaCell(PT("A3"), "COUNT(A1:B2, #REF!)");
    CHECK(s.getValue(PT("A3")) == 3);
    
    s.setFormulaCell(PT("A3"), "COUNTA(A1:B2, #REF!)");
    CHECK(s.getValue(PT("A3")) == 5);
    
    s.setValueCell(PT("B2"), Error::InvalidName);
    s.setFormulaCell(PT("A3"), "COUNT(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 3);
    
    s.setFormulaCell(PT("A3"), "COUNTA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 4);
    
    s.setValueCell(PT("B2"), true);
    s.setFormulaCell(PT("A3"), "COUNT(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 3);
    
    s.setFormulaCell(PT("A3"), "COUNTA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 4);
    
    s.clearCell(PT("B2"));
    CHECK(s.getValue(PT("A3")) == 3);
}


