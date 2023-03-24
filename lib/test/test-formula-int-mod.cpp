#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-int-mod]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "INT()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MOD()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "MOD(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "INT(1.9)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "INT(-1.9)");
    CHECK(s.getValue(PT("A1")) == -2);

    s.setFormulaCell(PT("A1"), "INT(0)");
    CHECK(s.getValue(PT("A1")) == 0);

    s.setFormulaCell(PT("A1"), "MOD(3, 2)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "MOD(-3, 2)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "MOD(3, -2)");
    CHECK(s.getValue(PT("A1")) == -1);

    s.setFormulaCell(PT("A1"), "MOD(-3, -2)");
    CHECK(s.getValue(PT("A1")) == -1);
    
    s.setFormulaCell(PT("A1"), "MOD(-3, 0)");
    CHECK(s.getValue(PT("A1")) == Error::DivisionByZero);
    
    s.setFormulaCell(PT("A1"), "MOD(-8.9, 2)");
    //CHECK(s.getValue(PT("A1")) == 1.1);
    CHECK_THAT(get<Number>(s.getValue(PT("A1"))).value(), NumericallyEqualsTo(1.1));
    
    s.setFormulaCell(PT("A1"), "MOD(-8.9, -2.3)");
    //CHECK(s.getValue(PT("A1")) == -2.);
    CHECK_THAT(get<Number>(s.getValue(PT("A1"))).value(), NumericallyEqualsTo(-2.));
}
