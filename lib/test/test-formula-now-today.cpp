#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Direct arguments", "[formula][formula-now-today]" ) {

    Sheet s;
    
    auto now = time(nullptr);
    tm * today = localtime(&now);
    auto currentYear = today->tm_year + 1900;
    auto currentMonth = today->tm_mon + 1;
    auto currentDay = today->tm_mday;
    auto currentHour = today->tm_hour;
    auto currentMin = today->tm_min;
    auto currentSec = today->tm_sec;
    
    s.setFormulaCell(PT("A1"), "TODAY()");
    CHECK_DATE(s.getValue(PT("A1")), currentYear, currentMonth, currentDay);
    std::cout << "Today: " << s.getValue(PT("A1")) << '\n';
    
    s.setFormulaCell(PT("A1"), "NOW()");
    CHECK_DATE_TIME(s.getValue(PT("A1")), currentYear, currentMonth, currentDay, currentHour, currentMin, currentSec);
    std::cout << "Now: " << s.getValue(PT("A1")) << '\n';
}
