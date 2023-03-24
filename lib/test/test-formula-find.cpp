#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid arguments", "[formula][formula-find]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "FIND()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "FIND(\"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "FIND(\"a\", \"b\", 3, 4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "FIND(\"a\", \"a\", 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "FIND(\"a\", \"a\", -1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
}


TEST_CASE( "Scalar arguments", "[formula][formula-find]" ) {   
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "FIND(\"\", \"\")");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "FIND(\"\", \"\", 1)");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "FIND(\"\", \"\", 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "FIND(\"\", \"abc\")");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "FIND(\"\", \"abc\", 1)");
    CHECK(s.getValue(PT("A1")) == 1);

    s.setFormulaCell(PT("A1"), "FIND(\"\", \"abc\", 2)");
    CHECK(s.getValue(PT("A1")) == 2);

    s.setFormulaCell(PT("A1"), "FIND(\"\", \"abc\", 3)");
    CHECK(s.getValue(PT("A1")) == 3);

    s.setFormulaCell(PT("A1"), "FIND(\"\", \"abc\", 4)");
    CHECK(s.getValue(PT("A1")) == 4);
    
    s.setFormulaCell(PT("A1"), "FIND(\"\", \"abc\", 5)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "FIND(\"a\", \"abc\")");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "FIND(\"q\", \"abc\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "FIND(\"a\", \"bac\")");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "FIND(\"a\", \"bac\", 2)");
    CHECK(s.getValue(PT("A1")) == 2);
    
    s.setFormulaCell(PT("A1"), "FIND(\"a\", \"bac\", 3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "FIND(\"𐀀\", \"a水𐀀𝄞\")");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "FIND(\"𐀀\", \"a水𐀀𝄞\", 3)");
    CHECK(s.getValue(PT("A1")) == 3);
    
    s.setFormulaCell(PT("A1"), "FIND(\"𐀀\", \"a水𐀀𝄞\", 4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

}
