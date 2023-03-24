#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

using namespace Spreader;
using Catch::Matchers::RangeEquals;


TEST_CASE( "Invalid arguments", "[formula][formula-weeknum]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A1"), "ISOWEEKNUM()");
    
    s.setFormulaCell(PT("A1"), "WEEKNUM()");
    
    s.setFormulaCell(PT("A1"), "ISOWEEKNUM(1,2)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "WEEKNUM(1,2,3)");
    CHECK(s.getValue(PT("A1")) == Error::InvalidFormula);
    
    s.setFormulaCell(PT("A1"), "WEEKNUM(1,0)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "WEEKNUM(1,3)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
    
    s.setFormulaCell(PT("A1"), "WEEKNUM(1,18)");
    CHECK(s.getValue(PT("A1")) == Error::NotANumber);
}

#define CHECK_ALL(exp) \
    for(SizeType y = 0; y < 12; ++y) \
        actual[y] = s.getValue({0, y + 1}); \
    CHECK_THAT(actual, RangeEquals(std::vector<Scalar>exp));

TEST_CASE( "All variants", "[formula][formula-weeknum]" ) {
    
    Sheet s;
    
    s.setFormulaCell(PT("A2"),  "ISOWEEKNUM(A1)");
    s.setFormulaCell(PT("A3"),  "WEEKNUM(A1)");
    s.setFormulaCell(PT("A4"),  "WEEKNUM(A1,1)");
    s.setFormulaCell(PT("A5"),  "WEEKNUM(A1,2)");
    s.setFormulaCell(PT("A6"),  "WEEKNUM(A1,11)");
    s.setFormulaCell(PT("A7"),  "WEEKNUM(A1,12)");
    s.setFormulaCell(PT("A8"),  "WEEKNUM(A1,13)");
    s.setFormulaCell(PT("A9"),  "WEEKNUM(A1,14)");
    s.setFormulaCell(PT("A10"), "WEEKNUM(A1,15)");
    s.setFormulaCell(PT("A11"), "WEEKNUM(A1,16)");
    s.setFormulaCell(PT("A12"), "WEEKNUM(A1,17)");
    s.setFormulaCell(PT("A13"), "WEEKNUM(A1,21)");
    
    std::vector<Scalar> actual(12);
    
    s.setFormulaCell(PT("A1"), "DATE(1977,01,01)");
    CHECK_ALL(({53,1,1,1,1,1,1,1,1,1,1,53}));
    
    s.setFormulaCell(PT("A1"), "DATE(1977,01,02)");
    CHECK_ALL(({53,2,2,1,1,1,1,1,1,1,2,53}));
    
    s.setFormulaCell(PT("A1"), "DATE(1977,12,31)");
    CHECK_ALL(({52,53,53,53,53,53,53,53,53,53,53,52}));
    
    s.setFormulaCell(PT("A1"), "DATE(1978,01,01)");
    CHECK_ALL(({52,1,1,1,1,1,1,1,1,1,1,52}));
    
    s.setFormulaCell(PT("A1"), "DATE(1978,01,02)");
    CHECK_ALL(({1,1,1,2,2,1,1,1,1,1,1,1}));
    
    s.setFormulaCell(PT("A1"), "DATE(1978,12,31)");
    CHECK_ALL(({52,53,53,53,53,53,53,53,53,53,53,52}));
    
    s.setFormulaCell(PT("A1"), "DATE(1979,01,01)");
    CHECK_ALL(({1,1,1,1,1,1,1,1,1,1,1,1}));
    
    s.setFormulaCell(PT("A1"), "DATE(1980,12,29)");
    CHECK_ALL(({1,53,53,53,53,52,53,53,53,53,53,1}));
    
    s.setFormulaCell(PT("A1"), "DATE(2023,01,30)");
    CHECK_ALL(({5,5,5,6,6,5,5,5,5,5,5,5}));
}
