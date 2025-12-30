#include <cppunit/extensions/HelperMacros.h>
#include <cstring>
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

void StringUtilTest::readNextCodepoint() {
    const char* testStr = "A©€\xF0\x9F\x98\x80"; //characters encoded respectively on 1 byte, 2 bytes, 3 bytes and 4 bytes
    const char* textIt = testStr;
    const char* textEndIt = textIt + std::strlen(testStr);

    char32_t firstChar = StringUtil::readNextCodepoint(textIt, textEndIt);
    AssertHelper::assertTrue(firstChar == 0x0041);

    char32_t secondChar = StringUtil::readNextCodepoint(textIt, textEndIt);
    AssertHelper::assertTrue(secondChar == 0x00A9);

    char32_t thirdChar = StringUtil::readNextCodepoint(textIt, textEndIt);
    AssertHelper::assertTrue(thirdChar == 0x20AC);

    char32_t fourthChar = StringUtil::readNextCodepoint(textIt, textEndIt);
    AssertHelper::assertTrue(fourthChar == 0x1F600);

    AssertHelper::assertTrue(*textIt == '\0');
}

void StringUtilTest::readCodepoints() {
    const std::string utf8String = "A©€\xF0\x9F\x98\x80"; //characters encoded respectively on 1 byte, 2 bytes, 3 bytes and 4 bytes
    std::vector<char32_t> codepoints;

    StringUtil::readCodepoints(utf8String, codepoints);

    AssertHelper::assertUnsignedIntEquals(codepoints.size(), 4);
    AssertHelper::assertTrue(codepoints[0] == 0x0041);
    AssertHelper::assertTrue(codepoints[1] == 0x00A9);
    AssertHelper::assertTrue(codepoints[2] == 0x20AC);
    AssertHelper::assertTrue(codepoints[3] == 0x1F600);
}

void StringUtilTest::readUtf8String() {
    std::vector<char32_t> codepoints = {0x0041, 0x00A9, 0x20AC, 0x1F600};
    std::string utf8String;

    StringUtil::readUtf8String(codepoints, utf8String);

    AssertHelper::assertStringEquals(utf8String, "A©€\xF0\x9F\x98\x80");
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

    suite->addTest(new CppUnit::TestCaller("readNextCodepoint", &StringUtilTest::readNextCodepoint));
    suite->addTest(new CppUnit::TestCaller("readCodepoints", &StringUtilTest::readCodepoints));
    suite->addTest(new CppUnit::TestCaller("readUtf8String", &StringUtilTest::readUtf8String));

    return suite;
}
