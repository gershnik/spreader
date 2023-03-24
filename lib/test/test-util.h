#ifndef SPR_HEADER_TEST_UTIL_H_INCLUDED
#define SPR_HEADER_TEST_UTIL_H_INCLUDED

#include <optional>
#include <variant>
#include <ostream>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

template<class T>
inline auto operator<<(std::ostream & str, const std::optional<T> & opt) -> std::ostream & {
    str << '{';
    if (opt)
        str << *opt;
    else
        str << "nullopt";
    return str << '}';
}

template<class T>
inline auto operator<<(std::ostream & str, const std::monostate &) -> std::ostream & {
    return str << "<blank>";
}

#define PT(x) s.parsePoint(SPRS(x)).value()
#define ROW(r) ((r) - 1)
#define COLUMN(n) s.parseColumn(SPRS(n)).value()
#define AREA(x) s.parseArea(SPRS(x)).value()


class NumericalMatcher : public Catch::Matchers::MatcherGenericBase {
public:
    NumericalMatcher(double expected):m_expected(expected) {}
    
    auto match(double other) const noexcept -> bool {
        if (std::isnan(m_expected))
            return std::isnan(other);
        if (std::isnan(other))
            return false;
        
        //this should work for infs, normal and subnormal
        int lhsExp, rhsExp;
        double lhsMant = std::frexp(m_expected, &lhsExp);
        double rhsMant = std::frexp(other, &rhsExp);
        lhsMant = std::round(lhsMant * 1e15) / 1e15;
        rhsMant = std::round(rhsMant * 1e15) / 1e15;
        double lhs = std::ldexp(lhsMant, lhsExp);
        double rhs = std::ldexp(rhsMant, rhsExp);
        return lhs == rhs;
    }
    
    auto describe() const -> std::string override {
        return "Numerically equals: " + Catch::StringMaker<double>::convert(m_expected);
    }

private:
    double m_expected;
};

inline auto NumericallyEqualsTo(double expected) noexcept -> NumericalMatcher {
    return NumericalMatcher(expected);
}

#if defined(_MSC_VER)

    #define CHECK_DATE(val, y,m,d) { \
        auto numeric = get<Number>(val); \
        auto dayCount = int(numeric.value()); \
        CHECK(dayCount == numeric.value()); \
        SYSTEMTIME st; \
        CHECK(VariantTimeToSystemTime(dayCount, &st)); \
        CHECK(st.wYear == y); \
        CHECK(st.wMonth == m); \
        CHECK(st.wDay == d); \
        CHECK(st.wHour == 0); \
        CHECK(st.wMinute == 0); \
        CHECK(st.wSecond == 0); \
    }

    #define CHECK_DATE_TIME(val, y,m,d,h,mn,s) { \
        auto numeric = get<Number>(val); \
        SYSTEMTIME st; \
        CHECK(VariantTimeToSystemTime(numeric.value(), &st)); \
        CHECK(st.wYear == y); \
        CHECK(st.wMonth == m); \
        CHECK(st.wDay == d); \
        CHECK(st.wHour == h); \
        CHECK(st.wMinute == mn); \
        CHECK(st.wSecond == s); \
    }

#else 

    #define CHECK_DATE(val, y,m,d) { \
        auto numeric = get<Number>(val); \
        auto dayCount = int(numeric.value()); \
        CHECK(dayCount == numeric.value()); \
        tm temp{}; \
        temp.tm_mday = dayCount - 1; \
        mktime(&temp); \
        CHECK(temp.tm_year == y - 1900); \
        CHECK(temp.tm_mon == m - 1); \
        CHECK(temp.tm_mday == d); \
    }

    #define CHECK_DATE_TIME(val, y,m,d,h,mn,s) { \
        auto numeric = get<Number>(val); \
        auto dayCount = int(numeric.value()); \
        auto timeFrac = numeric.value() - dayCount; \
        tm temp{}; \
        temp.tm_mday = dayCount - 1; \
        temp.tm_sec = int(round(timeFrac * (24 * 60 * 60))); \
        temp.tm_isdst = -1; \
        mktime(&temp); \
        CHECK(temp.tm_year == y - 1900); \
        CHECK(temp.tm_mon == m - 1); \
        CHECK(temp.tm_mday == d); \
        CHECK(temp.tm_hour == h); \
        CHECK(temp.tm_min == mn); \
        CHECK(temp.tm_sec == s); \
    }
#endif

#endif
