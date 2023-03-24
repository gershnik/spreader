#include "../src/scalar-numeric-functions.h"


#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

Scalar doAdd(Scalar lhs, Scalar rhs) {
    ScalarAdd add;
    add.handleFirst(lhs);
    add.handleSecond(rhs);
    return add.result;
}


TEST_CASE( "Scalar Addition", "[scalar]" ) {

    CHECK(doAdd(Scalar(), Scalar()) == Scalar(0.));
    CHECK(doAdd(Scalar(), Scalar(true)) == SPRN(1.));
    CHECK(doAdd(Scalar(), Scalar(0.1)) == Scalar(0.1));
    CHECK(doAdd(Scalar(), Scalar(SPRS("haha"))) == Scalar(Error::InvalidValue));
    CHECK(doAdd(Scalar(), Scalar(Error::InvalidName)) == Scalar(Error::InvalidName));

    CHECK(doAdd(Scalar(true), Scalar()) == Scalar(1.));
    CHECK(doAdd(Scalar(false), Scalar(true)) == Scalar(1.0));
    CHECK(doAdd(Scalar(false), Scalar(3.0)) == Scalar(3.0));
    CHECK(doAdd(Scalar(false), Scalar(SPRS("haha"))) == Scalar(Error::InvalidValue));
    CHECK(doAdd(Scalar(true), Scalar(Error::InvalidName)) == Scalar(Error::InvalidName));

    CHECK(doAdd(Scalar(1.3), Scalar()) == Scalar(1.3));
    CHECK(doAdd(Scalar(1.3), Scalar(true)) == Scalar(2.3));
    CHECK(doAdd(Scalar(2.0), Scalar(3.0)) == Scalar(5.0));
    CHECK(doAdd(Scalar(SPRN(1e308)), Scalar(SPRN(1e308))) == Scalar(Error::NotANumber));
    CHECK(doAdd(Scalar(1.3), Scalar(SPRS("haha"))) == Scalar(Error::InvalidValue));
    CHECK(doAdd(Scalar(1.3), Scalar(Error::InvalidName)) == Scalar(Error::InvalidName));

    CHECK(doAdd(Scalar(SPRS("haha")), Scalar()) == Scalar(Error::InvalidValue));
    CHECK(doAdd(Scalar(SPRS("haha")), Scalar(true)) == Scalar(Error::InvalidValue));
    CHECK(doAdd(Scalar(SPRS("haha")), Scalar(4.5)) == Scalar(Error::InvalidValue));
    CHECK(doAdd(Scalar(SPRS("haha")), Scalar(SPRS("hoho"))) == Scalar(Error::InvalidValue));
    CHECK(doAdd(Scalar(SPRS("haha")), Scalar(Error::InvalidName)) == Scalar(Error::InvalidValue));

    CHECK(doAdd(Scalar(Error::InvalidName), Scalar()) == Scalar(Error::InvalidName));
    CHECK(doAdd(Scalar(Error::InvalidName), Scalar(true)) == Scalar(Error::InvalidName));
    CHECK(doAdd(Scalar(Error::InvalidName), Scalar(4.5)) == Scalar(Error::InvalidName));
    CHECK(doAdd(Scalar(Error::InvalidName), Scalar(SPRS("hoho"))) == Scalar(Error::InvalidName));
    CHECK(doAdd(Scalar(Error::InvalidName), Scalar(Error::InvalidValue)) == Scalar(Error::InvalidName));
    
    CHECK(doAdd(doAdd(doAdd(1.333, 1.225), -1.333), -1.225) == -2.220446049250313E-16);
}

Scalar doDiv(Scalar lhs, Scalar rhs) {
    ScalarDivide div;
    div.handleFirst(lhs);
    div.handleSecond(rhs);
    return div.result;
}

TEST_CASE( "Scalar Division", "[scalar]" ) {

    CHECK(doDiv(Scalar(), Scalar()) == Scalar(Error::DivisionByZero));
    CHECK(doDiv(Scalar(), Scalar(true)) == Scalar(0.));
    CHECK(doDiv(Scalar(), Scalar(0.1)) == Scalar(0.));
    CHECK(doDiv(Scalar(), Scalar(SPRS("haha"))) == Scalar(Error::InvalidValue));
    CHECK(doDiv(Scalar(), Scalar(Error::InvalidName)) == Scalar(Error::InvalidName));

    CHECK(doDiv(Scalar(true), Scalar()) == Scalar(Error::DivisionByZero));
    CHECK(doDiv(Scalar(false), Scalar(true)) == Scalar(0.));
    CHECK(doDiv(Scalar(false), Scalar(3.0)) == Scalar(0.));
    CHECK(doDiv(Scalar(false), Scalar(0.0)) == Scalar(Error::DivisionByZero));
    CHECK(doDiv(Scalar(false), Scalar(SPRS("haha"))) == Scalar(Error::InvalidValue));
    CHECK(doDiv(Scalar(true), Scalar(Error::InvalidName)) == Scalar(Error::InvalidName));

    CHECK(doDiv(Scalar(1.3), Scalar()) == Scalar(Error::DivisionByZero));
    CHECK(doDiv(Scalar(1.3), Scalar(true)) == Scalar(1.3));
    CHECK(doDiv(Scalar(2.0), Scalar(3.0)) == Scalar(2. / 3.));
    CHECK(doDiv(Scalar(1.3), Scalar(SPRS("haha"))) == Scalar(Error::InvalidValue));
    CHECK(doDiv(Scalar(1.3), Scalar(Error::InvalidName)) == Scalar(Error::InvalidName));

    CHECK(doDiv(Scalar(SPRS("haha")), Scalar()) == Scalar(Error::InvalidValue));
    CHECK(doDiv(Scalar(SPRS("haha")), Scalar(true)) == Scalar(Error::InvalidValue));
    CHECK(doDiv(Scalar(SPRS("haha")), Scalar(4.5)) == Scalar(Error::InvalidValue));
    CHECK(doDiv(Scalar(SPRS("haha")), Scalar(SPRS("hoho"))) == Scalar(Error::InvalidValue));
    CHECK(doDiv(Scalar(SPRS("haha")), Scalar(Error::InvalidName)) == Scalar(Error::InvalidValue));

    CHECK(doDiv(Scalar(Error::InvalidName), Scalar()) == Scalar(Error::InvalidName));
    CHECK(doDiv(Scalar(Error::InvalidName), Scalar(true)) == Scalar(Error::InvalidName));
    CHECK(doDiv(Scalar(Error::InvalidName), Scalar(4.5)) == Scalar(Error::InvalidName));
    CHECK(doDiv(Scalar(Error::InvalidName), Scalar(SPRS("hoho"))) == Scalar(Error::InvalidName));
    CHECK(doDiv(Scalar(Error::InvalidName), Scalar(Error::InvalidValue)) == Scalar(Error::InvalidName));
}

Scalar doPow(Scalar lhs, Scalar rhs) {
    ScalarPower pow;
    pow.handleFirst(lhs);
    pow.handleSecond(rhs);
    return pow.result;
}


TEST_CASE( "Scalar Power", "[scalar]" ) {
    
    CHECK(doPow(Scalar(), Scalar()) == Error::NotANumber);
    
    CHECK(doPow(Scalar(1.2), Scalar(2)) == 1.44);
    CHECK(doPow(Scalar(0), Scalar(-3)) == Error::DivisionByZero);
    CHECK(doPow(Scalar(-3), Scalar(1.2)) == Error::NotANumber);
    CHECK(doPow(Scalar(0), Scalar(0)) == Error::NotANumber);
}
