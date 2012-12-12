#include <boost/lexical_cast.hpp>

#include "GetDateFormatEx.h"

std::wstring GetDateFormatEx(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    SYSTEMTIME const* st,
    LPCWSTR lpFormat,
    CALID CalendarID)
{
    return boost::lexical_cast<std::wstring>(st->wYear);
}
