#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/container/EverGrowHashMapTest.h"
#include "AssertHelper.h"
using namespace urchin;

void EverGrowHashMapTest::insertGetErase() {
    EverGrowHashMap<int, float> map(4, 0.75f);

    AssertHelper::assertTrue(map.isEmpty());

    map.insert(12, 1.0f);
    map.insert(14, 2.0f);
    AssertHelper::assertFalse(map.isEmpty());
    AssertHelper::assertFloatEquals(map.at(12), 1.0f);
    AssertHelper::assertFloatEquals(*map.find(12), 1.0f);
    AssertHelper::assertNull(map.find(13));
    AssertHelper::assertFloatEquals(map.at(14), 2.0f);
    AssertHelper::assertUnsignedIntEquals(map.getSize(), 2);
    AssertHelper::assertUnsignedIntEquals(map.getNumBuckets(), 4);

    map.insert(15, 3.0f);
    map.insert(16, 4.0f);
    map.insert(17, 5.0f);
    AssertHelper::assertUnsignedIntEquals(map.getNumBuckets(), 8);
    AssertHelper::assertFloatEquals(map.at(12), 1.0f);
    AssertHelper::assertFloatEquals(map.at(17), 5.0f);

    map.erase(17);
    AssertHelper::assertNull(map.find(17));
    AssertHelper::assertUnsignedIntEquals(map.getSize(), 4);

    map.clear();
    AssertHelper::assertUnsignedIntEquals(map.getSize(), 0);
    AssertHelper::assertUnsignedIntEquals(map.getNumBuckets(), 8);
    AssertHelper::assertNull(map.find(12));
}

void EverGrowHashMapTest::moveAssignment() {
    EverGrowHashMap<int, float> sourceMap(8, 0.75f);
    sourceMap.insert(1, 1.0f);
    sourceMap.insert(2, 2.0f);
    EverGrowHashMap<int, float> targetMap(4, 0.75f);
    targetMap.insert(3, 3.0f);

    targetMap = std::move(sourceMap);

    AssertHelper::assertUnsignedIntEquals(targetMap.getSize(), 2);
    AssertHelper::assertUnsignedIntEquals(targetMap.getNumBuckets(), 8);
    AssertHelper::assertFloatEquals(targetMap.at(1), 1.0f);
    AssertHelper::assertFloatEquals(targetMap.at(2), 2.0f);
    AssertHelper::assertNull(targetMap.find(3));
    AssertHelper::assertTrue(sourceMap.isEmpty());
    AssertHelper::assertUnsignedIntEquals(sourceMap.getNumBuckets(), 4);
    AssertHelper::assertNull(sourceMap.find(3));
}

CppUnit::Test* EverGrowHashMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("EverGrowHashMapTest");

    suite->addTest(new CppUnit::TestCaller("insertGetErase", &EverGrowHashMapTest::insertGetErase));
    suite->addTest(new CppUnit::TestCaller("moveAssignment", &EverGrowHashMapTest::moveAssignment));

    return suite;
}
