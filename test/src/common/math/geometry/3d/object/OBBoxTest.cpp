#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/object/OBBoxTest.h>
#include <AssertHelper.h>
using namespace urchin;

void OBBoxTest::rayIntersection() {
    //TODO impl...
}

CppUnit::Test* OBBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("OBBoxTest");

    suite->addTest(new CppUnit::TestCaller<OBBoxTest>("rayIntersection", &OBBoxTest::rayIntersection));

    return suite;
}
