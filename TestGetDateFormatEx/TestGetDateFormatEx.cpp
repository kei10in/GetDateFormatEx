#include <Windows.h>

#include "gtest/gtest.h"

#include "GetDateFormatEx.h"


TEST(TestGetDateFormatEx, FormatYear2012) {
    SYSTEMTIME st;
    st.wYear = 2012;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"yyyy", CAL_GREGORIAN));
    std::wstring expected(L"2012");
    ASSERT_EQ(expected, actual);    
}


TEST(TestGetDateFormatEx, FormatYear2011) {
    SYSTEMTIME st;
    st.wYear = 2011;
    std::wstring actual(
        GetDateFormatEx(L"en-US", 0, &st, L"yyyy", CAL_GREGORIAN));
    std::wstring expected(L"2011");
    ASSERT_EQ(expected, actual);    
}
