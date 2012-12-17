#include <Windows.h>

#include "gtest/gtest.h"

#include "GetDateFormatEx.h"

struct DateFormatTestParam {
    LPCWSTR localename;
    CALID calendar;
    WORD year;
    WORD month;
    WORD day;
    LPCWSTR format;
    LPCWSTR expected;
};


class TestDateFormat : public ::testing::TestWithParam<DateFormatTestParam> {
};

TEST_P(TestDateFormat, FormatToString) {
    DateFormatTestParam const &param = GetParam();

    LPCWSTR lpLocaleName = param.localename;
    CALID CalendarID = param.calendar;
    
    SYSTEMTIME st = {};
    st.wYear = param.year;
    st.wMonth = param.month;
    st.wDay = param.day;
    std::wstring actual(
        GetDateFormatEx(lpLocaleName, CalendarID, &st, param.format));
    std::wstring expected(param.expected);
    ASSERT_EQ(expected, actual);    
}

std::vector<DateFormatTestParam> GenDateFormatTestParamList() {
    static DateFormatTestParam params[] = {
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"", L"" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"yyyy", L"2012" },
        { L"en-US", CAL_GREGORIAN, 2011, 8, 12, L"yyyy", L"2011" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"yy", L"12" },
        { L"en-US", CAL_GREGORIAN, 2009, 8, 12, L"yy", L"09" },
        { L"en-US", CAL_GREGORIAN, 2009, 8, 12, L"y", L"9" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"MMMM", L"August" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"MMM", L"Aug" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"MM", L"08" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"M", L"8" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"dddd", L"Sunday" },
        { L"en-US", CAL_GREGORIAN, 2012, 8, 12, L"ddd", L"Sun" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"dd", L"01" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"d", L"1" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"gg", L"A.D." },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"g", L"A.D." },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"yyMM", L"1208" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"yy/dd", L"12/01" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"'y'", L"y" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"''''", L"'" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"'''a", L"'a" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"yyy", L"2012" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"yyyyy", L"2012" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"MMMMM", L"August" },
        { L"en-US", CAL_GREGORIAN, 2012, 8,  1, L"ddddd", L"Sunday" },
        { L"ja-JP", CAL_JAPAN, 2012, 8,  12, L"gg", L"ïΩê¨" },
        { L"ja-JP", CAL_JAPAN, 1989, 8,  12, L"gg", L"ïΩê¨" },
        { L"ja-JP", CAL_JAPAN, 1988, 8,  12, L"gg", L"è∫òa" },
        { L"ja-JP", CAL_JAPAN, 1926, 8,  12, L"gg", L"è∫òa" },
        { L"ja-JP", CAL_JAPAN, 1925, 8,  12, L"gg", L"ëÂê≥" },
        { L"ja-JP", CAL_JAPAN, 1912, 8,  12, L"gg", L"ëÂê≥" },
        { L"ja-JP", CAL_JAPAN, 1911, 8,  12, L"gg", L"ñæé°" },
        { L"ja-JP", CAL_JAPAN, 2012, 8,  12, L"y", L"24" },
        { L"ja-JP", CAL_JAPAN, 1989, 8,  12, L"y", L"1" },
        { L"ja-JP", CAL_JAPAN, 1988, 8,  12, L"y", L"63" },
        { L"ja-JP", CAL_JAPAN, 1926, 8,  12, L"y", L"1" },
        { L"ja-JP", CAL_JAPAN, 1925, 8,  12, L"y", L"14" },
        { L"ja-JP", CAL_JAPAN, 1912, 8,  12, L"y", L"1" },
        { L"ja-JP", CAL_JAPAN, 1911, 8,  12, L"y", L"44" },
    };
    return std::vector<DateFormatTestParam>(params, params + _countof(params));
}

INSTANTIATE_TEST_CASE_P(
    DateFormat,
    TestDateFormat,
    ::testing::ValuesIn(GenDateFormatTestParamList()));


//////////////////////////////////////////////////////////////////////
// Win32 API's GetDateFormatEx Test
//////////////////////////////////////////////////////////////////////
class TestWin32GetDateFormatEx
    : public ::testing::TestWithParam<DateFormatTestParam> {
};

TEST_P(TestWin32GetDateFormatEx, FormatToString) {
    DateFormatTestParam const& param(GetParam());
    
    LPCWSTR lpLocaleName = param.localename;

    SYSTEMTIME st = {};
    st.wYear = param.year;
    st.wMonth = param.month;
    st.wDay = param.day;
    std::wstring actual(
        GetDateFormatEx(lpLocaleName, 0, &st, param.format, nullptr));
    std::wstring expected(param.expected);
    ASSERT_EQ(expected, actual);    
}

std::vector<DateFormatTestParam> GenWin32GetDateFormatTestParamList() {
    static DateFormatTestParam params[] = {
        { L"en-US", 0, 2012, 8, 12, L"", L"" },
        { L"en-US", 0, 2012, 8, 12, L"yyyy", L"2012" },
        { L"en-US", 0, 2011, 8, 12, L"yyyy", L"2011" },
        { L"en-US", 0, 2012, 8, 12, L"yy", L"12" },
        { L"en-US", 0, 2009, 8, 12, L"yy", L"09" },
        { L"en-US", 0, 2009, 8, 12, L"y", L"9" },
        { L"en-US", 0, 2012, 8, 12, L"MMMM", L"August" },
        { L"en-US", 0, 2012, 8, 12, L"MMM", L"Aug" },
        { L"en-US", 0, 2012, 8, 12, L"MM", L"08" },
        { L"en-US", 0, 2012, 8, 12, L"M", L"8" },
        { L"en-US", 0, 2012, 8, 12, L"ddd", L"Sun" },
        { L"en-US", 0, 2012, 8,  1, L"gg", L"A.D." },
        { L"en-US", 0, 2012, 8,  1, L"g", L"A.D." },
        { L"en-US", 0, 2012, 8,  1, L"yy'/'MM", L"12/08" },
        { L"en-US", 0, 2012, 8,  1, L"yy/MM", L"12/08" },
        { L"en-US", 0, 2012, 8,  1, L"yy'îN'MM'åé'", L"12îN08åé" },
        { L"en-US", 0, 2012, 8,  1, L"yy'y' MM'M'", L"12y 08M" },
        { L"en-US", 0, 2012, 8,  1, L"''''a", L"'a" },
        { L"en-US", 0, 2012, 8,  1, L"'''a", L"'a" },
        { L"en-US", 0, 2012, 8,  1, L"'yyyy", L"yyyy" },
        { L"en-US", 0, 2012, 8,  1, L"yyyy''yy", L"201212" },
        { L"en-US", 0, 2012, 8,  1, L"yyy", L"2012" },
        { L"en-US", 0, 2012, 8,  12, L"ddddd", L"Sunday" },
        { L"en-US", 0, 2012, 8,  1, L"ggg", L"A.D." },
    };
    return std::vector<DateFormatTestParam>(params, params + _countof(params));
}

INSTANTIATE_TEST_CASE_P(
    DateFormat,
    TestWin32GetDateFormatEx,
    ::testing::ValuesIn(GenWin32GetDateFormatTestParamList()));
