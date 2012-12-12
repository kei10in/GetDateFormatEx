#include <Windows.h>

#include "gtest/gtest.h"

#include "GetDateFormatEx.h"


TEST(TestGetDateFormatEx, Format_yyyy_By2012Gets2012) {
    SYSTEMTIME st;
    st.wYear = 2012;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"yyyy", CAL_GREGORIAN));
    std::wstring expected(L"2012");
    ASSERT_EQ(expected, actual);    
}

TEST(TestGetDateFormatEx, Format_yyyy_By2011Gets2011) {
    SYSTEMTIME st;
    st.wYear = 2011;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"yyyy", CAL_GREGORIAN));
    std::wstring expected(L"2011");
    ASSERT_EQ(expected, actual);    
}

TEST(TestGetDateFormatEx, Format_yy_By2011Gets11) {
    SYSTEMTIME st;
    st.wYear = 2011;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"yy", CAL_GREGORIAN));
    std::wstring expected(L"11");
    ASSERT_EQ(expected, actual);    
}

TEST(TestGetDateFormatEx, Format_yy_By2009Gets09) {
    SYSTEMTIME st;
    st.wYear = 2009;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"yy", CAL_GREGORIAN));
    std::wstring expected(L"09");
    ASSERT_EQ(expected, actual);    
}

TEST(TestGetDateFormatEx, Format_y_By2009Gets9) {
    SYSTEMTIME st;
    st.wYear = 2009;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"y", CAL_GREGORIAN));
    std::wstring expected(L"9");
    ASSERT_EQ(expected, actual);    
}

TEST(TestGetDateFormatEx, FormatNullString) {
    SYSTEMTIME st;
    st.wYear = 2009;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"", CAL_GREGORIAN));
    std::wstring expected(L"");
    ASSERT_EQ(expected, actual);    
}


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
