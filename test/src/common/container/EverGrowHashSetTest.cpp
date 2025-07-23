#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/container/EverGrowHashSetTest.h"
#include "AssertHelper.h"
using namespace urchin;

void EverGrowHashSetTest::insertGetErase() {
    EverGrowHashSet<std::string> set(4, 0.75f);

    AssertHelper::assertTrue(set.isEmpty());
    for (const std::string& value : set) {
        AssertHelper::assertTrue(false, "Value found: " + value);
    }

    set.insert("12");
    AssertHelper::assertFalse(set.isEmpty());
    AssertHelper::assertTrue(set.isExist("12"));
    for (const std::string& value : set) {
        AssertHelper::assertTrue(value == "12");
    }

    set.insert("14");
    AssertHelper::assertFalse(set.insert("14"));
    AssertHelper::assertFalse(set.isExist("13"));
    AssertHelper::assertUnsignedIntEquals(set.getSize(), 2);
    AssertHelper::assertUnsignedIntEquals(set.getNumBuckets(), 4);
    for (const std::string& value : set) {
        AssertHelper::assertTrue(value == "12" || value == "14");
    }

    set.insert("15");
    set.insert("16");
    set.insert("17");
    AssertHelper::assertUnsignedIntEquals(set.getNumBuckets(), 8);

    set.erase("17");
    AssertHelper::assertFalse(set.isExist("17"));
    AssertHelper::assertUnsignedIntEquals(set.getSize(), 4);

    set.clear();
    AssertHelper::assertUnsignedIntEquals(set.getSize(), 0);
    AssertHelper::assertUnsignedIntEquals(set.getNumBuckets(), 8);
    AssertHelper::assertFalse(set.isExist("12"));
}

CppUnit::Test* EverGrowHashSetTest::suite() {
    auto* suite = new CppUnit::TestSuite("EverGrowHashSetTest");

    suite->addTest(new CppUnit::TestCaller("insertGetErase", &EverGrowHashSetTest::insertGetErase));

    return suite;
}
