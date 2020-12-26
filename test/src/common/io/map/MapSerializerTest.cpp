#include <cppunit/extensions/HelperMacros.h>

#include "MapSerializerTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MapSerializerTest::emptyMap() {
    std::map<std::string, std::string> emptyMap;
    auto serializedMap = MapSerializer::instance()->serialize(emptyMap);
    auto deserializedMap = MapSerializer::instance()->deserialize(serializedMap);

    AssertHelper::instance()->assertString(serializedMap, "");
    AssertHelper::instance()->assertTrue(deserializedMap.empty());
}

void MapSerializerTest::simpleMap() {
    std::map<std::string, std::string> map;
    map["key1"] = "value1";
    map["key2"] = "value2";
    auto serializedMap = MapSerializer::instance()->serialize(map);
    auto deserializedMap = MapSerializer::instance()->deserialize(serializedMap);

    AssertHelper::instance()->assertString(serializedMap, "key1,value1,key2,value2");
    AssertHelper::instance()->assertString(deserializedMap["key1"], "value1");
    AssertHelper::instance()->assertString(deserializedMap["key2"], "value2");
}

void MapSerializerTest::trickyMapValues() {
    std::map<std::string, std::string> map;
    map["k,ey1"] = "valu\\,ne1";
    map["key2\\,"] = "valu,,e2";
    auto serializedMap = MapSerializer::instance()->serialize(map);
    auto deserializedMap = MapSerializer::instance()->deserialize(serializedMap);

    AssertHelper::instance()->assertString(serializedMap, R"(k\,ey1,valu\\\,ne1,key2\\\,,valu\,\,e2)");
    AssertHelper::instance()->assertString(deserializedMap["k,ey1"], "valu\\,ne1");
    AssertHelper::instance()->assertString(deserializedMap["key2\\,"], "valu,,e2");
}

CppUnit::Test* MapSerializerTest::suite() {
    auto* suite = new CppUnit::TestSuite("MapSerializerTest");

    suite->addTest(new CppUnit::TestCaller<MapSerializerTest>("emptyMap", &MapSerializerTest::emptyMap));
    suite->addTest(new CppUnit::TestCaller<MapSerializerTest>("simpleMap", &MapSerializerTest::simpleMap));
    suite->addTest(new CppUnit::TestCaller<MapSerializerTest>("trickyMapValues", &MapSerializerTest::trickyMapValues));

    return suite;
}
