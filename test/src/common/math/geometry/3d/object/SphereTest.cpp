#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/object/SphereTest.h>
#include <AssertHelper.h>
using namespace urchin;

void SphereTest::sphereLineIntersection() {
    Sphere<float> sphere(1.5f, Point3<float>(1.0f, 1.0f, 0.0f));
    Line3D<float> line(Point3<float>(-2.0f, 2.0f, 0.0f), Point3<float>(4.0f, 2.0f, 0.0f));

    bool hasIntersection;
    Point3<float> firstCollision = sphere.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(firstCollision, Point3<float>(-0.11803f, 2.0f, 0.0f));
}

CppUnit::Test* SphereTest::suite() {
    auto* suite = new CppUnit::TestSuite("sphereLineIntersection");

    suite->addTest(new CppUnit::TestCaller<SphereTest>("sphereLineIntersection", &SphereTest::sphereLineIntersection));

    return suite;
}
