#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-mid-replace]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "REPLACE()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MID()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "REPLACE(\"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MID(\"a\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "REPLACE(\"a\", 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MID(\"a\", 1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "REPLACE(\"a\", 1, 2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MID(\"a\", 1, 2)");
    CHECK(s.getValue(PT("A1")) == "a");

    s.setFormulaCell(PT("A1"), "REPLACE(\"a\", 1, 2, \"b\", 3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "MID(\"a\", 1, 2, 3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 0, 0, \"x\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);

    s.setFormulaCell(PT("A1"), "MID(\"abc\", 0, 0)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 1, 0, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("xabc"));

    s.setFormulaCell(PT("A1"), "MID(\"abc\", 1, 0)");
    CHECK(s.getValue(PT("A1")) == SPRS(""));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 1, 1, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("xbc"));

    s.setFormulaCell(PT("A1"), "MID(\"abc\", 1, 1)");
    CHECK(s.getValue(PT("A1")) == SPRS("a"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 1, 2, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("xc"));

    s.setFormulaCell(PT("A1"), "MID(\"abc\", 1, 2)");
    CHECK(s.getValue(PT("A1")) == SPRS("ab"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 1, 3, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("x"));
    
    s.setFormulaCell(PT("A1"), "MID(\"abc\", 1, 3)");
    CHECK(s.getValue(PT("A1")) == SPRS("abc"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 1, 4, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("x"));
    
    s.setFormulaCell(PT("A1"), "MID(\"abc\", 1, 4)");
    CHECK(s.getValue(PT("A1")) == SPRS("abc"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 2, 0, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("axbc"));
    
    s.setFormulaCell(PT("A1"), "MID(\"abc\", 2, 0)");
    CHECK(s.getValue(PT("A1")) == SPRS(""));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 2, 2, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("ax"));
    
    s.setFormulaCell(PT("A1"), "MID(\"abc\", 2, 2)");
    CHECK(s.getValue(PT("A1")) == SPRS("bc"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 3, 1, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("abx"));
    
    s.setFormulaCell(PT("A1"), "MID(\"abc\", 3, 1)");
    CHECK(s.getValue(PT("A1")) == SPRS("c"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 4, 0, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("abcx"));
    
    s.setFormulaCell(PT("A1"), "MID(\"abc\", 4, 0)");
    CHECK(s.getValue(PT("A1")) == SPRS(""));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"abc\", 4, 1, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("abcx"));
    
    s.setFormulaCell(PT("A1"), "MID(\"abc\", 4, 1)");
    CHECK(s.getValue(PT("A1")) == SPRS(""));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"a水𐀀𝄞\", 3, 1, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("a水x𝄞"));
    
    s.setFormulaCell(PT("A1"), "MID(\"a水𐀀𝄞\", 3, 1)");
    CHECK(s.getValue(PT("A1")) == SPRS("𐀀"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"a水𐀀𝄞\", 4, 1, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("a水x𝄞"));
    
    s.setFormulaCell(PT("A1"), "MID(\"a水𐀀𝄞\", 4, 1)");
    CHECK(s.getValue(PT("A1")) == SPRS("𐀀"));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"a水𐀀𝄞\", 3, 0, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("a水x𐀀𝄞"));
    
    s.setFormulaCell(PT("A1"), "MID(\"a水𐀀𝄞\", 3, 0)");
    CHECK(s.getValue(PT("A1")) == SPRS(""));
    
    s.setFormulaCell(PT("A1"), "REPLACE(\"a水𐀀𝄞\", 4, 0, \"x\")");
    CHECK(s.getValue(PT("A1")) == SPRS("a水x𝄞"));
    
    s.setFormulaCell(PT("A1"), "MID(\"a水𐀀𝄞\", 4, 0)");
    CHECK(s.getValue(PT("A1")) == SPRS(""));
}
