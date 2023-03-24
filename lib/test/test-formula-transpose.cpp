#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid arguments", "[formula][formula-transpose]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "TRANSPOSE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "TRANSPOSE(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

}

TEST_CASE( "Basic arguments", "[formula][formula-transpose]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "TRANSPOSE(true)");
    CHECK(s.getValue(PT("A1")) == true);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setValueCell(PT("M1"), "Hello");
    s.setFormulaCell(PT("A1"), "TRANSPOSE(M1)");
    CHECK(s.getValue(PT("A1")) == "Hello");
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});


    s.setFormulaCell(PT("A1"), "TRANSPOSE({1,2,3})");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 3});

    s.setFormulaCell(PT("A1"), "TRANSPOSE({1;2;3})");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("C1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});


    s.setFormulaCell(PT("A1"), "TRANSPOSE({1,2,3;4,5,6})");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 4);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("B2")) == 5);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getValue(PT("B3")) == 6);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 3});


    s.setFormulaCell(PT("M1"), "{1,2,3;4,5,6}");
    s.setFormulaCell(PT("A1"), "TRANSPOSE(M1:O2)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 4);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("B2")) == 5);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getValue(PT("B3")) == 6);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 3});

    s.setFormulaCell(PT("E1"), "M1:O2");
    s.setFormulaCell(PT("A1"), "TRANSPOSE(E1:G2)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 4);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("B2")) == 5);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getValue(PT("B3")) == 6);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 3});

    s.setFormulaCell(PT("A1"), "TRANSPOSE({1,2,3;4,5,6}) + {10,10,10}");
    CHECK(s.getValue(PT("A1")) == 11);
    CHECK(s.getValue(PT("B1")) == 14);
    CHECK(s.getValue(PT("C1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A2")) == 12);
    CHECK(s.getValue(PT("B2")) == 15);
    CHECK(s.getValue(PT("C2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A3")) == 13);
    CHECK(s.getValue(PT("B3")) == 16);
    CHECK(s.getValue(PT("C3")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 3});
}


