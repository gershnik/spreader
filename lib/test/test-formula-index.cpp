#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid arguments", "[formula][formula-index]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "INDEX()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "INDEX(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "INDEX(0, 1, 2, 3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "INDEX(1, -1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "INDEX(1, 1, -1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
}

TEST_CASE( "Scalar Arguments", "[formula][formula-index]" ) {
    
    Sheet s;

    s.setFormulaCell(PT("A1"), "INDEX(1, , )");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(1, 0)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(1, , 0)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(1, 0, 0)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(1, 1)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(1, 1.7)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(1, , 1)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(1, , 1.7)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INDEX(\"2\", 1, 1)");
    CHECK(s.getValue(PT("A1")) == SPRS("2"));

    s.setFormulaCell(PT("A1"), "INDEX(#N/A, 1, 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);

    s.setFormulaCell(PT("A1"), "INDEX(1, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);

    s.setFormulaCell(PT("A1"), "INDEX(1, , 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
}

TEST_CASE( "Main scalar, array coords", "[formula][formula-index]" ) {
   
    Sheet s;
    
    s.setFormulaCell(PT("D1"), "{1,1,2;1,1,2}");
    
    s.setFormulaCell(PT("A1"), "INDEX(1, D1:E1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});

    s.setFormulaCell(PT("A1"), "INDEX(1, D1:D2)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 2});
    
    s.setFormulaCell(PT("A1"), "INDEX(1, E1:F1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    s.setFormulaCell(PT("A1"), "INDEX(1, , D1:E1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    s.setFormulaCell(PT("A1"), "INDEX(1, D1:E1, D1:D2)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("B2")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 2});
}

TEST_CASE( "Main array, scalar coords", "[formula][formula-index]" ) {

    Sheet s;

    //vertical

    s.setFormulaCell(PT("A1"), "INDEX({1;2;3}, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "indeX({1;2;3}, , 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1")).value() == Sheet::FormulaInfo{SPRS("INDEX({1;2;3}, , 1)"), Spreader::Size{1, 3}});

    s.setFormulaCell(PT("A1"), "INDEX({1;2;3}, 0)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 3});

    s.setFormulaCell(PT("A1"), "INDEX({1;2;3}, )");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 3});

    s.setFormulaCell(PT("A1"), "INDEX({1;2;3}, 2)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1;2;3}, 3)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1;2;3}, 4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});


    //horizontal

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, 1, 0)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("C1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});

    s.setFormulaCell(PT("A1"), "indeX({1,2,3}, , 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1")).value() == Sheet::FormulaInfo{SPRS("INDEX({1,2,3}, , 1)"), Spreader::Size{1, 1}});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, 0)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("C1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, )");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("C1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, 2)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, 3)");
    CHECK(s.getValue(PT("A1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, 4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3}, 4, 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    //2D

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("C1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 1, 0)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("C1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 2)");
    CHECK(s.getValue(PT("A1")) == 4);
    CHECK(s.getValue(PT("B1")) == 5);
    CHECK(s.getValue(PT("C1")) == 6);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 2, 3)");
    CHECK(s.getValue(PT("A1")) == 6);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 3, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, , 2)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getValue(PT("A2")) == 5);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 2});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 0, 2)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getValue(PT("A2")) == 5);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 2});

    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, 0, 4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{1, 1});
 
}

TEST_CASE( "Main array, array coords", "[formula][formula-index]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("D1"), "{1,1,2,3;1,1,2,3}");
    
    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, D1:E1)");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 1);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    s.setFormulaCell(PT("A1"), "INDEX({1,2,3;4,5,6}, F1:G1)");
    CHECK(s.getValue(PT("A1")) == 4);
    CHECK(s.getValue(PT("B1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
}


TEST_CASE( "Main rect, array coords", "[formula][formula-index]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("D1"), "{1,1,2,3;1,1,2,3}");
    s.setFormulaCell(PT("I1"), "{1,2,3;4,5,6}");
    
    
    s.setFormulaCell(PT("A1"), "INDEX(I1:K2, F1:G1)");
    CHECK(s.getValue(PT("A1")) == 4); //excell says REF! here - Excel bug
    CHECK(s.getValue(PT("B1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    s.setFormulaCell(PT("A1"), "INDEX(I1:K2, F1:G1, 1)");
    CHECK(s.getValue(PT("A1")) == 4); //but not here!
    CHECK(s.getValue(PT("B1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    s.setFormulaCell(PT("A1"), "COLUMN(INDEX(I1:K2, F1:G1, 1))");
    CHECK(s.getValue(PT("A1")) == 9);
    CHECK(s.getValue(PT("B1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "ROW(INDEX(I1:K2, F1:G1, 1))");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "INDEX(I1:K2, 0, F1:G1)");
    CHECK(s.getValue(PT("A1")) == 2);
    CHECK(s.getValue(PT("B1")) == 3);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    s.setFormulaCell(PT("A1"), "INDEX(I1:K2, F1:G1, F1:G1)");
    CHECK(s.getValue(PT("A1")) == 5);
    CHECK(s.getValue(PT("B1")) == Error::InvalidReference);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    s.setFormulaCell(PT("A1"), "INDEX(I1:K2, E1:F2)"); //Excel barfs with #REF! on all 4 here
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 4);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("B2")) == 4);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 2});
    
    s.setFormulaCell(PT("A1"), "INDEX(I1:K2,, E1:F2)"); //But not here!
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("B2")) == 2);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 2});
}

