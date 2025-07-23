#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/math/algebra/vector/VectorTest.h"
#include "AssertHelper.h"
using namespace urchin;

void VectorTest::triviallyCopyable() {
    AssertHelper::assertTrue( std::is_trivially_copyable_v<Vector2<float>>);
    AssertHelper::assertTrue( std::is_trivially_copyable_v<Vector3<float>>);
    AssertHelper::assertTrue( std::is_trivially_copyable_v<Vector4<float>>);
}

CppUnit::Test* VectorTest::suite() {
    auto* suite = new CppUnit::TestSuite("VectorTest");

    suite->addTest(new CppUnit::TestCaller("triviallyCopyable", &VectorTest::triviallyCopyable));

    return suite;
}
