#include <spreader/floating-decimal.h>

#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

template<bool Sign, uint32_t IeeeExponent, uint64_t IeeeMantissa>
requires(IeeeExponent < 2047 && IeeeMantissa <= (uint64_t(1) << 53) - 1)
static consteval Number ieeeParts2Double() {
    constexpr uint64_t pattern = (uint64_t(Sign) << 63) | (uint64_t(IeeeExponent) << 52) | IeeeMantissa;
    return Number::bitPatternConstant<pattern>();
}

#define CHECK_FD(val, str) {\
    FloatingDecimal fd(SPRN(val)); \
    CHECK(fd.toString() == SPRS(str)); \
    CHECK(fd.toDouble() == val); \
}

#define CHECK_NFD(num, str) {\
    FloatingDecimal fd(num); \
    CHECK(fd.toString() == SPRS(str)); \
    CHECK(fd.toDouble() == num.value()); \
}


TEST_CASE( "Basic", "[floating-decimal]" ) {

    CHECK_FD( 0.0,  "0");
    CHECK_FD(-0.0,  "0");
    CHECK_FD( 1.0,  "1");
    CHECK_FD(-1.0 ,"-1");
    
    CHECK_FD(123.,   "123");
    CHECK_FD(12.3,   "12.3");
    CHECK_FD(1.23,   "1.23");
    CHECK_FD(0.123,  "0.123");
    CHECK_FD(0.0123, "0.0123");
}

TEST_CASE( "Switch to subnormal", "[floating-decimal]" ) {
    CHECK_FD(2.2250738585072014E-308, "2.2250738585072014E-308");
}

TEST_CASE( "Min and max", "[floating-decimal]" ) {
    CHECK_FD(SPR_BIT_CAST(double, 0x7fefffffffffffff), "1.7976931348623157E308");
    CHECK_FD(SPR_BIT_CAST(double, uint64_t(1)), "5E-324");
}

TEST_CASE( "Lots of trailing zeroes", "[floating-decimal]" ) {
    CHECK_FD(2.98023223876953125E-8, "2.9802322387695312E-8");
}

TEST_CASE( "Regression", "[floating-decimal]" ) {
    CHECK_FD(-2.109808898695963E16, "-21098088986959630");
    CHECK_FD(4.940656E-318, "4.940656E-318");
    CHECK_FD(1.18575755E-316, "1.18575755E-316");
    CHECK_FD(2.989102097996E-312, "2.989102097996E-312");
    CHECK_FD(9.0608011534336E15, "9060801153433600");
    CHECK_FD(4.708356024711512E18, "4708356024711512000");
    CHECK_FD(9.409340012568248E18, "9409340012568248000");
    CHECK_FD(1.2345678, "1.2345678");
}

TEST_CASE( "Looks like pow5", "[floating-decimal]" ) {
    // These numbers have a mantissa that is a multiple of the largest power of 5 that fits,
    // and an exponent that causes the computation for q to result in 22, which is a corner
    // case for Ryu.
    CHECK_FD(SPR_BIT_CAST(double, 0x4830F0CF064DD592), "5.764607523034235E39");
    CHECK_FD(SPR_BIT_CAST(double, 0x4840F0CF064DD592), "1.152921504606847E40");
    CHECK_FD(SPR_BIT_CAST(double, 0x4850F0CF064DD592), "2.305843009213694E40");
}

TEST_CASE( "Output length", "[floating-decimal]" ) {
    CHECK_FD(1., "1"); // already tested in Basic
    CHECK_FD(1.2, "1.2");
    CHECK_FD(1.23, "1.23");
    CHECK_FD(1.234, "1.234");
    CHECK_FD(1.2345, "1.2345");
    CHECK_FD(1.23456, "1.23456");
    CHECK_FD(1.234567, "1.234567");
    CHECK_FD(1.2345678, "1.2345678"); // already tested in Regression
    CHECK_FD(1.23456789, "1.23456789");
    CHECK_FD(1.234567895, "1.234567895"); // 1.234567890 would be trimmed
    CHECK_FD(1.2345678901, "1.2345678901");
    CHECK_FD(1.23456789012, "1.23456789012");
    CHECK_FD(1.234567890123, "1.234567890123");
    CHECK_FD(1.2345678901234, "1.2345678901234");
    CHECK_FD(1.23456789012345, "1.23456789012345");
    CHECK_FD(1.234567890123456, "1.234567890123456");
    CHECK_FD(1.2345678901234567, "1.2345678901234567");

    // Test 32-bit chunking
    CHECK_FD(4.294967294, "4.294967294"); // 2^32 - 2
    CHECK_FD(4.294967295, "4.294967295"); // 2^32 - 1
    CHECK_FD(4.294967296, "4.294967296"); // 2^32
    CHECK_FD(4.294967297, "4.294967297"); // 2^32 + 1
    CHECK_FD(4.294967298, "4.294967298"); // 2^32 + 2
}

// Test min, max shift values in shiftright128
TEST_CASE( "Min/max shift", "[floating-decimal]" ) {
    const uint64_t maxMantissa = ((uint64_t)1 << 53) - 1;

    // 32-bit opt-size=0:  49 <= dist <= 50
    // 32-bit opt-size=1:  30 <= dist <= 50
    // 64-bit opt-size=0:  50 <= dist <= 50
    // 64-bit opt-size=1:  30 <= dist <= 50
    CHECK_NFD((ieeeParts2Double<false, 4, 0>()), "1.7800590868057611E-307");
    // 32-bit opt-size=0:  49 <= dist <= 49
    // 32-bit opt-size=1:  28 <= dist <= 49
    // 64-bit opt-size=0:  50 <= dist <= 50
    // 64-bit opt-size=1:  28 <= dist <= 50
    CHECK_NFD((ieeeParts2Double<false, 6, maxMantissa>()), "2.8480945388892175E-306");
    // 32-bit opt-size=0:  52 <= dist <= 53
    // 32-bit opt-size=1:   2 <= dist <= 53
    // 64-bit opt-size=0:  53 <= dist <= 53
    // 64-bit opt-size=1:   2 <= dist <= 53
    CHECK_NFD((ieeeParts2Double<false, 41, 0>()), "2.446494580089078E-296");
    // 32-bit opt-size=0:  52 <= dist <= 52
    // 32-bit opt-size=1:   2 <= dist <= 52
    // 64-bit opt-size=0:  53 <= dist <= 53
    // 64-bit opt-size=1:   2 <= dist <= 53
    CHECK_NFD((ieeeParts2Double<false, 40, maxMantissa>()), "4.8929891601781557E-296");

    // 32-bit opt-size=0:  57 <= dist <= 58
    // 32-bit opt-size=1:  57 <= dist <= 58
    // 64-bit opt-size=0:  58 <= dist <= 58
    // 64-bit opt-size=1:  58 <= dist <= 58
    CHECK_NFD((ieeeParts2Double<false, 1077, 0>()), "18014398509481984");
    // 32-bit opt-size=0:  57 <= dist <= 57
    // 32-bit opt-size=1:  57 <= dist <= 57
    // 64-bit opt-size=0:  58 <= dist <= 58
    // 64-bit opt-size=1:  58 <= dist <= 58
    CHECK_NFD((ieeeParts2Double<false, 1076, maxMantissa>()), "36028797018963964");
    // 32-bit opt-size=0:  51 <= dist <= 52
    // 32-bit opt-size=1:  51 <= dist <= 59
    // 64-bit opt-size=0:  52 <= dist <= 52
    // 64-bit opt-size=1:  52 <= dist <= 59
    CHECK_NFD((ieeeParts2Double<false, 307, 0>()), "2.900835519859558E-216");
    // 32-bit opt-size=0:  51 <= dist <= 51
    // 32-bit opt-size=1:  51 <= dist <= 59
    // 64-bit opt-size=0:  52 <= dist <= 52
    // 64-bit opt-size=1:  52 <= dist <= 59
    CHECK_NFD((ieeeParts2Double<false, 306, maxMantissa>()), "5.801671039719115E-216");

    // https://github.com/ulfjack/ryu/commit/19e44d16d80236f5de25800f56d82606d1be00b9#commitcomment-30146483
    // 32-bit opt-size=0:  49 <= dist <= 49
    // 32-bit opt-size=1:  44 <= dist <= 49
    // 64-bit opt-size=0:  50 <= dist <= 50
    // 64-bit opt-size=1:  44 <= dist <= 50
    CHECK_NFD((ieeeParts2Double<false, 934, 0x000FA7161A4D6E0Cu>()), "3.196104012172126E-27");
}

TEST_CASE( "Small integers", "[floating-decimal]" ) {
    CHECK_FD(9007199254740991.0, "9007199254740991"); // 2^53-1
    CHECK_FD(9007199254740992.0, "9007199254740992"); // 2^53

    CHECK_FD(               1.0e+0,                "1");
    CHECK_FD(               1.2e+1,               "12");
    CHECK_FD(              1.23e+2,              "123");
    CHECK_FD(             1.234e+3,             "1234");
    CHECK_FD(            1.2345e+4,            "12345");
    CHECK_FD(           1.23456e+5,           "123456");
    CHECK_FD(          1.234567e+6,          "1234567");
    CHECK_FD(         1.2345678e+7,         "12345678");
    CHECK_FD(        1.23456789e+8,        "123456789");
    CHECK_FD(        1.23456789e+9,       "1234567890");
    CHECK_FD(       1.234567895e+9,       "1234567895");
    CHECK_FD(     1.2345678901e+10,      "12345678901");
    CHECK_FD(    1.23456789012e+11,     "123456789012");
    CHECK_FD(   1.234567890123e+12,    "1234567890123");
    CHECK_FD(  1.2345678901234e+13,   "12345678901234");
    CHECK_FD( 1.23456789012345e+14,  "123456789012345");
    CHECK_FD(1.234567890123456e+15, "1234567890123456");

    // 10^i
    CHECK_FD(1.0e+0,  "1");
    CHECK_FD(1.0e+1,  "10");
    CHECK_FD(1.0e+2,  "100");
    CHECK_FD(1.0e+3,  "1000");
    CHECK_FD(1.0e+4,  "10000");
    CHECK_FD(1.0e+5,  "100000");
    CHECK_FD(1.0e+6,  "1000000");
    CHECK_FD(1.0e+7,  "10000000");
    CHECK_FD(1.0e+8,  "100000000");
    CHECK_FD(1.0e+9,  "1000000000");
    CHECK_FD(1.0e+10, "10000000000");
    CHECK_FD(1.0e+11, "100000000000");
    CHECK_FD(1.0e+12, "1000000000000");
    CHECK_FD(1.0e+13, "10000000000000");
    CHECK_FD(1.0e+14, "100000000000000");
    CHECK_FD(1.0e+15, "1000000000000000");

    // 10^15 + 10^i
    CHECK_FD(1.0e+15 + 1.0e+0,  "1000000000000001");
    CHECK_FD(1.0e+15 + 1.0e+1,  "1000000000000010");
    CHECK_FD(1.0e+15 + 1.0e+2,  "1000000000000100");
    CHECK_FD(1.0e+15 + 1.0e+3,  "1000000000001000");
    CHECK_FD(1.0e+15 + 1.0e+4,  "1000000000010000");
    CHECK_FD(1.0e+15 + 1.0e+5,  "1000000000100000");
    CHECK_FD(1.0e+15 + 1.0e+6,  "1000000001000000");
    CHECK_FD(1.0e+15 + 1.0e+7,  "1000000010000000");
    CHECK_FD(1.0e+15 + 1.0e+8,  "1000000100000000");
    CHECK_FD(1.0e+15 + 1.0e+9,  "1000001000000000");
    CHECK_FD(1.0e+15 + 1.0e+10, "1000010000000000");
    CHECK_FD(1.0e+15 + 1.0e+11, "1000100000000000");
    CHECK_FD(1.0e+15 + 1.0e+12, "1001000000000000");
    CHECK_FD(1.0e+15 + 1.0e+13, "1010000000000000");
    CHECK_FD(1.0e+15 + 1.0e+14, "1100000000000000");

    // Largest power of 2 <= 10^(i+1)
    CHECK_FD(               8.0,                "8");
    CHECK_FD(              64.0,               "64");
    CHECK_FD(             512.0,              "512");
    CHECK_FD(            8192.0,             "8192");
    CHECK_FD(           65536.0,            "65536");
    CHECK_FD(          524288.0,           "524288");
    CHECK_FD(         8388608.0,          "8388608");
    CHECK_FD(        67108864.0,         "67108864");
    CHECK_FD(       536870912.0,        "536870912");
    CHECK_FD(      8589934592.0,       "8589934592");
    CHECK_FD(     68719476736.0,      "68719476736");
    CHECK_FD(    549755813888.0,     "549755813888");
    CHECK_FD(   8796093022208.0,    "8796093022208");
    CHECK_FD(  70368744177664.0,   "70368744177664");
    CHECK_FD( 562949953421312.0,  "562949953421312");
    CHECK_FD(9007199254740992.0, "9007199254740992");

    // 1000 * (Largest power of 2 <= 10^(i+1))
    CHECK_FD(            8.0e+3,             "8000");
    CHECK_FD(           64.0e+3,            "64000");
    CHECK_FD(          512.0e+3,           "512000");
    CHECK_FD(         8192.0e+3,          "8192000");
    CHECK_FD(        65536.0e+3,         "65536000");
    CHECK_FD(       524288.0e+3,        "524288000");
    CHECK_FD(      8388608.0e+3,       "8388608000");
    CHECK_FD(     67108864.0e+3,      "67108864000");
    CHECK_FD(    536870912.0e+3,     "536870912000");
    CHECK_FD(   8589934592.0e+3,    "8589934592000");
    CHECK_FD(  68719476736.0e+3,   "68719476736000");
    CHECK_FD( 549755813888.0e+3,  "549755813888000");
    CHECK_FD(8796093022208.0e+3, "8796093022208000");
}

