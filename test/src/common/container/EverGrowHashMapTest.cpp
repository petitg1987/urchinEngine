#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/container/EverGrowHashMapTest.h>
#include <AssertHelper.h>
using namespace urchin;

void EverGrowHashMapTest::insertAndGet() {
    EverGrowHashMap<int, float> map(4, 0.75f);

    map.insert(12, 1.0f);
    AssertHelper::assertFloatEquals(map.at(12), 1.0f);
    AssertHelper::assertFloatEquals(*map.find(12), 1.0f);
    AssertHelper::assertNull(map.find(13));

    //TODO add more test
}

CppUnit::Test* EverGrowHashMapTest::suite() {
    auto* suite = new CppUnit::TestSuite("EverGrowHashMapTest");

    suite->addTest(new CppUnit::TestCaller("insertAndGet", &EverGrowHashMapTest::insertAndGet));

    return suite;
}
