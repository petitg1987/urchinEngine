#include <cppunit/extensions/HelperMacros.h>

#include <common/util/HashUtilTest.h>
#include <AssertHelper.h>
using namespace urchin;

void HashUtilTest::hashBool() {
    std::size_t firstHash = 0;
    std::size_t secondHash = 0;

    HashUtil::combine(firstHash, false, true);
    HashUtil::combine(secondHash, true, false);

    AssertHelper::assertTrue(firstHash != secondHash);
}

void HashUtilTest::hashFloat() {
    std::size_t firstHash = 0;
    std::size_t secondHash = 0;

    HashUtil::combine(firstHash, 1.0f, 0.0f);
    HashUtil::combine(secondHash, 0.0f, 1.0f);

    AssertHelper::assertTrue(firstHash != secondHash);
}

CppUnit::Test* HashUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("HashUtilTest");

    suite->addTest(new CppUnit::TestCaller("hashBool", &HashUtilTest::hashBool));
    suite->addTest(new CppUnit::TestCaller("hashFloat", &HashUtilTest::hashFloat));

    return suite;
}
