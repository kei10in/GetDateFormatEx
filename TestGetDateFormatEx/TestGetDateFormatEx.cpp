#include <Windows.h>

#include "gtest/gtest.h"

#include "GetDateFormatEx.h"

struct DateFormatTestParam {
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
    
    SYSTEMTIME st = {};
    st.wYear = param.year;
    st.wMonth = param.month;
    st.wDay = param.day;
    std::wstring actual(
        GetDateFormatEx(L"en-US", CAL_GREGORIAN, &st, param.format));
    std::wstring expected(param.expected);
    ASSERT_EQ(expected, actual);    
}

std::vector<DateFormatTestParam> GenDateFormatTestParamList() {
    static DateFormatTestParam params[] = {
        { 2012, 8, 12, L"", L"" },
        { 2012, 8, 12, L"yyyy", L"2012" },
        { 2011, 8, 12, L"yyyy", L"2011" },
        { 2012, 8, 12, L"yy", L"12" },
        { 2009, 8, 12, L"yy", L"09" },
        { 2009, 8, 12, L"y", L"9" },
        { 2012, 8, 12, L"MMMM", L"August" },
        { 2012, 8, 12, L"MMM", L"Aug" },
        { 2012, 8, 12, L"MM", L"08" },
        { 2012, 8, 12, L"M", L"8" },
        { 2012, 8, 12, L"dddd", L"Sunday" },
        { 2012, 8, 12, L"ddd", L"Sun" },
        { 2012, 8,  1, L"dd", L"01" },
        { 2012, 8,  1, L"d", L"1" },
        { 2012, 8,  1, L"gg", L"A.D." },
        { 2012, 8,  1, L"g", L"A.D." },
        { 2012, 8,  1, L"yyMM", L"1208" },
        { 2012, 8,  1, L"yy/dd", L"12/01" },
        { 2012, 8,  1, L"'y'", L"y" },
        { 2012, 8,  1, L"''''", L"'" },
        { 2012, 8,  1, L"'''a", L"'a" },
        { 2012, 8,  1, L"yyy", L"2012" },
        { 2012, 8,  1, L"yyyyy", L"2012" },
        { 2012, 8,  1, L"MMMMM", L"August" },
        { 2012, 8,  1, L"ddddd", L"Sunday" },
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
    
    SYSTEMTIME st = {};
    st.wYear = param.year;
    st.wMonth = param.month;
    st.wDay = param.day;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, param.format, nullptr));
    std::wstring expected(param.expected);
    ASSERT_EQ(expected, actual);    
}

std::vector<DateFormatTestParam> GenWin32GetDateFormatTestParamList() {
    static DateFormatTestParam params[] = {
        { 2012, 8, 12, L"", L"" },
        { 2012, 8, 12, L"yyyy", L"2012" },
        { 2011, 8, 12, L"yyyy", L"2011" },
        { 2012, 8, 12, L"yy", L"12" },
        { 2009, 8, 12, L"yy", L"09" },
        { 2009, 8, 12, L"y", L"9" },
        { 2012, 8, 12, L"MMMM", L"August" },
        { 2012, 8, 12, L"MMM", L"Aug" },
        { 2012, 8, 12, L"MM", L"08" },
        { 2012, 8, 12, L"M", L"8" },
        { 2012, 8, 12, L"ddd", L"Sun" },
        { 2012, 8,  1, L"gg", L"A.D." },
        { 2012, 8,  1, L"g", L"A.D." },
        { 2012, 8,  1, L"yy'/'MM", L"12/08" },
        { 2012, 8,  1, L"yy/MM", L"12/08" },
        { 2012, 8,  1, L"yy'”N'MM'ŒŽ'", L"12”N08ŒŽ" },
        { 2012, 8,  1, L"yy'y' MM'M'", L"12y 08M" },
        { 2012, 8,  1, L"''''a", L"'a" },
        { 2012, 8,  1, L"'''a", L"'a" },
        { 2012, 8,  1, L"'yyyy", L"yyyy" },
        { 2012, 8,  1, L"yyyy''yy", L"201212" },
        { 2012, 8,  1, L"yyy", L"2012" },
        { 2012, 8,  12, L"ddddd", L"Sunday" },
        { 2012, 8,  1, L"ggg", L"A.D." },
    };
    return std::vector<DateFormatTestParam>(params, params + _countof(params));
}

INSTANTIATE_TEST_CASE_P(
    DateFormat,
    TestWin32GetDateFormatEx,
    ::testing::ValuesIn(GenWin32GetDateFormatTestParamList()));
