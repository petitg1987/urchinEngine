#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/algebra/point/PointTest.h>
#include <AssertHelper.h>
using namespace urchin;

void PointTest::triviallyCopyable() {
    AssertHelper::assertTrue(std::is_trivially_copyable_v<Point2<float>>);
    AssertHelper::assertTrue(std::is_trivially_copyable_v<Point3<float>>);
    AssertHelper::assertTrue(std::is_trivially_copyable_v<Point4<float>>);
}

CppUnit::Test* PointTest::suite() {
    auto* suite = new CppUnit::TestSuite("PointTest");

    suite->addTest(new CppUnit::TestCaller<PointTest>("triviallyCopyable", &PointTest::triviallyCopyable));

    return suite;
}
