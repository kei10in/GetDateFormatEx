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

TEST_P(TestDateFormat, FormatYearGetsStrings) {
    DateFormatTestParam param = GetParam();
    
    SYSTEMTIME st;
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
class TestWin32GetDateFormatEx_
    : public ::testing::TestWithParam<DateFormatTestParam> {
};

TEST_P(TestWin32GetDateFormatEx_, FormatToString) {
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
    };
    return std::vector<DateFormatTestParam>(params, params + _countof(params));
}

INSTANTIATE_TEST_CASE_P(
    DateFormat,
    TestWin32GetDateFormatEx_,
    ::testing::ValuesIn(GenWin32GetDateFormatTestParamList()));
