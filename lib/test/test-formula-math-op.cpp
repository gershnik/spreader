#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Array arguments", "[formula][formula-math-op]" ) {

    Sheet s;

    s.setFormulaCell(PT("A1"), "{1,2,3} + {5;6;7}");
    CHECK(s.getValue(PT("A1")) == 6);
    CHECK(s.getValue(PT("A2")) == 7);
    CHECK(s.getValue(PT("A3")) == 8);
    CHECK(s.getValue(PT("B1")) == 7);
    CHECK(s.getValue(PT("B2")) == 8);
    CHECK(s.getValue(PT("B3")) == 9);
    CHECK(s.getValue(PT("C1")) == 8);
    CHECK(s.getValue(PT("C2")) == 9);
    CHECK(s.getValue(PT("C3")) == 10);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{3,3});
    
    s.setFormulaCell(PT("A1"), "{1,2;2,3;3,4} + {5;6;7}");
    CHECK(s.getValue(PT("A1")) == 6);
    CHECK(s.getValue(PT("A2")) == 8);
    CHECK(s.getValue(PT("A3")) == 10);
    CHECK(s.getValue(PT("B1")) == 7);
    CHECK(s.getValue(PT("B2")) == 9);
    CHECK(s.getValue(PT("B3")) == 11);
    CHECK(s.getFormulaInfo(PT("A1"))->extent == Spreader::Size{2,3});
}
