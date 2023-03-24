#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-sum]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "SUM()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "SUM(,)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "SUM(,,)");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "SUM(,1,)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "SUM(1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "SUM(5, 6)");
    CHECK(s.getValue(PT("A1")) == 11);
    
    s.setFormulaCell(PT("A1"), "SUM(1, 2, \"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "SUM(1, 2, true)");
    CHECK(s.getValue(PT("A1")) == 4);
    
}

TEST_CASE( "Arrays", "[formula][formula-sum]" ) {
    
    Sheet s;
    
    
    s.setFormulaCell(PT("A1"), "SUM({100, 10, true, \"a\", \"3\"})");
    CHECK(s.getValue(PT("A1")) == 110);
    
    s.setFormulaCell(PT("A1"), "SUM({100, 10, true, \"a\", \"3\", #REF!})");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
}

TEST_CASE( "Rectangles", "[formula][formula-sum]" ) {

    Sheet s;

    s.setValueCell(PT("A1"), 1);
    s.setValueCell(PT("A2"), 10);
    s.setValueCell(PT("B1"), 100);
    s.setValueCell(PT("B2"), SPRS("a"));
    
    s.setFormulaCell(PT("A3"), "SUM(A1)");
    CHECK(s.getValue(PT("A3")) == 1);

    s.setFormulaCell(PT("A3"), "SUM(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 111);

    s.setFormulaCell(PT("A3"), "SUM(A1:B2, 5)");
    CHECK(s.getValue(PT("A3")) == 116);
    
    s.setFormulaCell(PT("A3"), "SUM(A1:B2, true)");
    CHECK(s.getValue(PT("A3")) == 112);
    
    s.setFormulaCell(PT("A3"), "SUM(A1:B2, #REF!)");
    CHECK(s.getValue(PT("A3")) == Error::InvalidReference);
    
    s.setValueCell(PT("B2"), Error::InvalidName);
    s.setFormulaCell(PT("A3"), "SUM(A1:B2)");
    CHECK(s.getValue(PT("A3")) == Error::InvalidName);
    
    s.setValueCell(PT("B2"), true);
    CHECK(s.getValue(PT("A3")) == 111);
}

TEST_CASE( "Dependencies", "[formula][formula-sum]" ) {
    
    {
        Sheet s;
        
        s.setValueCell(PT("A1"), 1);
        s.setFormulaCell(PT("A2"), "B2 + 7");
        s.setFormulaCell(PT("A3"), "B3 + 9");
        s.setValueCell(PT("A4"), 2);
        
        s.setFormulaCell(PT("A5"), "SUM(A1, A2:A4)");
        CHECK(s.getValue(PT("A5")) == 19);
    }
    
    {
        Sheet s;
        
        s.setValueCell(PT("A1"), 1);
        s.setFormulaCell(PT("A2"), "B2 + 7");
        s.setFormulaCell(PT("A3"), "B3 + 9");
        s.setValueCell(PT("A4"), 2);
        
        s.setFormulaCell(PT("A5"), "SUM(A1, A2:A4 + 2)");
        CHECK(s.getValue(PT("A5")) == 25);
    }
    

}

TEST_CASE( "Edge cases", "[formula][formula-sum]" ) {

    Sheet s;
    
    s.setValueCell(PT("A1"), 9e307);
    s.setValueCell(PT("A2"), 9e307);
    s.setValueCell(PT("B1"), 9e307);
    s.setValueCell(PT("B2"), 9e307);
    s.setFormulaCell(PT("A3"), "SUM(A1:B2)");
    CHECK(s.getValue(PT("A3")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A3"), "SUM(1.333, 1.225, -1.333, -1.225)");
    CHECK(s.getValue(PT("A3")) == 0);

}
