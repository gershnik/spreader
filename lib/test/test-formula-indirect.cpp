#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Single arguments", "[formula][formula-indirect]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "INDIRECT()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);

    s.setFormulaCell(PT("A1"), "INDIRECT(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);

    s.setFormulaCell(PT("A1"), "INDIRECT(\"_\")");
    CHECK(s.getValue(PT("A1")) == Error::InvalidReference);

    s.setValueCell(PT("Z2"), SPRS("Hello"));

    s.setFormulaCell(PT("A1"), "INDIRECT(\"Z2\")");
    CHECK(s.getValue(PT("A1")) == "Hello");
    
    s.setValueCell(PT("A2"), SPRS("Z2"));
    s.setFormulaCell(PT("A1"), "INDIRECT(A2)");
    CHECK(s.getValue(PT("A1")) == "Hello");
    
    s.setFormulaCell(PT("A1"), "INDIRECT({\"Z2\"})");
    CHECK(s.getValue(PT("A1")) == "Hello");
    
    s.setFormulaCell(PT("A1"), "INDIRECT(ADDRESS(2, 26))");
    CHECK(s.getValue(PT("A1")) == "Hello");

}

TEST_CASE( "Areas", "[formula][formula-indirect]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("X2"), "{1,2;3,4}");
    
    s.setFormulaCell(PT("A1"), "INDIRECT(\"X2:Y3\")");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("A2")) == 3);
    CHECK(s.getValue(PT("B2")) == 4);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 2});
    
    s.setFormulaCell(PT("A1"), "INDIRECT({\"X2:Y3\"})");
    CHECK(s.getValue(PT("A1")) == 1);
    CHECK(s.getValue(PT("B1")) == 2);
    CHECK(s.getValue(PT("A2")) == 3);
    CHECK(s.getValue(PT("B2")) == 4);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 2});
    
    s.setFormulaCell(PT("A1"), "INDIRECT({\"X2\", \"X3\"})");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    CHECK(s.getValue(PT("B1")) == Error::InvalidValue);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
    
    
    s.setFormulaCell(PT("A1"), "INDIRECT(\"X2\" & M2:N2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidValue);
    CHECK(s.getValue(PT("B1")) == Error::InvalidValue);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2, 1});
}
