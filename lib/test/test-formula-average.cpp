#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-average]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "AVERAGE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA(1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(1,)");
    CHECK(s.getValue(PT("A1")) == 0.5);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(,1)");
    CHECK(s.getValue(PT("A1")) == 0.5);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(,3,)");
    CHECK(s.getValue(PT("A1")) == 1.);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(,,4,)");
    CHECK(s.getValue(PT("A1")) == 1.);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(\"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA(\"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(\"3\")");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA(\"3\")");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(5, 6)");
    CHECK(s.getValue(PT("A1")) == 5.5);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA(5, 6)");
    CHECK(s.getValue(PT("A1")) == 5.5);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(1, 2, \"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA(1, 2, \"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "AVERAGE(1, 2, true)");
    CHECK(s.getValue(PT("A1")) == 4./3);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA(1, 2, true)");
    CHECK(s.getValue(PT("A1")) == 4./3);
    
}

TEST_CASE( "Arrays", "[formula][formula-average]" ) {
    
    Sheet s;
    
    
    s.setFormulaCell(PT("A1"), "AVERAGE({100, 10, true, \"a\", \"3\"})");
    CHECK(s.getValue(PT("A1")) == 55);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA({100, 10, true, \"a\", \"3\"})");
    CHECK(s.getValue(PT("A1")) == 22.2);
    //Excel gives 27.5 which is wrong by their own docs:
    // https://support.microsoft.com/en-us/office/averagea-function-f5f84098-d453-4f4c-bbba-3d2c66356091
    //Google sheets gives 22.2
    
    s.setFormulaCell(PT("A1"), "AVERAGE({100, 10, true, \"a\", \"3\", #REF!})");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    
    s.setFormulaCell(PT("A1"), "AVERAGEA({100, 10, true, \"a\", \"3\", #REF!})");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
}

TEST_CASE( "Rectangles", "[formula][formula-average]" ) {

    Sheet s;

    s.setValueCell(PT("A1"), 1);
    s.setValueCell(PT("A2"), 10);
    s.setValueCell(PT("B1"), 100);
    s.setValueCell(PT("B2"), SPRS("a"));
    
    s.setFormulaCell(PT("A3"), "AVERAGE(A1)");
    CHECK(s.getValue(PT("A3")) == 1);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(A1)");
    CHECK(s.getValue(PT("A3")) == 1);

    s.setFormulaCell(PT("A3"), "AVERAGE(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 111./3);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 111./4);

    s.setFormulaCell(PT("A3"), "AVERAGE(A1:B2, 5)");
    CHECK(s.getValue(PT("A3")) == 116./4);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(A1:B2, 5)");
    CHECK(s.getValue(PT("A3")) == 116./5);
    
    s.setFormulaCell(PT("A3"), "AVERAGE(A1:B2, true)");
    CHECK(s.getValue(PT("A3")) == 112./4);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(A1:B2, true)");
    CHECK(s.getValue(PT("A3")) == 112./5);
    
    s.setFormulaCell(PT("A3"), "AVERAGE(A1:B2, #REF!)");
    CHECK(s.getValue(PT("A3")) == Error::InvalidReference);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(A1:B2, #REF!)");
    CHECK(s.getValue(PT("A3")) == Error::InvalidReference);
    
    s.setValueCell(PT("B2"), Error::InvalidName);
    s.setFormulaCell(PT("A3"), "AVERAGE(A1:B2)");
    CHECK(s.getValue(PT("A3")) == Error::InvalidName);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == Error::InvalidName);
    
    s.setValueCell(PT("B2"), true);
    s.setFormulaCell(PT("A3"), "AVERAGE(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 111./3);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 112./4);
    
    s.clearCell(PT("B2"));
    CHECK(s.getValue(PT("A3")) == 111./3);
}


TEST_CASE( "Edge cases", "[formula][formula-average]" ) {

    Sheet s;
    
    s.setValueCell(PT("A1"), 9e307);
    s.setValueCell(PT("A2"), 9e307);
    s.setValueCell(PT("B1"), 9e307);
    s.setValueCell(PT("B2"), 9e307);
    
    s.setFormulaCell(PT("A3"), "AVERAGE(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 9e307);

    s.setFormulaCell(PT("A3"), "AVERAGEA(A1:B2)");
    CHECK(s.getValue(PT("A3")) == 9e307);
    
    s.setFormulaCell(PT("A3"), "AVERAGEA(1.333, 1.225, -1.333, -1.225)");
    CHECK(s.getValue(PT("A3")) == 0);
}
