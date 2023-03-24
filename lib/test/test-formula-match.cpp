#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid arguments", "[formula][formula-match]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "MATCH()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "MATCH(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MATCH(1, {1, 2}, 1, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MATCH(1, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);

    s.setFormulaCell(PT("A1"), "MATCH(1,{1,2,3;1,2,3},1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);

    s.setFormulaCell(PT("A1"), "MATCH(1,{1,2,3},\"e\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
}

TEST_CASE( "Structure", "[formula][formula-match]" ) {
    
    Sheet s;

    s.setFormulaCell(PT("A1"), "match(5, {5,6,7}, 2)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "match({5; 6; 7}, {5,6,7}, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 3});

    s.setFormulaCell(PT("A1"), "match({5; 6; 7}, {5,6,7;5,6,7}, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A3")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 3});

    s.setFormulaCell(PT("D1"), "{1,2}");
    s.setFormulaCell(PT("A1"), "match({5; 8; 8}, {5,6,7}, D1:E1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 3);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getValue(PT("B2")) == 3);
    CHECK(s.getValue(PT("B3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 3});

    s.setFormulaCell(PT("A1"), "match(5, {5,6,7}, D1:E1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});

    s.setFormulaCell(PT("A1"), "match({1, 2}, 2, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});

    s.setFormulaCell(PT("A1"), "match({1;1}, {1,2}, \"e\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    CHECK(s.getValue(PT("A2")) == Error::InvalidValue);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 2});
    
    s.setFormulaCell(PT("D1"), "{5,6,7}");
    s.setFormulaCell(PT("A1"), "match({5; 6; 7}, D1:F1, {-1, 1})");
    CHECK(s.getValue(PT("A1")) == 3); //Excel does 1 here, google does 3
    CHECK(s.getValue(PT("A2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A3")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getValue(PT("B2")) == 2);
    CHECK(s.getValue(PT("B3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 3});
    
    s.setFormulaCell(PT("D1"), "{5,6,7}");
    s.setFormulaCell(PT("A1"), "match({5; 6; 7}, D1:F1, {-1, 1, 0})");
    CHECK(s.getValue(PT("A1")) == 3); //See above
    CHECK(s.getValue(PT("A2")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("A3")) == Error::InvalidArgs);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getValue(PT("B2")) == 2);
    CHECK(s.getValue(PT("B3")) == 3);
    CHECK(s.getValue(PT("C1")) == 1);
    CHECK(s.getValue(PT("C2")) == 2);
    CHECK(s.getValue(PT("C3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 3});
}

TEST_CASE( "Exact Matching", "[formula][formula-match]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "match(0, {1,2}, 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "match(3, {1,2}, 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "match(2, {1,2}, 0)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});


    //blank matches 0
    s.setFormulaCell(PT("A1"), "match(, {1,2,0,4}, 0)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank does not match ""
    s.setFormulaCell(PT("A1"), "match(, {1,2,\"\",4}, 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //errors in data are ignored
    s.setFormulaCell(PT("A1"), "match(5, {#NAME?,2,2,4,0,7}, 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "match(2, {#NAME?,2,2,4,0,7}, 0)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //boolean and numbers don't cross match
    s.setFormulaCell(PT("A1"), "match(true, {1,2,2,4,true,7}, 0)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //strings and numbers don't cross match
    s.setFormulaCell(PT("A1"), "match(\"1\", {1,2,2,4,\"1\",7}, 0)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //wildcards work
    s.setFormulaCell(PT("A1"), "match(\"1*\", {1,2,\"132\",7}, 0)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    s.setFormulaCell(PT("A1"), "match(\"1?2\", {1,2,\"132\",7}, 0)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
}

TEST_CASE( "Positive Matching", "[formula][formula-match]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "match(0, {1,2}, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    s.setFormulaCell(PT("A1"), "match(3, {1,2}, 1)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "match(1, {1,2}, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    s.setFormulaCell(PT("A1"), "match(5, {3,4,5,5,5,6}, 1)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank matches 0
    s.setFormulaCell(PT("A1"), "match(, {0,4}, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank does not match ""
    s.setFormulaCell(PT("A1"), "match(, {1,\"\"}, 5)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //errors in data are ignored
    s.setFormulaCell(PT("A1"), "match(5, {#NAME?,2,0}, 1.5)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "match(2, {#NAME?,0,2}, 1.01)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //boolean and numbers don't cross match
    s.setFormulaCell(PT("A1"), "match(true, {1;2;true}, 1)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //strings and numbers don't cross match
    s.setFormulaCell(PT("A1"), "match(\"1\", {1;2;\"1\"}, 1)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
}

TEST_CASE( "Negative Matching", "[formula][formula-match]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "match(3, {2,1}, -1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    s.setFormulaCell(PT("A1"), "match(0, {2,1}, -1)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "match(1, {2,1}, -1)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    s.setFormulaCell(PT("A1"), "match(5, {7,6,5,5,5,4}, -1)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank matches 0
    s.setFormulaCell(PT("A1"), "match(, {4;0}, -1)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //blank does not match ""
    s.setFormulaCell(PT("A1"), "match(, {1,\"\"}, -5)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
    
    //errors in data are ignored
    s.setFormulaCell(PT("A1"), "match(5, {#NAME?,2,0}, -1.5)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "match(2, {#NAME?,3,2}, -1.01)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //boolean and numbers don't cross match
    s.setFormulaCell(PT("A1"), "match(true, {1;2;true}, -1)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //strings and numbers don't cross match
    s.setFormulaCell(PT("A1"), "match(\"1\", {1;2;\"1\"}, -1)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
}

