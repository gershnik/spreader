#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Self-dependent array formula", "[sheet-funky]" ) {
    {
        Sheet s;
        s.setFormulaCell(PT("A1"), "B1:C2 + 1");
        
        //Excel makes them all 1, Google flickers between #REF! and all 1s
//        CHECK(s.getValue(PT("A1")) == 1);
//        CHECK(s.getValue(PT("A2")) == 1);
//        CHECK(s.getValue(PT("B1")) == 1);
//        CHECK(s.getValue(PT("B2")) == 1);
//        CHECK(s.nonNullCellCount() == 4);
//
//        s.recalculate();
        
        CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
        CHECK(s.nonNullCellCount() == 1);
    }
}
