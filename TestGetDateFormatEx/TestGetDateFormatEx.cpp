#include <Windows.h>

#include "gtest/gtest.h"

#include "GetDateFormatEx.h"

struct YearFormatTestParam {
    WORD year;
    LPCWSTR format;
    LPCWSTR expected;
};


class TestYearFormat : public ::testing::TestWithParam<YearFormatTestParam> {
};

TEST_P(TestYearFormat, FormatYearGetsStrings) {
    YearFormatTestParam param = GetParam();
    
    SYSTEMTIME st;
    st.wYear = param.year;
    std::wstring actual(
        GetDateFormatEx(L"en-US", CAL_GREGORIAN, &st, param.format));
    std::wstring expected(param.expected);
    ASSERT_EQ(expected, actual);    
}

std::vector<YearFormatTestParam> GenYearFormatTestParamList() {
    static YearFormatTestParam params[] = {
        { 2012, L"yyyy", L"2012" },
        { 2011, L"yyyy", L"2011" },
        { 2012, L"yy", L"12" },
        { 2009, L"yy", L"09" },
        { 2009, L"y", L"9" },
        
    };
    return std::vector<YearFormatTestParam>(params, params + _countof(params));
}

INSTANTIATE_TEST_CASE_P(
    YearFormat,
    TestYearFormat,
    ::testing::ValuesIn(GenYearFormatTestParamList()));


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
