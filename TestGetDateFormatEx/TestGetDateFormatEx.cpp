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

TEST(TestWin32GetDateFormatEx, Format_yy_By2012Gets12) {
    SYSTEMTIME st = {};
    st.wYear = 2012;
    st.wMonth = 1;
    st.wDay = 2;
    std::wstring actual(GetDateFormatEx(L"en-US", 0, &st, L"yy", nullptr));
    std::wstring expected(L"12");
    ASSERT_EQ(expected, actual);    
}

TEST(TestWin32GetDateFormatEx, FormatNullString) {
    SYSTEMTIME st = {};
    st.wYear = 2012;
    st.wMonth = 1;
    st.wDay = 2;
    std::wstring actual(GetDateFormatEx(L"en-US", 0, &st, L"", nullptr));
    std::wstring expected(L"");
    ASSERT_EQ(expected, actual);    
}

TEST(TestWin32GetDateFormatEx, Format_MMM_By8GetsAug) {
    SYSTEMTIME st = {};
    st.wYear = 2012;
    st.wMonth = 8;
    st.wDay = 2;
    std::wstring actual(GetDateFormatEx(L"en-US", 0, &st, L"MMM", nullptr));
    std::wstring expected(L"Aug");
    ASSERT_EQ(expected, actual);
}
