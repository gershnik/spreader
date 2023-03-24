#include "../src/number-matcher.h"
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

static void checkMatch(NumberMatcher<char> & nm, const char * text, double val, const char * rem = "") {
    std::string str(text);
    auto it = str.begin();
    INFO("text: " << text);
    CHECK(nm.prefixMatch(it, str.end()));
    CHECK(std::string(it, str.end()) == rem);
    CHECK(nm.value() == val);
}

static void checkMismatch(NumberMatcher<char> & nm, const char * text) {
    std::string str(text);
    auto it = str.begin();
    INFO("text: " << text);
    CHECK(!nm.prefixMatch(it, str.end()));
    CHECK(it - str.begin() == 0);
}

TEST_CASE( "Simple numbers", "[number-matcher]" ) {

    NumberMatcher<char> nm;

    checkMismatch(nm, "");
    checkMismatch(nm, " 1");

    checkMismatch(nm, "+");
    checkMismatch(nm, "-");
    
    checkMatch(nm, "0", 0);
    checkMatch(nm, "+0", 0);
    checkMatch(nm, "-0", 0);
    checkMatch(nm, "+1", 1);
    checkMatch(nm, "-1", -1);
    checkMatch(nm, "-1c", -1, "c");
    
    checkMismatch(nm, ".");
    checkMismatch(nm, "+.");
    checkMismatch(nm, "-.");
    checkMatch(nm, "0.", 0);
    checkMatch(nm, ".0", 0);
    checkMatch(nm, "0.0", 0);
    
    checkMatch(nm, "1.", 1);
    checkMatch(nm, ".1", .1);
    checkMatch(nm, "1.1", 1.1);
    
    checkMatch(nm, ".1c", .1, "c");
    checkMatch(nm, "1.c", 1, "c");
    checkMatch(nm, "1..", 1, ".");
    checkMatch(nm, "1.2.3", 1.2, ".3");
    
    checkMismatch(nm, "E");
    checkMismatch(nm, "e");
    checkMatch(nm, "1e", 1, "e");
    checkMatch(nm, "1E", 1, "E");
    checkMatch(nm, "1.E", 1, "E");
    checkMatch(nm, "0E0", 0);
    checkMatch(nm, ".0E0", 0);
    checkMatch(nm, ".0e0", 0);
    checkMatch(nm, "0E0", 0);
    checkMatch(nm, ".0E0", 0);
    checkMatch(nm, ".0e0", 0);
    
    checkMatch(nm, "1E0", 1);
    checkMatch(nm, ".1E0", .1);
    
    checkMatch(nm, "1E1", 10);
    checkMatch(nm, ".1E1", 1);
    
    checkMatch(nm, "1E1a", 10, "a");
}
