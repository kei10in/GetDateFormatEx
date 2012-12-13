#include <Windows.h>

#include <vector>
#include <sstream>
#include <iomanip>

#include <boost/lexical_cast.hpp>

#include "GetDateFormatEx.h"

namespace detail_ {

std::wstring GetCalendarInfoEx(
    LPCWSTR lpLocaleName,
    CALID Calendar,
    LPCWSTR lpReserved,
    CALTYPE CalType,
    LPDWORD lpValue)
{
    int const req = ::GetCalendarInfoEx(
        lpLocaleName, Calendar, lpReserved, CalType, 0, 0, lpValue);
    if (req == 0) {
        throw GetLastError();
    }
    std::vector<WCHAR> buf(req);
    int const res = ::GetCalendarInfoEx(
        lpLocaleName, Calendar, lpReserved, CalType, &(buf[0]), req, lpValue);
    return std::wstring(&(buf[0]));
}


BOOL ValidateSystemTime(SYSTEMTIME* lpSystemTime)
{
    SYSTEMTIME SystemTime = {};
    SystemTime.wYear = lpSystemTime->wYear;
    SystemTime.wMonth = lpSystemTime->wMonth;
    SystemTime.wDay = lpSystemTime->wDay;
    // wDayOfWeek is ignored in SystemTimeToFileTime

    FILETIME FileTime = {};
    BOOL const stf_res = SystemTimeToFileTime(lpSystemTime, &FileTime);
    if (!stf_res) { return stf_res; }

    BOOL const fts_res = FileTimeToSystemTime(&FileTime, lpSystemTime);
    if (!fts_res) { return fts_res; }

    lpSystemTime->wYear = SystemTime.wYear ;
    lpSystemTime->wMonth = SystemTime.wMonth;
    lpSystemTime->wDay = SystemTime.wDay;
    lpSystemTime->wDayOfWeek = SystemTime.wDayOfWeek;

    return fts_res;
}

}


std::wstring GetDateFormatEx(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    SYSTEMTIME const* st,
    LPCWSTR lpFormat,
    LPCWSTR lpCalendar)
{
    int const req = GetDateFormatEx(
        lpLocaleName, dwFlags, st, lpFormat, 0, 0, lpCalendar);
    if (req == 0) {
        throw GetLastError();
    }
    std::vector<WCHAR> buf(req);
    int const res = GetDateFormatEx(
        lpLocaleName, dwFlags, st, lpFormat, &(buf[0]), req, lpCalendar);
    if (res == 0) {
        throw GetLastError();
    }
    return std::wstring(&(buf[0]));
}



namespace detail_ {

static CALTYPE const AbbrevMonthNameCalTypes[] = {
    CAL_SABBREVMONTHNAME1,
    CAL_SABBREVMONTHNAME2,
    CAL_SABBREVMONTHNAME3,
    CAL_SABBREVMONTHNAME4,
    CAL_SABBREVMONTHNAME5,
    CAL_SABBREVMONTHNAME6,
    CAL_SABBREVMONTHNAME7,
    CAL_SABBREVMONTHNAME8,
    CAL_SABBREVMONTHNAME9,
    CAL_SABBREVMONTHNAME10,
    CAL_SABBREVMONTHNAME11,
    CAL_SABBREVMONTHNAME12,
    CAL_SABBREVMONTHNAME13,
};

static CALTYPE const MonthNameCalTypes[] = {
    CAL_SMONTHNAME1,
    CAL_SMONTHNAME2,
    CAL_SMONTHNAME3,
    CAL_SMONTHNAME4,
    CAL_SMONTHNAME5,
    CAL_SMONTHNAME6,
    CAL_SMONTHNAME7,
    CAL_SMONTHNAME8,
    CAL_SMONTHNAME9,
    CAL_SMONTHNAME10,
    CAL_SMONTHNAME11,
    CAL_SMONTHNAME12,
    CAL_SMONTHNAME13,
};

static CALTYPE const AbbrevDayOfWeekCalTypes[] = {
    CAL_SABBREVDAYNAME7,  // Sunday
    CAL_SABBREVDAYNAME1,  // Monday
    CAL_SABBREVDAYNAME2,
    CAL_SABBREVDAYNAME3,
    CAL_SABBREVDAYNAME4,
    CAL_SABBREVDAYNAME5,
    CAL_SABBREVDAYNAME6,
};

static CALTYPE const DayOfWeekCalTypes[] = {
    CAL_SDAYNAME7,  // Sunday
    CAL_SDAYNAME1,  // Monday
    CAL_SDAYNAME2,
    CAL_SDAYNAME3,
    CAL_SDAYNAME4,
    CAL_SDAYNAME5,
    CAL_SDAYNAME6,
};

}


std::wstring GetDateFormatEx(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    SYSTEMTIME const* lpSystemTime,
    LPCWSTR lpFormat)
{
    SYSTEMTIME st(*lpSystemTime);
    detail_::ValidateSystemTime(&st);
    
    if (wcscmp(lpFormat, L"yyyy") == 0) {
        return boost::lexical_cast<std::wstring>(st.wYear);
    } else if (wcscmp(lpFormat, L"yy") == 0) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0')
           << st.wYear % 100;
        return ss.str();
    } else if (*lpFormat == L'y') {
        return boost::lexical_cast<std::wstring>(st.wYear % 100);
    } else if (wcscmp(lpFormat, L"MMMM") == 0) {
        CALTYPE CalType(detail_::MonthNameCalTypes[st.wMonth - 1]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"MMM") == 0) {
        CALTYPE CalType(detail_::AbbrevMonthNameCalTypes[st.wMonth - 1]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"MM") == 0) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0') << st.wMonth;
        return ss.str();
    } else if (*lpFormat == L'M') {
        return boost::lexical_cast<std::wstring>(st.wMonth);
    } else if (wcscmp(lpFormat, L"dddd") == 0) {
        CALTYPE CalType(detail_::DayOfWeekCalTypes[st.wDayOfWeek]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"ddd") == 0) {
        CALTYPE CalType(detail_::AbbrevDayOfWeekCalTypes[st.wDayOfWeek]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"dd") == 0) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0') << st.wDay;
        return ss.str();
    } else if (*lpFormat == L'd') {
        return boost::lexical_cast<std::wstring>(st.wDay);
    }
    return L"";
}
