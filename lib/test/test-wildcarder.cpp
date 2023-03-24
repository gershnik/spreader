#include "../src/wildcarder.h"
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Empty pattern", "[wildcarder]" ) {

    Wildcarder wc('~');

    CHECK(wc.match("", ""));
    CHECK(!wc.match("", "a"));

}

TEST_CASE( "Simple string", "[wildcarder]" ) {

    Wildcarder wc('~');

    CHECK(!wc.match("a", ""));
    CHECK( wc.match("a", "a"));
    CHECK( wc.match("ab", "ab"));
    CHECK(!wc.match("ab", "ba"));
}

TEST_CASE( "Star", "[wildcarder]" ) {

    Wildcarder wc('~');

    CHECK( wc.match("*", ""));
    CHECK( wc.match("*", "a"));
    CHECK( wc.match("*", "abcd"));
    
    CHECK(!wc.match("*a", ""));
    CHECK( wc.match("*a", "a"));
    CHECK( wc.match("*a", "ba"));
    CHECK( wc.match("*a", "bca"));
    CHECK( wc.match("*a", "aaaaa"));
    CHECK(!wc.match("*a", "ab"));
    
    CHECK(!wc.match("*ab", ""));
    CHECK( wc.match("*ab", "ab"));
    CHECK( wc.match("*ab", "bab"));
    CHECK( wc.match("*ab", "bcab"));
    CHECK( wc.match("*ab", "ababababab"));
    CHECK(!wc.match("*ab", "abc"));
    
    CHECK(!wc.match("a*", ""));
    CHECK( wc.match("a*", "a"));
    CHECK( wc.match("a*", "ab"));
    CHECK( wc.match("a*", "abc"));
    CHECK( wc.match("a*", "aaaaa"));
    CHECK(!wc.match("a*", "ba"));
    
    CHECK(!wc.match("a*b", ""));
    CHECK( wc.match("a*b", "ab"));
    CHECK( wc.match("a*b", "acb"));
    CHECK( wc.match("a*b", "abbbbb"));
    
    CHECK(!wc.match("a**b", ""));
    CHECK( wc.match("a**b", "ab"));
    CHECK( wc.match("a**b", "acb"));
    CHECK( wc.match("a**b", "abbbbb"));
}

TEST_CASE( "Question", "[wildcarder]" ) {
    
    Wildcarder wc('~');

    CHECK(!wc.match("?", ""));
    CHECK( wc.match("?", "a"));
    CHECK( wc.match("?", "?"));
    CHECK(!wc.match("?", "ab"));
    
    CHECK(!wc.match("a?", "a"));
    CHECK( wc.match("a?", "ab"));
    CHECK( wc.match("a?", "aq"));
    CHECK(!wc.match("a?", "abc"));
    
    CHECK(!wc.match("?a", "a"));
    CHECK( wc.match("?a", "ma"));
    CHECK( wc.match("?a", "\0a"));
    CHECK(!wc.match("?a", "bca"));
    
    CHECK(!wc.match("a?b", "ab"));
    CHECK( wc.match("a?b", "a!b"));
    CHECK( wc.match("a?b", "aab"));
    
    CHECK(!wc.match("a??b", "amb"));
    CHECK( wc.match("a??b", "amzb"));
    
}

TEST_CASE( "Escaping", "[wildcarder]" ) {
    
    Wildcarder wc('~');
    
    CHECK( wc.match("~", "~"));
    CHECK( wc.match("~~", "~"));
    CHECK(!wc.match("~~", "~~"));
    CHECK( wc.match("~q", "~q"));
    CHECK(!wc.match("~q", "q"));
    CHECK( wc.match("~?", "?"));
    CHECK(!wc.match("~?", "~?"));
    CHECK( wc.match("~*", "*"));
    CHECK(!wc.match("~*", "~*"));
    
    CHECK( wc.match("a*~", "a~qqqq~"));
    
}
