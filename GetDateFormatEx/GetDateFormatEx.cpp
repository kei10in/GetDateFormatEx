#include <Windows.h>

#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <functional>

#include <boost/lexical_cast.hpp>

#include "GetDateFormatEx.h"


inline const wchar_t* wcschr_not(const wchar_t* str, wchar_t ch)
{
    while (*str && *str == ch) { ++str; }
    return str;
}


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


BOOL CALLBACK EnumCalendarInfoProcExEx(
    LPWSTR lpCalendarInfoString,
    CALID Calendar,
    LPWSTR lpReserved,
    LPARAM lParam)
{
    typedef std::function<bool (LPWSTR, CALID, LPWSTR)> FuncT;
    auto proc = reinterpret_cast<FuncT*>(lParam);
    return BOOL((*proc)(lpCalendarInfoString, Calendar, lpReserved));
}

bool EnumCalendarInfoExEx(
    std::function<bool (LPWSTR, CALID, LPWSTR)> proc,
    LPCWSTR lpLocaleName,
    CALID Calendar,
    LPCWSTR lpReserved,
    CALTYPE CalType)
{
    return ::EnumCalendarInfoExEx(
        detail_::EnumCalendarInfoProcExEx,
        lpLocaleName,
        Calendar,
        lpReserved,
        CalType,
        reinterpret_cast<LPARAM>(&proc)) ? true : false;
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


std::wstring FormatDatePicture(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    SYSTEMTIME const* lpDate,
    LPCWSTR lpFormat)
{
    std::vector<std::wstring> eras;
    detail_::EnumCalendarInfoExEx(
        [&](LPWSTR lpCalendarInfoString, CALID Calendar, LPWSTR) -> bool {
            eras.push_back(lpCalendarInfoString);
            return true;
        },
        lpLocaleName, CalendarID, 0, CAL_SERASTRING);

    if (*lpFormat == L'y') {
        LPCWSTR lpNext = wcschr_not(lpFormat, L'y');
        size_t count = std::distance(lpFormat, lpNext);
        if (count >= 3) {
            return boost::lexical_cast<std::wstring>(lpDate->wYear);
        } else if (count == 2) {
            std::wstringstream ss;
            ss << std::right << std::setw(2) << std::setfill(L'0')
               << lpDate->wYear % 100;
            return ss.str();
        } else {
            return boost::lexical_cast<std::wstring>(lpDate->wYear % 100);
        }
    } else if (wcscmp(lpFormat, L"MMMM") == 0) {
        CALTYPE CalType(detail_::MonthNameCalTypes[lpDate->wMonth - 1]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"MMM") == 0) {
        CALTYPE CalType(detail_::AbbrevMonthNameCalTypes[lpDate->wMonth - 1]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"MM") == 0) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0')
           << lpDate->wMonth;
        return ss.str();
    } else if (*lpFormat == L'M') {
        return boost::lexical_cast<std::wstring>(lpDate->wMonth);
    } else if (wcscmp(lpFormat, L"dddd") == 0) {
        CALTYPE CalType(detail_::DayOfWeekCalTypes[lpDate->wDayOfWeek]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"ddd") == 0) {
        CALTYPE CalType(detail_::AbbrevDayOfWeekCalTypes[lpDate->wDayOfWeek]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    } else if (wcscmp(lpFormat, L"dd") == 0) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0') << lpDate->wDay;
        return ss.str();
    } else if (*lpFormat == L'd') {
        return boost::lexical_cast<std::wstring>(lpDate->wDay);
    } else if (*lpFormat == L'g' || wcscmp(lpFormat, L"gg") == 0) {
        return eras[0];
    }
    return L"";
}

}


bool IsPictureChar(wchar_t wc)
{
    return wc == L'y' || wc == L'M' || wc == L'd' || wc == L'g';
}

bool IsQuote(wchar_t wc)
{
    return wc == L'\'';
}


std::wstring GetDateFormatEx(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    SYSTEMTIME const* lpDate,
    LPCWSTR lpFormat)
{
    SYSTEMTIME st(*lpDate);
    detail_::ValidateSystemTime(&st);

    LPCWSTR lpPos(lpFormat);

    std::wstringstream ss;
    while (*lpPos) {
        if (IsPictureChar(*lpPos)) {
            ss << detail_::FormatDatePicture(
                lpLocaleName, CalendarID, lpDate, lpPos);
            lpPos = wcschr_not(lpPos, *lpPos);
        } else if (IsQuote(*lpPos)) {
            ++lpPos;
            for (; *lpPos != L'\0'; ++lpPos) {
                if (IsQuote(*lpPos)) {
                    ++lpPos;
                    if (IsQuote(*lpPos)) {
                        ss << *lpPos;
                    } else {
                        break;
                    }
                } else {
                    ss << *lpPos;
                }
            }
        } else {
            ss << *lpPos;
            ++lpPos;
        }
    }
    return ss.str();
}


