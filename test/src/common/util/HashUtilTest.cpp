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

CppUnit::Test* HashUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("HashUtilTest");

    suite->addTest(new CppUnit::TestCaller<HashUtilTest>("hashBool", &HashUtilTest::hashBool));

    return suite;
}
