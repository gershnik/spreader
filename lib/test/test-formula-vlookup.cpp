#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid arguments", "[formula][formula-vlookup]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "VLOOKUP()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "VLOOKUP(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, {1, 2})");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, 1, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, {1, 2}, 3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
}

TEST_CASE( "Structure", "[formula][formula-vlookup]" ) {
    
    Sheet s;

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, {1,\"a\",\"q\";2,\"b\",\"m\"}, 2)");
    CHECK(s.getValue(PT("A1")) == "a");

    s.setFormulaCell(PT("A1"), "VLOOKUP({1; 2; 3}, {1,\"a\",\"q\";2,\"b\",\"m\"}, 2)");
    CHECK(s.getValue(PT("A1")) == "a");
    CHECK(s.getValue(PT("A2")) == "b");
    CHECK(s.getValue(PT("A3")) == "b");
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 3});

    s.setFormulaCell(PT("D1"), "{2,3}");
    s.setFormulaCell(PT("A1"), "VLOOKUP({1; 2; 3}, {1,\"a\",\"q\";2,\"b\",\"m\"}, D1:E1)");
    CHECK(s.getValue(PT("A1")) == "a");
    CHECK(s.getValue(PT("A2")) == "b");
    CHECK(s.getValue(PT("A3")) == "b");
    CHECK(s.getValue(PT("B1")) == "q");
    CHECK(s.getValue(PT("B2")) == "m");
    CHECK(s.getValue(PT("B3")) == "m");
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 3});

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, {1,\"a\",\"q\";2,\"b\",\"m\"}, D1:E1)");
    CHECK(s.getValue(PT("A1")) == "a");
    CHECK(s.getValue(PT("B1")) == "q");
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, {1,\"a\",\"q\";2,\"b\",\"m\"}, D1:E1, {1;2})");
    CHECK(s.getValue(PT("A1")) == "a");
    CHECK(s.getValue(PT("B1")) == "q");
    CHECK(s.getValue(PT("A2")) == "a");
    CHECK(s.getValue(PT("B2")) == "q");
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 2});

    s.setFormulaCell(PT("A1"), "VLOOKUP({1, 2}, 2, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});

    s.setFormulaCell(PT("A1"), "VLOOKUP({1;1}, {1,2}, D1:E1, \"e\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B1")) == Error::InvalidValue);
    CHECK(s.getValue(PT("B2")) == Error::InvalidValue);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 2});
    
    s.setFormulaCell(PT("D1"), "{1,\"a\",\"q\";2,\"b\",\"m\"}");
    s.setFormulaCell(PT("A1"), "VLOOKUP({1; 2; 3}, D1:F2, {2,3})");
    CHECK(s.getValue(PT("A1")) == "a");
    CHECK(s.getValue(PT("A2")) == "b");
    CHECK(s.getValue(PT("A3")) == "b");
    CHECK(s.getValue(PT("B1")) == "q");
    CHECK(s.getValue(PT("B2")) == "m");
    CHECK(s.getValue(PT("B3")) == "m");
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 3});
    
    s.setFormulaCell(PT("D1"), "{1,\"a\",\"q\";2,\"b\",\"m\"}");
    s.setFormulaCell(PT("A1"), "VLOOKUP({1; 2; 3}, D1:F2, {2,3}, {1, 1, 0})");
    CHECK(s.getValue(PT("A1")) == "a");
    CHECK(s.getValue(PT("A2")) == "b");
    CHECK(s.getValue(PT("A3")) == "b");
    CHECK(s.getValue(PT("B1")) == "q");
    CHECK(s.getValue(PT("B2")) == "m");
    CHECK(s.getValue(PT("B3")) == "m");
    CHECK(s.getValue(PT("C1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("C2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("C3")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 3});
}

TEST_CASE( "Exact Matching", "[formula][formula-vlookup]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "VLOOKUP(0, {1,2}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "VLOOKUP(3, {1,2}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, {1,2}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});


    //blank matches 0
    s.setFormulaCell(PT("A1"), "VLOOKUP(, {1,2;0,4}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == 4);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank does not match ""
    s.setFormulaCell(PT("A1"), "VLOOKUP(, {1,2;\"\",4}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //errors in data are ignored
    s.setFormulaCell(PT("A1"), "VLOOKUP(5, {#NAME?,2;2,4;0,7}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "VLOOKUP(2, {#NAME?,2;2,4;0,7}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == 4);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //boolean and numbers don't cross match
    s.setFormulaCell(PT("A1"), "VLOOKUP(true, {1,2;2,4;true,7}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == 7);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //strings and numbers don't cross match
    s.setFormulaCell(PT("A1"), "VLOOKUP(\"1\", {1,2;2,4;\"1\",7}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == 7);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //wildcards work
    s.setFormulaCell(PT("A1"), "VLOOKUP(\"1*\", {1,2;2,4;\"132\",7}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == 7);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    s.setFormulaCell(PT("A1"), "VLOOKUP(\"1?2\", {1,2;2,4;\"132\",7}, 2, FALSE)");
    CHECK(s.getValue(PT("A1")) == 7);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
}

TEST_CASE( "Inexact Matching", "[formula][formula-vlookup]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "VLOOKUP(0, {1,2}, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    s.setFormulaCell(PT("A1"), "VLOOKUP(3, {1,2}, 2, TRUE)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "VLOOKUP(1, {1,2}, 2)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank matches 0
    s.setFormulaCell(PT("A1"), "VLOOKUP(, {0,4;1,2}, 2)");
    CHECK(s.getValue(PT("A1")) == 4);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank does not match ""
    s.setFormulaCell(PT("A1"), "VLOOKUP(, {1,2;\"\",4}, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //errors in data are ignored
    s.setFormulaCell(PT("A1"), "VLOOKUP(5, {#NAME?,2;2,4;0,7}, 2)");
    CHECK(s.getValue(PT("A1")) == 7);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "VLOOKUP(2, {#NAME?,2;0,7;2,4}, 2)");
    CHECK(s.getValue(PT("A1")) == 4);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //boolean and numbers don't cross match
    s.setFormulaCell(PT("A1"), "VLOOKUP(true, {1,2;2,4;true,7}, 2)");
    CHECK(s.getValue(PT("A1")) == 7);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //strings and numbers don't cross match
    s.setFormulaCell(PT("A1"), "VLOOKUP(\"1\", {1,2;2,4;\"1\",7}, 2)");
    CHECK(s.getValue(PT("A1")) == 7);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
}

