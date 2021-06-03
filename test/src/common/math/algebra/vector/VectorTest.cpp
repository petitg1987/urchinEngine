#include <cppunit/extensions/HelperMacros.h>

#include <common/math/algebra/vector/VectorTest.h>
#include <AssertHelper.h>

void VectorTest::triviallyCopyable() {
    AssertHelper::assertTrue( std::is_trivially_copyable<Vector2<float>>::value);
    AssertHelper::assertTrue( std::is_trivially_copyable<Vector3<float>>::value);
    AssertHelper::assertTrue( std::is_trivially_copyable<Vector4<float>>::value);
}

CppUnit::Test* VectorTest::suite() {
    auto* suite = new CppUnit::TestSuite("VectorTest");

    suite->addTest(new CppUnit::TestCaller<VectorTest>("triviallyCopyable", &VectorTest::triviallyCopyable));

    return suite;
}
