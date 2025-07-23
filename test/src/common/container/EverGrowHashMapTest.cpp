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

CppUnit::Test* EverGrowHashMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("EverGrowHashMapTest");

    suite->addTest(new CppUnit::TestCaller("insertGetErase", &EverGrowHashMapTest::insertGetErase));

    return suite;
}
