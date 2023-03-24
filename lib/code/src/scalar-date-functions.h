// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_DATE_FUNCTIONS_H_INCLUDED
#define SPR_HEADER_SCALAR_DATE_FUNCTIONS_H_INCLUDED

#include <spreader/number.h>
#include "scalar-function.h"
#include "mini-trie.h"
#include "macro-map.h"

#include <chrono>

namespace Spreader {

    struct DateFunctions {
        
        using DateDuration = std::chrono::days;
        using DateTimeDuration = std::chrono::duration<double, std::chrono::days::period>;
        
        static constexpr auto s_base = std::chrono::local_days(std::chrono::year(1899)/std::chrono::December/std::chrono::day(30));
        static constexpr auto s_maxDate = std::chrono::local_days(std::chrono::year(10000)/std::chrono::January/std::chrono::day(1));
        static constexpr auto s_maxValue = (s_maxDate - s_base).count();

        struct Now {
            auto operator()() const noexcept -> Number {
                using namespace std::chrono;
                
            #if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
                
                auto now = time(nullptr);
                tm localTime;
                localtime_r(&now, &localTime);
                
                auto today = local_days(year_month_day(year(localTime.tm_year + 1900),
                                                       month(localTime.tm_mon + 1),
                                                       day(localTime.tm_mday)));
                
                
                auto tod = makeTimeOfDay(localTime.tm_hour, localTime.tm_min, localTime.tm_sec);

            #elif defined (_MSC_VER)

                SYSTEMTIME localTime;
                GetLocalTime(&localTime);

                auto today = local_days(year_month_day(year(localTime.wYear),
                                                       month(localTime.wMonth),
                                                       day(localTime.wDay)));

                auto tod = makeTimeOfDay(localTime.wHour, localTime.wMinute, localTime.wSecond);

            #endif
                
                const auto diff = today - s_base;
                return diff.count() + tod;
            };
            
            static auto makeTimeOfDay(unsigned hour, unsigned minute, unsigned second) -> double {
                return (double(hour) * 3600 + double(minute) * 60 + double(second)) / (24 * 60 * 60);
            }
        };
        
        struct Today {

            auto operator()() const noexcept -> Number {
                using namespace std::chrono;
                
            #if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
                
                auto now = time(nullptr);
                tm localTime;
                localtime_r(&now, &localTime);
                
                auto today = local_days(year_month_day(year(localTime.tm_year + 1900),
                                                       month(localTime.tm_mon + 1),
                                                       day(localTime.tm_mday)));
                
                
            #elif defined (_MSC_VER)

                SYSTEMTIME localTime;
                GetLocalTime(&localTime);

                auto today = local_days(year_month_day(year(localTime.wYear),
                                                       month(localTime.wMonth),
                                                       day(localTime.wDay)));

            #endif
                
                auto diff = today - s_base;
                return diff.count();
            }
        };
        
        struct Time {
            auto operator()(Number hour, Number minute, Number second) const noexcept -> Scalar {
                auto truncHour = std::trunc(hour.value());
                if (truncHour < -32767 || truncHour > 32767)
                    return Error::NotANumber;
                auto truncMinute = std::trunc(minute.value());
                if (truncMinute < -32767 || truncMinute > 32767)
                    return Error::NotANumber;
                auto truncSecond = std::trunc(second.value());
                if (truncSecond < -32767 || truncSecond > 32767)
                    return Error::NotANumber;
                
                auto totalSeconds = truncHour * (60 * 60) + truncMinute * 60 + truncSecond;
                if (totalSeconds < 0)
                    return Error::NotANumber;
                double days;
                auto fraction = std::modf(totalSeconds / (24 * 60 * 60), &days);
                return Number(fraction);
            }
        };
        
        struct Date {
            auto operator()(Number yearArg, Number monthArg, Number dayArg) const noexcept -> Scalar {
                
                using namespace std::chrono;
                
                auto truncYear = std::trunc(yearArg.value());
                if (truncYear < -32767 || truncYear > 32767)
                    return Error::NotANumber;
                auto truncMonth = std::trunc(monthArg.value()) - 1;
                if (truncMonth < -32767 || truncMonth > 32767)
                    return Error::NotANumber;
                auto truncDay = std::trunc(dayArg.value()) - 1;
                if (truncDay < -32767 || truncDay > 32767)
                    return Error::NotANumber;
                
                truncYear = std::floor((truncYear * 12 + truncMonth) / 12);
                if (truncYear < 0 || truncYear >= 10000)
                    return Error::NotANumber;
                if (truncYear < 1900)
                    truncYear += 1900;
                truncMonth = Numeric::flooredModulo(truncMonth, 12);
                
                year_month_day start(year(int(truncYear))/month(unsigned(truncMonth + 1))/1d);
                auto result = double((local_days(start) - s_base).count());
                result += truncDay;
                if (result < 0 || result > s_maxValue)
                    return Error::NotANumber;
                
                return Number(result);
            }
        };
        
        struct Year {
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                using namespace std::chrono;
                return parseDate(dateSerial, [](const year_month_day & ymd) { return Number(int(ymd.year())); });
            };
        };
        struct Month {
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                using namespace std::chrono;
                return parseDate(dateSerial, [](const year_month_day & ymd) { return Number(unsigned(ymd.month())); });
            };
        };
        struct Day {
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                using namespace std::chrono;
                return parseDate(dateSerial, [](const year_month_day & ymd) { return Number(unsigned(ymd.day())); });
            };
        };
        
        struct Hour {
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                return parseTime(dateSerial, [](double timePart) {
                    return Number(std::round(timePart * 24));
                });
            };
        };
        struct Minute {
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                return parseTime(dateSerial, [](double timePart) {
                    return Number(unsigned(std::round(timePart * 24 * 60)) % 60);
                });
            };
        };
        struct Second {
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                return parseTime(dateSerial, [](double timePart) {
                    return Number(unsigned(std::round(timePart * 24 * 60 * 60)) % 60);
                });
            };
        };
        
        struct Days {
            auto operator()(Number endSerial, Number startSerial) const noexcept -> Scalar {
                if (startSerial.value() < 0 || startSerial > s_maxValue)
                    return Error::NotANumber;
                if (endSerial.value() < 0 || endSerial > s_maxValue)
                    return Error::NotANumber;
                
                return Number(int(endSerial.value()) - int(startSerial.value()));
            }
        };
        
        struct DateDif {
            
            #define SPR_DATE_DIFF_LITERALS "Y", "M", "D", "MD", "YM", "YD"
            #define SPR_DATE_DIFF_LITERALS_U16 SPR_MAP_LIST(SPR_U16, SPR_DATE_DIFF_LITERALS)
            #define SPR_DATE_DIFF_LITERALS_U32 SPR_MAP_LIST(SPR_U32, SPR_DATE_DIFF_LITERALS)

            template<class Char=String::storage_type> 
            static auto typesMatcher() -> const MiniTrie<Char> & {

                if constexpr (std::is_same_v<Char, char>) {
                    static MiniTrie<char> theInstance = {SPR_DATE_DIFF_LITERALS};
                    return theInstance;
                } else if constexpr (std::is_same_v<Char, char16_t>) {
                    static MiniTrie<char16_t> theInstance = {SPR_DATE_DIFF_LITERALS_U16};
                    return theInstance;
                } else if constexpr (std::is_same_v<Char, char32_t>) {
                    static MiniTrie<char32_t> theInstance = {SPR_DATE_DIFF_LITERALS_U32};
                    return theInstance;
                }
            }
            
    
            auto operator()(Number startSerial, Number endSerial, const String & type) const noexcept -> Scalar {
                using namespace std::chrono;
                
                if (startSerial.value() < 0 || startSerial > s_maxValue)
                    return Error::NotANumber;
                if (endSerial.value() < 0 || endSerial > s_maxValue)
                    return Error::NotANumber;
                if (startSerial > endSerial)
                    return Error::NotANumber;
                
                auto daysStart = days(unsigned(startSerial.value()));
                auto daysEnd = days(unsigned(endSerial.value()));
                
                auto typeLower = type.to_upper();
                String::char_access typeAccess(type);
                auto cur = typeAccess.begin();
                auto end = typeAccess.end();
                auto match = typesMatcher().prefixMatch(cur, end);
                if (cur != end)
                    return Error::NotANumber;
                switch(match) {
                    case 0: return doY(daysStart, daysEnd);
                    case 1: return doM(daysStart, daysEnd);
                    case 2: return Number((daysEnd - daysStart).count());
                    case 3: return doMD(daysStart, daysEnd);
                    case 4: return doYM(daysStart, daysEnd);
                    case 5: return doYD(daysStart, daysEnd);
                }
                return Error::NotANumber;
            }
                
            static auto doY(std::chrono::days daysStart, std::chrono::days daysEnd) -> Number {
                using namespace std::chrono;
                
                year_month_day ymdStart(s_base + daysStart);
                year_month_day ymdEnd(s_base + daysEnd);
                
                int yearDiff = int(ymdEnd.year()) - int(ymdStart.year());
                if (ymdStart.month() != ymdEnd.month())
                    yearDiff -= (ymdStart.month() > ymdEnd.month());
                else
                    yearDiff -= (ymdStart.day() > ymdEnd.day());
                return Number(yearDiff);
            }
                
            static auto doM(std::chrono::days daysStart, std::chrono::days daysEnd) -> Number {
                using namespace std::chrono;
                
                year_month_day ymdStart(s_base + daysStart);
                year_month_day ymdEnd(s_base + daysEnd);
                
                int monthDiff = (int(ymdEnd.year()) - int(ymdStart.year())) * 12;
                if (ymdStart.month() < ymdEnd.month())
                    monthDiff += unsigned(ymdEnd.month()) - unsigned(ymdStart.month());
                else if (ymdStart.month() > ymdEnd.month())
                    monthDiff += unsigned(ymdStart.month()) - unsigned(ymdEnd.month());
                monthDiff -= (ymdStart.day() > ymdEnd.day()); //yes this is wrong if months have different # of days but Excel does it
                return Number(monthDiff);
            }
            
            static auto doYM(std::chrono::days daysStart, std::chrono::days daysEnd) -> Number {
                using namespace std::chrono;
                
                year_month_day ymdStart(s_base + daysStart);
                year_month_day ymdEnd(s_base + daysEnd);
                
                unsigned result;
                if (ymdStart.month() <= ymdEnd.month())
                    result = unsigned(ymdEnd.month()) - unsigned(ymdStart.month());
                else
                    result = 12 - (unsigned(ymdStart.month()) - unsigned(ymdEnd.month()));
                result -= (ymdStart.day() > ymdEnd.day());
                return Number(result);
            }
                
            static auto doMD(std::chrono::days daysStart, std::chrono::days daysEnd) -> Number {
                using namespace std::chrono;
                
                year_month_day ymdStart(s_base + daysStart);
                year_month_day ymdEnd(s_base + daysEnd);
                
                if (ymdStart.day() <= ymdEnd.day())
                    return Number(unsigned(ymdEnd.day()) - unsigned(ymdStart.day()));
                
                auto prevEnd = ymdEnd.year()/--ymdEnd.month()/last;
                if (prevEnd.day() >= ymdStart.day())
                    return Number(unsigned(prevEnd.day()) - unsigned(ymdStart.day()) + unsigned(ymdEnd.day()));
                
                return Number(unsigned(ymdEnd.day()) - (unsigned(ymdStart.day()) - unsigned(prevEnd.day())));
            }
            
            static auto doYD(std::chrono::days daysStart, std::chrono::days daysEnd) -> Number {
                using namespace std::chrono;
                
                year_month_day ymdStart(s_base + daysStart);
                year_month_day ymdEnd(s_base + daysEnd);
                
                if (ymdStart.month() <= ymdEnd.month())
                    ymdStart = ymdEnd.year() / ymdStart.month() / ymdStart.day();
                else
                    ymdStart = (ymdEnd.year() - years(1)) / ymdStart.month() / ymdStart.day();
                return Number((local_days(ymdEnd) - local_days(ymdStart)).count());
            }
        };
        
        struct EDate {
            auto operator()(Number startSerial, Number monthCount) const noexcept -> Scalar {
                using namespace std::chrono;
                
                return calcEndDate(startSerial, monthCount, [](const year_month_day & startYmd, const year_month_day & endYmd) {
                    auto result = (local_days(endYmd) - s_base).count();
                    
                    if (endYmd.day() > startYmd.day())
                        result -= (unsigned(endYmd.day()) - unsigned(startYmd.day()));
                    
                    return Number(result);
                });
            }
        };
        
        struct EOMonth {
            auto operator()(Number startSerial, Number monthCount) const noexcept -> Scalar {
                using namespace std::chrono;
                
                return calcEndDate(startSerial, monthCount, [](const year_month_day & /*startYmd*/, const year_month_day & endYmd) {
                    auto result = (local_days(endYmd) - s_base).count();
                    return Number(result);
                });
            }
        };
        
        struct Weekday {
            auto operator()(Number dateSerial, Number returnType) const noexcept -> Scalar {
                
                using namespace std::chrono;
                
                if (dateSerial.value() < 0 || dateSerial > s_maxValue)
                    return Error::NotANumber;
                
                auto datePart = unsigned(dateSerial.value());
                auto localDays = s_base + days(datePart);
                weekday wd(localDays);
                
                if (auto type = int(returnType.value()); type == 1)
                    return wd.c_encoding() + 1;        //1 (Sunday) through 7 (Saturday)
                else if (type == 2)
                    return wd.iso_encoding();          //1 (Monday) through 7 (Sunday)
                else if (type == 3)
                    return (wd.c_encoding() + 6) % 7;  //0 (Monday) through 6 (Sunday).
                else if (type >= 11 && type <= 17)
                    return (wd.c_encoding() + (17 - type)) % 7 + 1;
                
                return Error::NotANumber;
            }
            
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                return operator()(dateSerial, 1);
            }
        };
        
        struct IsoWeekNum {
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                
                using namespace std::chrono;
                
                if (dateSerial.value() < 0 || dateSerial > s_maxValue)
                    return Error::NotANumber;
                
                //algorithm from https://github.com/HowardHinnant/date/blob/master/include/date/iso_week.h

                auto datePart = unsigned(dateSerial.value());
                auto localDays = s_base + days(datePart);
                
                weekday wd(localDays);
                auto y = year_month_day{localDays + days{3}}.year();
                auto start = local_days((y - years{1})/December/Thursday[last]) + (Monday-Thursday);
                if (localDays < start) {
                    --y;
                    start = local_days((y - years{1})/December/Thursday[last]) + (Monday-Thursday);
                }
                return Number(static_cast<unsigned>(floor<weeks>(localDays - start).count() + 1));
            }
        };
        
        struct WeekNum {
            auto operator()(Number dateSerial, Number returnType) const noexcept -> Scalar {
                
                using namespace std::chrono;
                
                auto type = int(returnType.value());
                if (type == 21)
                    return IsoWeekNum()(dateSerial);
                
                if (dateSerial.value() < 0 || dateSerial > s_maxValue)
                    return Error::NotANumber;
                
                auto datePart = unsigned(dateSerial.value());
                auto localDays = s_base + days(datePart);
                year_month_day ymd(localDays);
                auto localYearStart = local_days(ymd.year()/January/1d);
                weekday weekdayYearStart(localYearStart);
                auto dayOfYear = (localDays - localYearStart).count();
                
                days yearStartWeekOffset;
                if (type == 1)
                    yearStartWeekOffset = weekdayYearStart - Sunday;
                else if (type == 2)
                    yearStartWeekOffset = weekdayYearStart - Monday;
                else if (type >= 11 && type <= 17)
                    yearStartWeekOffset = weekdayYearStart - weekday((type - 10) % 7);
                else
                    return Error::NotANumber;
                
                auto res = (dayOfYear + yearStartWeekOffset.count()) / 7 + 1;
                return Number(res);
            }
            
            auto operator()(Number dateSerial) const noexcept -> Scalar {
                return operator()(dateSerial, 1);
            }
        };
        
    private:
        
        template<class Extractor>
        static auto parseDate(Number dateSerial, Extractor extractor) noexcept -> Scalar {
            using namespace std::chrono;
            
            if (dateSerial.value() < 0 || dateSerial > s_maxValue)
                return Error::NotANumber;
            
            auto datePart = unsigned(dateSerial.value());
            auto localDays = s_base + days(datePart);
            year_month_day ymd(localDays);
            return extractor(ymd);
        }
        
        template<class Extractor>
        static auto parseTime(Number dateSerial, Extractor extractor) noexcept -> Scalar {
            using namespace std::chrono;
            
            if (dateSerial.value() < 0 || dateSerial > s_maxValue)
                return Error::NotANumber;
            
            double days;
            auto timePart = std::modf(dateSerial.value(), &days);
            return extractor(timePart);
        }
        
        template<class Extractor>
        static auto calcEndDate(Number startSerial, Number monthCount, Extractor extractor) noexcept -> Scalar {
            using namespace std::chrono;
            
            if (startSerial.value() < 0 || startSerial > s_maxValue)
                return Error::NotANumber;
            
            auto datePart = unsigned(startSerial.value());
            auto localDays = s_base + days(datePart);
            year_month_day startYmd(localDays);
            
            double numYears = int(startYmd.year());
            double numMonths = (unsigned(startYmd.month()) - 1) + monthCount.value();
            
            numYears = std::floor((numYears * 12 + numMonths) / 12);
            if (numYears < 1900 || numYears >= 10000)
                return Error::NotANumber;
            numMonths = Numeric::flooredModulo(numMonths, 12);
            
            auto endYmd = year_month_day(year(int(numYears))/month(unsigned(numMonths + 1))/last);
            return extractor(startYmd, endYmd);
        }
    };
    
    using ScalarDate =       ScalarDetail::ScalarFunction<DateFunctions::Date,         Typelist<Number, Number, Number>>;
    using ScalarDateDif =    ScalarDetail::ScalarFunction<DateFunctions::DateDif,      Typelist<Number, Number, String>>;
    using ScalarDay =        ScalarDetail::ScalarFunction<DateFunctions::Day,          Typelist<Number>>;
    using ScalarDays =       ScalarDetail::ScalarFunction<DateFunctions::Days,         Typelist<Number, Number>>;
    using ScalarEDate =      ScalarDetail::ScalarFunction<DateFunctions::EDate,        Typelist<Number, Number>>;
    using ScalarEOMonth =    ScalarDetail::ScalarFunction<DateFunctions::EOMonth,      Typelist<Number, Number>>;
    using ScalarHour =       ScalarDetail::ScalarFunction<DateFunctions::Hour,         Typelist<Number>>;
    using ScalarIsoWeekNum = ScalarDetail::ScalarFunction<DateFunctions::IsoWeekNum,   Typelist<Number>>;
    using ScalarMinute =     ScalarDetail::ScalarFunction<DateFunctions::Minute,       Typelist<Number>>;
    using ScalarMonth =      ScalarDetail::ScalarFunction<DateFunctions::Month,        Typelist<Number>>;
    using ScalarNow =        ScalarDetail::ScalarFunction<DateFunctions::Now,          Typelist<>>;
    using ScalarSecond =     ScalarDetail::ScalarFunction<DateFunctions::Second,       Typelist<Number>>;
    using ScalarTime =       ScalarDetail::ScalarFunction<DateFunctions::Time,         Typelist<Number, Number, Number>>;
    using ScalarToday =      ScalarDetail::ScalarFunction<DateFunctions::Today,        Typelist<>>;
    using ScalarWeekday =    ScalarDetail::ScalarFunction<DateFunctions::Weekday,      Typelist<Number>, Typelist<Number>>;
    using ScalarWeekNum =    ScalarDetail::ScalarFunction<DateFunctions::WeekNum,      Typelist<Number>, Typelist<Number>>;
    using ScalarYear =       ScalarDetail::ScalarFunction<DateFunctions::Year,         Typelist<Number>>;
}

#endif
