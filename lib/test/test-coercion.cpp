#include <spreader/scalar.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


static_assert(!IsCoercible<std::monostate, std::monostate>);
static_assert( IsCoercible<std::monostate, bool>);
static_assert( IsCoercible<std::monostate, Number>);
static_assert( IsCoercible<std::monostate, String>);
static_assert(!IsCoercible<std::monostate, Error>);

static_assert(!IsCoercible<bool, std::monostate>);
static_assert(!IsCoercible<bool, bool>);
static_assert( IsCoercible<bool, Number>);
static_assert( IsCoercible<bool, String>);
static_assert(!IsCoercible<bool, Error>);

static_assert(!IsCoercible<Number, std::monostate>);
static_assert( IsCoercible<Number, bool>);
static_assert(!IsCoercible<Number, Number>);
static_assert( IsCoercible<Number, String>);
static_assert(!IsCoercible<Number, Error>);

static_assert(!IsCoercible<String, std::monostate>);
static_assert(!IsCoercible<String, bool>);
static_assert(!IsCoercible<String, Number>);
static_assert( IsOptionallyCoercible<String, Number>);
static_assert(!IsCoercible<String, String>);
static_assert(!IsCoercible<String, Error>);

static_assert(!IsCoercible<Error, std::monostate>);
static_assert(!IsCoercible<Error, bool>);
static_assert(!IsCoercible<Error, Number>);
static_assert(!IsCoercible<Error, String>);
static_assert(!IsCoercible<Error, Error>);

TEST_CASE( "String to number", "[coercion]" ) {

    CHECK(!coerceTo<Number>(SPRS("")));
    CHECK(!coerceTo<Number>(SPRS("abc")));
    CHECK(coerceTo<Number>(SPRS("3")).value() == 3);
    CHECK(coerceTo<Number>(SPRS("  3")).value() == 3);
    CHECK(coerceTo<Number>(SPRS("  2.8 ")).value() == SPRN(2.8));
}

TEST_CASE( "Number to string", "[coercion]" ) {

    CHECK(coerceTo<String>(Number(1)) == SPRS("1"));
    CHECK(coerceTo<String>(SPRN(2.8)) == SPRS("2.8"));
}


