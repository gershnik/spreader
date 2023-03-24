#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid arguments", "[formula][formula-choose]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "CHOOSE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "CHOOSE(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "CHOOSE(0, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "CHOOSE(2, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "CHOOSE(500, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
}

TEST_CASE( "Scalar Arguments", "[formula][formula-choose]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "CHOOSE(1, 5)");
    CHECK(s.getValue(PT("A1")) == 5);

    s.setFormulaCell(PT("A1"), "CHOOSE(1, 5, 6)");
    CHECK(s.getValue(PT("A1")) == 5);
    
    s.setFormulaCell(PT("A1"), "CHOOSE(2, 5, 6)");
    CHECK(s.getValue(PT("A1")) == 6);

    s.setFormulaCell(PT("A1"), "CHOOSE(4, 1, 2, 3, 4, 5, 6, 7, 8)");
    CHECK(s.getValue(PT("A1")) == 4);
    
    s.setFormulaCell(PT("A1"), "CHOOSE(\"2\", 5, 6)");
    CHECK(s.getValue(PT("A1")) == 6);

    s.setFormulaCell(PT("A1"), "CHOOSE(#N/A, 5, 6)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
}

TEST_CASE( "Array Arguments", "[formula][formula-choose]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "CHOOSE({1;2;3}, 5, 6, 7)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getValue(PT("A2")) == 6);
    CHECK(s.getValue(PT("A3")) == 7);
    
    s.setFormulaCell(PT("A1"), "CHOOSE({1;2;3}, {5,6,7}, 8, 9)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getValue(PT("B1")) == 6);
    CHECK(s.getValue(PT("C1")) == 7);
    CHECK(s.getValue(PT("A2")) == 8);
    CHECK(s.getValue(PT("B2")) == 8);
    CHECK(s.getValue(PT("C2")) == 8);
    CHECK(s.getValue(PT("A3")) == 9);
    CHECK(s.getValue(PT("B3")) == 9);
    CHECK(s.getValue(PT("C3")) == 9);
    
    s.setFormulaCell(PT("A1"), "CHOOSE({1;2;3}, {5,6,7}, {8;18;19;20}, 9)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getValue(PT("B1")) == 6);
    CHECK(s.getValue(PT("C1")) == 7);
    CHECK(s.getValue(PT("A2")) == 18);
    CHECK(s.getValue(PT("B2")) == 18);
    CHECK(s.getValue(PT("C2")) == 18);
    CHECK(s.getValue(PT("A3")) == 9);
    CHECK(s.getValue(PT("B3")) == 9);
    CHECK(s.getValue(PT("C3")) == 9);
    CHECK(s.getValue(PT("A4")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B4")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("C4")) == Error::InvalidArgs);
}
