#include <cppunit/extensions/HelperMacros.h>

#include "StringUtilTest.h"
#include "AssertHelper.h"
using namespace urchin;

void StringUtilTest::splitString() {
    std::string str = "str1,str2,str3";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ',', splitStr);

    AssertHelper::assertInt(splitStr.size(), 3);
    AssertHelper::assertString(splitStr[0], "str1");
    AssertHelper::assertString(splitStr[1], "str2");
    AssertHelper::assertString(splitStr[2], "str3");
}

void StringUtilTest::splitEmptyString() {
    std::string str = "str1,,str2";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ',', splitStr);

    AssertHelper::assertInt(splitStr.size(), 3);
    AssertHelper::assertString(splitStr[0], "str1");
    AssertHelper::assertString(splitStr[1], "");
    AssertHelper::assertString(splitStr[2], "str2");
}

CppUnit::Test *StringUtilTest::suite() {
    auto *suite = new CppUnit::TestSuite("StringUtilTest");

    suite->addTest(new CppUnit::TestCaller<StringUtilTest>("splitString", &StringUtilTest::splitString));
    suite->addTest(new CppUnit::TestCaller<StringUtilTest>("splitEmptyString", &StringUtilTest::splitEmptyString));

    return suite;
}
