#include <cppunit/extensions/HelperMacros.h>

#include "MapUtilTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MapUtilTest::emptyMap()
{
    std::map<std::string, std::string> emptyMap;
    auto serializedMap = MapUtil::serialize(emptyMap);
    auto deserializedMap = MapUtil::deserialize(serializedMap);

    AssertHelper::assertString(serializedMap, "");
    AssertHelper::assertTrue(deserializedMap.empty());
}

void MapUtilTest::simpleMap()
{
    std::map<std::string, std::string> map;
    map["key1"] = "value1";
    map["key2"] = "value2";
    auto serializedMap = MapUtil::serialize(map);
    auto deserializedMap = MapUtil::deserialize(serializedMap);

    AssertHelper::assertString(serializedMap, "key1,value1,key2,value2");
    AssertHelper::assertString(deserializedMap["key1"], "value1");
    AssertHelper::assertString(deserializedMap["key2"], "value2");
}

void MapUtilTest::trickyMapValues()
{
    std::map<std::string, std::string> map;
    map["k,ey1"] = "valu\\,ne1";
    map["key2\\,"] = "valu,,e2";
    auto serializedMap = MapUtil::serialize(map);
    auto deserializedMap = MapUtil::deserialize(serializedMap);

    AssertHelper::assertString(serializedMap, R"(k\,ey1,valu\\\,ne1,key2\\\,,valu\,\,e2)");
    AssertHelper::assertString(deserializedMap["k,ey1"], "valu\\,ne1");
    AssertHelper::assertString(deserializedMap["key2\\,"], "valu,,e2");
}

CppUnit::Test *MapUtilTest::suite()
{
    auto *suite = new CppUnit::TestSuite("MapUtilTest");

    suite->addTest(new CppUnit::TestCaller<MapUtilTest>("emptyMap", &MapUtilTest::emptyMap));
    suite->addTest(new CppUnit::TestCaller<MapUtilTest>("simpleMap", &MapUtilTest::simpleMap));
    suite->addTest(new CppUnit::TestCaller<MapUtilTest>("trickyMapValues", &MapUtilTest::trickyMapValues));

    return suite;
}
