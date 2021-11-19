#include <cppunit/extensions/HelperMacros.h>

#include <common/math/algebra/point/PointTest.h>
#include <AssertHelper.h>
using namespace urchin;

void PointTest::triviallyCopyable() {
    AssertHelper::assertTrue( std::is_trivially_copyable<Point2<float>>::value);
    AssertHelper::assertTrue( std::is_trivially_copyable<Point3<float>>::value);
    AssertHelper::assertTrue( std::is_trivially_copyable<Point4<float>>::value);
}

CppUnit::Test* PointTest::suite() {
    auto* suite = new CppUnit::TestSuite("PointTest");

    suite->addTest(new CppUnit::TestCaller<PointTest>("triviallyCopyable", &PointTest::triviallyCopyable));

    return suite;
}
