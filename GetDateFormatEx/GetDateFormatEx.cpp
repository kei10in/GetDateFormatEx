#include <Windows.h>

#include <vector>
#include <sstream>
#include <iomanip>

#include <boost/lexical_cast.hpp>

#include "GetDateFormatEx.h"


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


std::wstring GetDateFormatEx(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    SYSTEMTIME const* st,
    LPCWSTR lpFormat,
    CALID CalendarID)
{
    if (wcscmp(lpFormat, L"yyyy") == 0) {
        return boost::lexical_cast<std::wstring>(st->wYear);
    } else if (wcscmp(lpFormat, L"yy") == 0) {
        std::wstringstream ss;
        ss << std::right << std::setw(2) << std::setfill(L'0')
           << st->wYear % 100;
        return ss.str();
    } else if (*lpFormat == L'y') {
        return boost::lexical_cast<std::wstring>(st->wYear % 100);
    }
    return L"";
}
