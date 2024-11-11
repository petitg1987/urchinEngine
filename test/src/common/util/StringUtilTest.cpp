#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/util/StringUtilTest.h>
#include <AssertHelper.h>
using namespace urchin;

void StringUtilTest::splitString() {
    std::string str = "str1,str2,str3";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ',', splitStr);

    AssertHelper::assertUnsignedIntEquals(splitStr.size(), 3);
    AssertHelper::assertStringEquals(splitStr[0], "str1");
    AssertHelper::assertStringEquals(splitStr[1], "str2");
    AssertHelper::assertStringEquals(splitStr[2], "str3");
}

void StringUtilTest::splitEmptyString() {
    std::string str = "str1,,str2";
    std::vector<std::string> splitStr;

    StringUtil::split(str, ',', splitStr);

    AssertHelper::assertUnsignedIntEquals(splitStr.size(), 3);
    AssertHelper::assertStringEquals(splitStr[0], "str1");
    AssertHelper::assertStringEquals(splitStr[1], "");
    AssertHelper::assertStringEquals(splitStr[2], "str2");
}

void StringUtilTest::mergeString() {
    std::vector<std::string> splitStr = {"str1", "", "str2"};

    std::string merged = StringUtil::merge(splitStr, ',');

    AssertHelper::assertStringEquals(merged, "str1,,str2");
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

    suite->addTest(new CppUnit::TestCaller<StringUtilTest>("splitString", &StringUtilTest::splitString));
    suite->addTest(new CppUnit::TestCaller<StringUtilTest>("splitEmptyString", &StringUtilTest::splitEmptyString));
    suite->addTest(new CppUnit::TestCaller<StringUtilTest>("mergeString", &StringUtilTest::mergeString));

    suite->addTest(new CppUnit::TestCaller<StringUtilTest>("camelToKebabCase", &StringUtilTest::camelToKebabCase));
    suite->addTest(new CppUnit::TestCaller<StringUtilTest>("kebabToCamelCase", &StringUtilTest::kebabToCamelCase));

    return suite;
}
