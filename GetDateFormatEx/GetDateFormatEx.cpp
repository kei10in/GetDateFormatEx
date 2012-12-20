#include <Windows.h>

#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <functional>

#include "GetDateFormatEx.h"


template <typename T1, typename T2>
T1 lexical_cast(T2 const& arg) {
    std::wstringstream wss;
    wss << arg;
    T1 value;
    wss >> value;
    return value;
}

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


std::wstring FormatYearPicture(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    CalendarDate const* lpCalDate,
    size_t repeat)
{
    if (repeat >= 3) {
        return lexical_cast<std::wstring>(lpCalDate->wYear);
    }

    if (repeat == 2) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0')
           << lpCalDate->wYear % 100;
        return ss.str();
    }

    return lexical_cast<std::wstring>(lpCalDate->wYear % 100);
}

std::wstring FormatMonthPicture(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    CalendarDate const* lpCalDate,
    size_t repeat)
{
    if (repeat >= 4) {
        CALTYPE CalType(detail_::MonthNameCalTypes[lpCalDate->wMonth - 1]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    }

    if (repeat == 3) {
        CALTYPE CalType(detail_::AbbrevMonthNameCalTypes[lpCalDate->wMonth - 1]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    }

    if (repeat == 2) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0')
           << lpCalDate->wMonth;
        return ss.str();
    }

    return lexical_cast<std::wstring>(lpCalDate->wMonth);
}

std::wstring FormatDayPicture(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    CalendarDate const* lpCalDate,
    size_t repeat)
{
    if (repeat >= 4) {
        CALTYPE CalType(
            detail_::DayOfWeekCalTypes[lpCalDate->wDayOfWeek]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    }

    if (repeat == 3) {
        CALTYPE CalType(
            detail_::AbbrevDayOfWeekCalTypes[lpCalDate->wDayOfWeek]);
        return detail_::GetCalendarInfoEx(
            lpLocaleName, CalendarID, 0, CalType, 0);
    }

    if (repeat == 2) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0')
           << lpCalDate->wDay;
        return ss.str();
    }

    return lexical_cast<std::wstring>(lpCalDate->wDay);
}

std::wstring FormatEraPicture(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    CalendarDate const* lpCalDate,
    size_t /* repeat */)
{
    std::vector<std::wstring> eras;
    detail_::EnumCalendarInfoExEx(
        [&](LPWSTR lpCalendarInfoString, CALID Calendar, LPWSTR) -> bool {
            eras.insert(eras.begin(), lpCalendarInfoString);
            return true;
        },
        lpLocaleName, CalendarID, 0, CAL_SERASTRING);

    size_t eraIndex = lpCalDate->Era;
    return eras[eraIndex];

}

std::wstring FormatDatePicture(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    CalendarDate const* lpCalDate,
    WCHAR type,
    size_t repeat)
{
    switch (type) {
        case L'y':
            return FormatYearPicture(
                lpLocaleName, CalendarID, lpCalDate, repeat);
        case L'M':
            return FormatMonthPicture(
                lpLocaleName, CalendarID, lpCalDate, repeat);
        case L'd':
            return FormatDayPicture(
                lpLocaleName, CalendarID, lpCalDate, repeat);
        case L'g':
            return FormatEraPicture(
                lpLocaleName, CalendarID, lpCalDate, repeat);
        default:
            return L"";
    }
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
    CalendarDate cd(ConvertSystemTimeToCalendarDate(CalendarID, &st));

    LPCWSTR lpPos(lpFormat);

    std::wstringstream ss;
    while (*lpPos) {
        if (IsPictureChar(*lpPos)) {
            LPCWSTR lpNext = wcschr_not(lpPos, *lpPos);
            size_t const repeat = std::distance(lpPos, lpNext);
            
            ss << detail_::FormatDatePicture(
                lpLocaleName, CalendarID, &cd, *lpPos, repeat);
            lpPos = lpNext;
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


bool operator==(SYSTEMTIME const& rhs, SYSTEMTIME const& lhs)
{
    return rhs.wYear == lhs.wYear
        && rhs.wMonth == lhs.wMonth
        && rhs.wDay == lhs.wDay
        && rhs.wHour == lhs.wHour
        && rhs.wMinute == lhs.wMinute
        && rhs.wSecond == lhs.wSecond
        && rhs.wMilliseconds == lhs.wMilliseconds;
}

bool operator<(SYSTEMTIME const& rhs, SYSTEMTIME const& lhs)
{
    if (rhs.wYear < lhs.wYear) {
        return true;
    } else if (rhs.wYear > lhs.wYear) {
        return false;
    }

    if (rhs.wMonth < lhs.wMonth) {
        return true;
    } else if (rhs.wMonth > lhs.wMonth) {
        return false;
    }

    if (rhs.wDay < lhs.wDay) {
        return true;
    } else if (rhs.wDay > lhs.wDay) {
        return false;
    }

    if (rhs.wHour < lhs.wHour) {
        return true;
    } else if (rhs.wHour > lhs.wHour) {
        return false;
    }

    if (rhs.wMinute < lhs.wMinute) {
        return true;
    } else if (rhs.wMinute > lhs.wMinute) {
        return false;
    }

    if (rhs.wSecond < lhs.wSecond) {
        return true;
    } else if (rhs.wSecond > lhs.wSecond) {
        return false;
    }

    if (rhs.wMilliseconds < lhs.wMilliseconds) {
        return true;
    } else if (rhs.wMilliseconds > lhs.wMilliseconds) {
        return false;
    }

    return false;
}

bool operator<=(SYSTEMTIME const& rhs, SYSTEMTIME const& lhs)
{
    return rhs == lhs || rhs < lhs;
}

namespace detail_ {

SYSTEMTIME JapaneseEra[] = {
    { 1868,  1, 0,  1, 0 },
    { 1912,  7, 0, 30, 0 },
    { 1926, 12, 0, 25, 0 },
    { 1989,  1, 0,  8, 0 },
};


int GetYearOffsetRange(CALID CalendarID) 
{
    switch (CalendarID) {
        case CAL_THAI:
            return lexical_cast<WORD>(
                detail_::GetCalendarInfoEx(
                    L"th-TH", CalendarID, 0, CAL_IYEAROFFSETRANGE, 0));
        case CAL_KOREA:
            return 2333;
        default:
            return 0;
    }
}

}


CalendarDate ConvertSystemTimeToCalendarDate(
    CALID CalendarID, SYSTEMTIME const* lpSystemTime)
{
    SYSTEMTIME st(*lpSystemTime);
    detail_::ValidateSystemTime(&st);
    switch (CalendarID) {
        case CAL_GREGORIAN:
        default: {
            CalendarDate calDate
                = { CAL_GREGORIAN, 0,
                    st.wYear, st.wMonth, st.wDay, st.wDayOfWeek };
            return calDate;
        }
        case CAL_JAPAN: {
            CalendarDate calDate = { CAL_JAPAN, 0 };
            for (DWORD dwIndex = 0;
                 dwIndex < _countof(detail_::JapaneseEra);
                 ++dwIndex) {
                if (detail_::JapaneseEra[dwIndex] <= *lpSystemTime) {
                    calDate.Era = dwIndex;
                    calDate.wYear
                        = st.wYear - detail_::JapaneseEra[dwIndex].wYear + 1;
                }
            }

            calDate.wMonth = st.wMonth;
            calDate.wDay = st.wDay;
            calDate.wDayOfWeek = st.wDayOfWeek;
            
            return calDate;
        }
        case CAL_THAI:
        case CAL_KOREA: {
            CalendarDate calDate = { CalendarID, 0 };
            int offset(detail_::GetYearOffsetRange(CalendarID));
            calDate.wYear = st.wYear + offset;
            calDate.wMonth = st.wMonth;
            calDate.wDay = st.wDay;
            calDate.wDayOfWeek = st.wDayOfWeek;
            return calDate;
        }
    }

}
