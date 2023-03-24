#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Direct arguments", "[formula][formula-time]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A2"), "HOUR(A1)");
    s.setFormulaCell(PT("A3"), "MINUTE(A1)");
    s.setFormulaCell(PT("A4"), "SECOND(A1)");
    
    
    s.setFormulaCell(PT("A1"), "TIME()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A2")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A3")) == Error::InvalidFormula);
    CHECK(s.getValue(PT("A4")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "TIME(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "TIME(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "TIME(1,2,3,4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "TIME(-1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    CHECK(s.getValue(PT("A2")) == Error::NotANumber);
    CHECK(s.getValue(PT("A3")) == Error::NotANumber);
    CHECK(s.getValue(PT("A4")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "TIME(32768,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);

    s.setFormulaCell(PT("A1"), "TIME(0,0,0)");
    CHECK(s.getValue(PT("A1")) == 0.);
    CHECK(s.getValue(PT("A2")) == 0);
    CHECK(s.getValue(PT("A3")) == 0);
    CHECK(s.getValue(PT("A4")) == 0);
    
    s.setFormulaCell(PT("A1"), "TIME(1,0,0)");
    CHECK(s.getValue(PT("A1")) == 0.041666666666666664);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 0);
    CHECK(s.getValue(PT("A4")) == 0);
    
    s.setFormulaCell(PT("A1"), "TIME(1.5,0,0)");
    CHECK(s.getValue(PT("A1")) == 0.041666666666666664);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 0);
    CHECK(s.getValue(PT("A4")) == 0);
    
    s.setFormulaCell(PT("A1"), "TIME(25,0,0)");
    CHECK(s.getValue(PT("A1")) == 0.04166666666666674);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 0);
    CHECK(s.getValue(PT("A4")) == 0);
    
    s.setFormulaCell(PT("A1"), "TIME(1,1,0)");
    CHECK(s.getValue(PT("A1")) == 0.04236111111111111);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 1);
    CHECK(s.getValue(PT("A4")) == 0);
    
    s.setFormulaCell(PT("A1"), "TIME(1,1.8,0)");
    CHECK(s.getValue(PT("A1")) == 0.04236111111111111);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 1);
    CHECK(s.getValue(PT("A4")) == 0);
    
    s.setFormulaCell(PT("A1"), "TIME(0,61.8,0)");
    CHECK(s.getValue(PT("A1")) == 0.04236111111111111);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 1);
    CHECK(s.getValue(PT("A4")) == 0);
    
    s.setFormulaCell(PT("A1"), "TIME(1,2,3)");
    CHECK(s.getValue(PT("A1")) == 0.043090277777777776);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 2);
    CHECK(s.getValue(PT("A4")) == 3);
    
    s.setFormulaCell(PT("A1"), "TIME(1,2,63)");
    CHECK(s.getValue(PT("A1")) == 0.043784722222222225);
    CHECK(s.getValue(PT("A2")) == 1);
    CHECK(s.getValue(PT("A3")) == 3);
    CHECK(s.getValue(PT("A4")) == 3);
    
    s.setFormulaCell(PT("A1"), "TIME(1,2,3603)");
    CHECK(s.getValue(PT("A1")) == 0.08475694444444444);
    CHECK(s.getValue(PT("A2")) == 2);
    CHECK(s.getValue(PT("A3")) == 2);
    CHECK(s.getValue(PT("A4")) == 3);
    
    s.setFormulaCell(PT("A1"), "TIME(32767, -10000, 1)");
    CHECK(s.getValue(PT("A1")) == 0.34723379629622286);
    CHECK(s.getValue(PT("A2")) == 8);
    CHECK(s.getValue(PT("A3")) == 20);
    CHECK(s.getValue(PT("A4")) == 1);
    
    s.setFormulaCell(PT("A1"), "TIME(32768, -10000, 1)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
}
