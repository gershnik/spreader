#include "../src/scalar-xif-matcher.h"
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Empty matcher", "[scalar-xif-matcher]" ) {

    {
        XIfMatcher matcher(SPRS(""));
        
        CHECK( matcher(Scalar()));
        CHECK( matcher(SPRS("")));
        CHECK(!matcher(0.));
        CHECK(!matcher(true));
    }
    {
        XIfMatcher matcher(Scalar{});
        
        CHECK( matcher(Scalar()));
        CHECK( matcher(SPRS("")));
        CHECK(!matcher(0.));
        CHECK(!matcher(true));
    }
}

TEST_CASE( "Number matcher", "[scalar-xif-matcher]" ) {
    
    XIfMatcher matcher(0);
    
    CHECK(!matcher(Scalar()));
    CHECK(!matcher(SPRS("")));
    CHECK( matcher(SPRS("0")));
    CHECK(!matcher(false));
    CHECK(!matcher(Error::DivisionByZero));
    CHECK( matcher(0));
}
