#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/util/StringUtilTest.h"
#include "AssertHelper.h"
using namespace urchin;

void StringUtilTest::splitStringByChar() {
    std::string str = "str1,str2,str3";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ',', splitStr);

    AssertHelper::assertUnsignedIntEquals(splitStr.size(), 3);
    AssertHelper::assertStringEquals(splitStr[0], "str1");
    AssertHelper::assertStringEquals(splitStr[1], "str2");
    AssertHelper::assertStringEquals(splitStr[2], "str3");
}

void StringUtilTest::splitEmptyStringByChar() {
    std::string str = "str1,,str2";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ',', splitStr);

    AssertHelper::assertUnsignedIntEquals(splitStr.size(), 3);
    AssertHelper::assertStringEquals(splitStr[0], "str1");
    AssertHelper::assertStringEquals(splitStr[1], "");
    AssertHelper::assertStringEquals(splitStr[2], "str2");
}

void StringUtilTest::splitStringByString() {
    std::string str = "str1,str2, str3";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ", ", splitStr);

    AssertHelper::assertUnsignedIntEquals(splitStr.size(), 2);
    AssertHelper::assertStringEquals(splitStr[0], "str1,str2");
    AssertHelper::assertStringEquals(splitStr[1], "str3");
}

void StringUtilTest::splitUniqueStringByString() {
    std::string str = "str1";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ", ", splitStr);

    AssertHelper::assertUnsignedIntEquals(splitStr.size(), 1);
    AssertHelper::assertStringEquals(splitStr[0], "str1");
}

void StringUtilTest::splitEmptyStringByString() {
    std::string str = "";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ", ", splitStr);

    AssertHelper::assertTrue(splitStr.empty());
}

void StringUtilTest::joinString() {
    std::vector<std::string> splitStr = {"str1", "", "str2"};

    std::string joined = StringUtil::join(splitStr, ',');

    AssertHelper::assertStringEquals(joined, "str1,,str2");
}

void StringUtilTest::camelToKebabCase() {
    std::string kebabCaseStr = StringUtil::camelToKebabCase("myCamelCaseBBC");

    AssertHelper::assertStringEquals(kebabCaseStr, "my-camel-case-bbc");
}

void StringUtilTest::kebabToCamelCase() {
    std::string camelCaseStr = StringUtil::kebabToCamelCase("-my-kebab-case-");

    AssertHelper::assertStringEquals(camelCaseStr, "-myKebabCase-");
}

CppUnit::Test* StringUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("StringUtilTest");

    suite->addTest(new CppUnit::TestCaller("splitStringByChar", &StringUtilTest::splitStringByChar));
    suite->addTest(new CppUnit::TestCaller("splitEmptyStringByChar", &StringUtilTest::splitEmptyStringByChar));
    suite->addTest(new CppUnit::TestCaller("splitStringByString", &StringUtilTest::splitStringByString));
    suite->addTest(new CppUnit::TestCaller("splitUniqueStringByString", &StringUtilTest::splitUniqueStringByString));
    suite->addTest(new CppUnit::TestCaller("splitEmptyStringByString", &StringUtilTest::splitEmptyStringByString));
    suite->addTest(new CppUnit::TestCaller("joinString", &StringUtilTest::joinString));

    suite->addTest(new CppUnit::TestCaller("camelToKebabCase", &StringUtilTest::camelToKebabCase));
    suite->addTest(new CppUnit::TestCaller("kebabToCamelCase", &StringUtilTest::kebabToCamelCase));

    return suite;
}
