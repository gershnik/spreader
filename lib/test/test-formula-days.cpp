#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Invalid input", "[formula][formula-datedif]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "DATEDIF()");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(1)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(1,2,\"a\",4)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(2,1,\"Y\")");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(2,1,\"Q\")");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(2,1,\"Y \")");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
}
    
TEST_CASE( "Y", "[formula][formula-datedif]" ) {
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2014, 7, 7), DATE(2023, 1, 28), \"Y\")");
    CHECK(s.getValue(PT("A1")) == 8);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2004, 11, 12), DATE(2023, 1, 28), \"Y\")");
    CHECK(s.getValue(PT("A1")) == 18);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 1, 29), DATE(2020, 3, 28), \"Y\")");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 1, 29), DATE(2021, 1, 28), \"Y\")");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 1, 29), DATE(2021, 2, 28), \"Y\")");
    CHECK(s.getValue(PT("A1")) == 1);
}

TEST_CASE( "MD", "[formula][formula-datedif]" ) {
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 1, 29), DATE(2020, 3, 28), \"MD\")");
    CHECK(s.getValue(PT("A1")) == 28);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2021, 1, 29), DATE(2021, 3, 28), \"MD\")");
    CHECK(s.getValue(PT("A1")) == 27);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2021, 1, 28), DATE(2021, 3, 28), \"MD\")");
    CHECK(s.getValue(PT("A1")) == 0);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2021, 1, 27), DATE(2021, 3, 28), \"MD\")");
    CHECK(s.getValue(PT("A1")) == 1);
}

TEST_CASE( "YM", "[formula][formula-datedif]" ) {
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 1, 29), DATE(2020, 3, 28), \"YM\")");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2021, 1, 29), DATE(2021, 3, 28), \"YM\")");
    CHECK(s.getValue(PT("A1")) == 1);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 2, 28), DATE(2021, 1, 28), \"YM\")");
    CHECK(s.getValue(PT("A1")) == 11);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 2, 28), DATE(2021, 1, 25), \"YM\")");
    CHECK(s.getValue(PT("A1")) == 10);
}

TEST_CASE( "YD", "[formula][formula-datedif]" ) {
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 1, 29), DATE(2024, 2, 28), \"YD\")");
    CHECK(s.getValue(PT("A1")) == 30);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 3, 29), DATE(2024, 2, 28), \"YD\")");
    CHECK(s.getValue(PT("A1")) == 336);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 3, 29), DATE(2024, 2, 29), \"YD\")");
    CHECK(s.getValue(PT("A1")) == 337);
    
    s.setFormulaCell(PT("A1"), "DATEDIF(DATE(2020, 1, 29), DATE(2024, 3, 28), \"YD\")");
    CHECK(s.getValue(PT("A1")) == 59);
}
