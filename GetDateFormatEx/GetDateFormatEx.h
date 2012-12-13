#pragma once
#ifndef GETDATEFORMATEX_GETDATEFORMATEX_H_
#define GETDATEFORMATEX_GETDATEFORMATEX_H_

#include <Windows.h>

#include <string>


extern std::wstring GetDateFormatEx(
    LPCWSTR lpLocaleName,
    DWORD dwFlags,
    SYSTEMTIME const* st,
    LPCWSTR lpFormat,
    LPCWSTR lpCalendar);

extern std::wstring GetDateFormatEx(
    LPCWSTR lpLocaleName,
    CALID CalendarID,
    SYSTEMTIME const* st,
    LPCWSTR lpFormat);


#endif  /* GETDATEFORMATEX_GETDATEFORMATEX_H_ */
