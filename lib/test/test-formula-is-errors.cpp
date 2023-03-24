#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid arguments", "[formula][formula-is-errors]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "ERROR.TYPE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ISBLANK()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ISERR()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "ERROR.TYPE(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ISBLANK(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "ISERR(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
}

TEST_CASE( "ERROR.TYPE", "[formula][formula-is-errors]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "ERROR.TYPE(\"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);

    s.setFormulaCell(PT("A1"), "ERROR.TYPE(#N/A)");
    CHECK(s.getValue(PT("A1")) == 7);
}

TEST_CASE( "ISBLANK", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISBLANK(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == false);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == true);
}

TEST_CASE( "ISERR", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISERR(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == false);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::NotANumber);
    CHECK(s.getValue(PT("A1")) == true);
}

TEST_CASE( "ISERROR", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISERROR(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == false);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), Error::NotANumber);
    CHECK(s.getValue(PT("A1")) == true);
}

TEST_CASE( "ISLOGICAL", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISLOGICAL(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == false);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), false);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == false);
}

TEST_CASE( "ISNA", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISNA(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == false);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), Error::NotANumber);
    CHECK(s.getValue(PT("A1")) == false);
}

TEST_CASE( "ISNONTEXT", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISNONTEXT(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == false);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == true);
}

TEST_CASE( "ISNUMBER", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISNUMBER(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == false);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == false);
}

TEST_CASE( "ISTEXT", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISTEXT(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == true);

    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == false);
}

TEST_CASE( "ISEVEN", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISEVEN(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    
    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), -2);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), 1.7);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), -2.8);
    CHECK(s.getValue(PT("A1")) == true);
    
}

TEST_CASE( "ISODD", "[formula][formula-is-errors]" ) {

    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISODD(A2)");

    s.setValueCell(PT("A2"), "a");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setValueCell(PT("A2"), true);
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setValueCell(PT("A2"), Scalar::Blank{});
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), Error::InvalidArgs);
    CHECK(s.getValue(PT("A1")) == Error::InvalidArgs);
    
    s.setValueCell(PT("A2"), 1);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), -2);
    CHECK(s.getValue(PT("A1")) == false);
    
    s.setValueCell(PT("A2"), 1.7);
    CHECK(s.getValue(PT("A1")) == true);
    
    s.setValueCell(PT("A2"), -2.8);
    CHECK(s.getValue(PT("A1")) == false);
    
}
