#include <catch2/catch_session.hpp>
#include <catch2/catch_tostring.hpp>

int main(int argc, char ** argv) {

    Catch::StringMaker<double>::precision = 17;
    return Catch::Session().run( argc, argv );
}

