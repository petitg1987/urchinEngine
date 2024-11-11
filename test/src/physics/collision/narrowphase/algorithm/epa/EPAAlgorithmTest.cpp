#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/collision/narrowphase/algorithm/epa/EPAAlgorithmTest.h>
using namespace urchin;

void EPAAlgorithmTest::identicalAABBox() {
    CollisionBoxObject aabbox1(0.0f, Vector3(0.5f, 0.5f, 0.5f), Point3(0.5f, 0.5f, 0.5f), Quaternion<float>());
    CollisionBoxObject aabbox2(0.0f, Vector3(0.5f, 0.5f, 0.5f), Point3(0.5f, 0.5f, 0.5f), Quaternion<float>());

    EPAResult<float> resultEpa = executeEPA(aabbox1, aabbox2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 1.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 1.0f);
}

void EPAAlgorithmTest::overlapAABBox() {
    CollisionBoxObject aabbox1(0.0f, Vector3(0.5f, 0.5f, 0.5f), Point3(0.5f, 0.5f, 0.5f), Quaternion<float>());
    CollisionBoxObject aabbox2(0.0f, Vector3(0.5f, 0.5f, 0.5f), Point3(1.1f, 0.5f, 0.5f), Quaternion<float>());

    EPAResult<float> resultEpa = executeEPA(aabbox1, aabbox2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.4f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(1.0f, 0.0f, 0.0f));
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.4f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().X, 1.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().X, 0.6f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Y, resultEpa.getContactPointB().Y);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, resultEpa.getContactPointB().Z);
}

void EPAAlgorithmTest::identicalOBBox() {
    CollisionBoxObject obbox1(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    CollisionBoxObject obbox2(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));

    EPAResult<float> resultEpa = executeEPA(obbox1, obbox2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 2.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 2.0f);
}

void EPAAlgorithmTest::overlapOnEdgeOBBox() {
    CollisionBoxObject obbox1(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    CollisionBoxObject obbox2(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(2.5f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));

    EPAResult<float> resultEpa = executeEPA(obbox1, obbox2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.23223304702f);
    AssertHelper::assertFloatEquals(resultEpa.getNormal().X, 0.70710678118f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.23223304702f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, resultEpa.getContactPointB().Z);
}

void EPAAlgorithmTest::overlapOnCornerOBBox() {
    CollisionBoxObject obbox1(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f),
            Quaternion<float>::fromAxisAngle(Vector3(0.245f, 0.769f, -0.59f), 0.987859f));
    CollisionBoxObject obbox2(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(2.5f, 0.0f, 0.0f), Quaternion<float>());

    EPAResult<float> resultEpa = executeEPA(obbox1, obbox2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.23205080757f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(1.0f, 0.0f, 0.0f));
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.23205080757f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().X, 1.73205080757f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().X, 1.5f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Y, resultEpa.getContactPointB().Y);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, resultEpa.getContactPointB().Z);
}

void EPAAlgorithmTest::identicalBox() {
    std::vector obbPoints1 = {
            Point3(-0.3f, 1.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(-0.3f, -1.0f, 0.0f), Point3(-0.8f, 0.0f, 0.0f),
            Point3(-0.3f, 1.0f, -1.0f), Point3(0.2f, 0.0f, -1.0f), Point3(-0.3f, -1.0f, -1.0f), Point3(-0.8f, 0.0f, -1.0f)
    };
    std::vector obbPoints2 = {
            Point3(-0.3f, 1.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(-0.3f, -1.0f, 0.0f), Point3(-0.8f, 0.0f, 0.0f),
            Point3(-0.3f, 1.0f, -1.0f), Point3(0.2f, 0.0f, -1.0f), Point3(-0.3f, -1.0f, -1.0f), Point3(-0.8f, 0.0f, -1.0f)
    };
    CollisionConvexHullObject ch1(0.0f, obbPoints1, obbPoints1);
    CollisionConvexHullObject ch2(0.0f, obbPoints2, obbPoints2);

    EPAResult<float> resultEpa = executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.894427191f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.894427191f);
}

void EPAAlgorithmTest::cornerInsideBox() {
    std::vector aabbPoints = {
            Point3(0.0f, 1.0f, 0.0f), Point3(1.0f, 1.0f, 0.0f), Point3(1.0f, -1.0f, 0.0f), Point3(0.0f, -1.0f, 0.0f),
            Point3(0.0f, 1.0f, -1.0f), Point3(1.0f, 1.0f, -1.0f), Point3(1.0f, -1.0f, -1.0f), Point3(0.0f, -1.0f, -1.0f)
    };
    std::vector obbPoints = {
            Point3(-0.3f, 1.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(-0.3f, -1.0f, 0.0f), Point3(-0.8f, 0.0f, 0.0f),
            Point3(-0.3f, 1.0f, -1.0f), Point3(0.2f, 0.0f, -1.0f), Point3(-0.3f, -1.0f, -1.0f), Point3(-0.8f, 0.0f, -1.0f)
    };
    CollisionConvexHullObject ch1(0.0f, aabbPoints, aabbPoints);
    CollisionConvexHullObject ch2(0.0f, obbPoints, obbPoints);

    EPAResult<float> resultEpa = executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.2f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(-1.0f, 0.0f, 0.0f));
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.2f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().X, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Y, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().X, 0.2f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().Y, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, resultEpa.getContactPointB().Z);
}

void EPAAlgorithmTest::faceInsideTrapeze() {
    std::vector trapezePoints1 = {
            Point3(1.0f, 0.0f, 0.0f), Point3(3.0f, 0.0f, 0.0f), Point3(0.0f, 3.0f, 0.0f), Point3(0.0f, 1.0f, 0.0f),
            Point3(1.0f, 0.0f, -1.0f), Point3(3.0f, 0.0f, -1.0f), Point3(0.0f, 3.0f, -1.0f), Point3(0.0f, 1.0f, -1.0f),
    };
    std::vector trapezePoints2 = {
            Point3(0.0f, -1.0f, 0.0f), Point3(1.0f, 0.8f, 0.0f), Point3(0.8f, 1.0f, 0.0f), Point3(-1.0f, 0.0f, 0.0f),
            Point3(0.0f, -1.0f, -1.0f), Point3(1.0f, 0.8f, -1.0f), Point3(0.8f, 1.0f, -1.0f), Point3(-1.0f, 0.0f, -1.0f),
    };
    CollisionConvexHullObject ch1(0.0f, trapezePoints1, trapezePoints1);
    CollisionConvexHullObject ch2(0.0f, trapezePoints2, trapezePoints2);

    EPAResult<float> resultEpa = executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.56568542494f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(-0.70710678118f, -0.70710678118f, 0.0f));
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.56568542494f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().X, 0.5f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Y, 0.5f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().X, 0.9f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().Y, 0.9f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, resultEpa.getContactPointB().Z);
}

void EPAAlgorithmTest::cornerInsideHexagon() {
    std::vector hexagonPoints1 = {
            Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f), Point3(1.5f, 1.0f, 0.0f), Point3(1.0f, 2.0f, 0.0f), Point3(0.0f, 2.0f, 0.0f), Point3(-0.5f, 1.0f, 0.0f),
            Point3(0.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f), Point3(1.5f, 1.0f, -1.0f), Point3(1.0f, 2.0f, -1.0f), Point3(0.0f, 2.0f, -1.0f), Point3(-0.5f, 1.0f, -1.0f),
    };
    std::vector hexagonPoints2 = {
            Point3(-1.5f, 0.0f, 0.0f), Point3(-0.5f, 0.0f, 0.0f), Point3(0.0f, 1.0f, 0.0f), Point3(-0.5f, 2.0f, 0.0f), Point3(-1.5f, 2.0f, 0.0f), Point3(-2.0f, 1.0f, 0.0f),
            Point3(-1.5f, 0.0f, -1.0f), Point3(-0.5f, 0.0f, -1.0f), Point3(0.0f, 1.0f, -1.0f), Point3(-0.5f, 2.0f, -1.0f), Point3(-1.5f, 2.0f, -1.0f), Point3(-2.0f, 1.0f, -1.0f),
    };
    CollisionConvexHullObject ch1(0.0f, hexagonPoints1, hexagonPoints1);
    CollisionConvexHullObject ch2(0.0f, hexagonPoints2, hexagonPoints2);

    EPAResult<float> resultEpa = executeEPA(ch1, ch2);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.44721359f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.44721359f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, resultEpa.getContactPointB().Z);
}

void EPAAlgorithmTest::identicalSphere() {
    CollisionSphereObject sphere1(1.0f, Point3(1.0f, 0.0f, 0.0f));
    CollisionSphereObject sphere2(1.0f, Point3(1.0f, 0.0f, 0.0f));

    EPAResult<float> resultEpa = executeEPA(sphere1, sphere2);

    float epsilon = 0.3f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 2.0f, epsilon);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 2.0f, epsilon);
}

void EPAAlgorithmTest::overlapSphere() {
    CollisionSphereObject sphere1(10.0f, Point3(0.0f, 0.0f, 0.0f));
    CollisionSphereObject sphere2(10.0f, Point3(7.0710678118f + 7.0f, 7.0710678118f + 7.0f, 0.0f));

    EPAResult<float> resultEpa = executeEPA(sphere1, sphere2);

    float epsilon = 0.01f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.10050506338f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(0.70710678118f, 0.70710678118f, 0.0f), epsilon);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().X, 7.0710678118f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Y, 7.0710678118f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().X, 7.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().Y, 7.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().Z, 0.0f);
}

void EPAAlgorithmTest::overlapOBBoxAABBox() {
    CollisionBoxObject obbox(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    CollisionBoxObject aabbox(0.0f, Vector3(1.0f, 1.0f, 1.0f), Point3(2.0f, 0.0f, 0.0f), Quaternion<float>());

    EPAResult<float> resultEpa = executeEPA(obbox, aabbox);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.41421356237f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(1.0f, 0.0f, 0.0f));
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.41421356237f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().X, 1.41421356237f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Y, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().X, 1.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointB().Y, 0.0f);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().Z, resultEpa.getContactPointB().Z);
}

void EPAAlgorithmTest::overlapSphereAndBox() {
    CollisionSphereObject sphere(1.0f, Point3(0.0f, 0.0f, 0.0f));
    CollisionBoxObject aabbox(0.0f, Vector3(0.5f, 0.5f, 0.5f), Point3(1.4f, 0.5f, 0.5f), Quaternion<float>());

    EPAResult<float> resultEpa = executeEPA(sphere, aabbox);

    float epsilon = 0.05f; //high epsilon used because curved shapes are very bad case for EPA
    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.1f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(1.0f, 0.0f, 0.0f), epsilon);
    AssertHelper::assertFloatEquals(resultEpa.getContactPointA().vector(resultEpa.getContactPointB()).length(), 0.1f, epsilon);
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointA(), Point3(1.0f, 0.0f, 0.0f), epsilon);
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointB(), Point3(0.9f, 0.0f, 0.0f), epsilon);
}

void EPAAlgorithmTest::overlapCapsuleAndTriangle() {
    CollisionCapsuleObject capsule(0.0f, 0.25f, 1.0f, CapsuleShape<float>::CAPSULE_Y, Point3(0.0f, 0.2f, 0.0f), Quaternion<float>());
    CollisionTriangleObject triangle(0.0f, Point3(0.0f, 0.0f, 0.0f), Point3(-2.0f, 0.0f, -2.0f), Point3(-2.0f, 0.0f, 0.0f));

    EPAResult<float> resultEpa = executeEPA(capsule, triangle);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.55f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(0.0f, -1.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointA(), Point3(0.0f, -0.55f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointB(), Point3(0.0f, 0.0f, 0.0f));
}

void EPAAlgorithmTest::overlapTriangleAndCapsule() {
    CollisionTriangleObject triangle(0.0f, Point3(0.0f, 0.0f, 0.0f), Point3(-2.0f, 0.0f, -2.0f), Point3(-2.0f, 0.0f, 0.0f));
    CollisionCapsuleObject capsule(0.0f, 0.25f, 1.0f, CapsuleShape<float>::CAPSULE_Y, Point3(0.0f, 0.2f, 0.0f), Quaternion<float>());

    EPAResult<float> resultEpa = executeEPA(triangle, capsule);

    AssertHelper::assertTrue(resultEpa.isCollide());
    AssertHelper::assertFloatEquals(resultEpa.getPenetrationDepth(), 0.55f);
    AssertHelper::assertVector3FloatEquals(resultEpa.getNormal(), Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointA(), Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(resultEpa.getContactPointB(), Point3(0.0f, -0.55f, 0.0f));
}

EPAResult<float> EPAAlgorithmTest::executeEPA(const CollisionConvexObject3D& object1, const CollisionConvexObject3D& object2) const {
    GJKResult<float> resultGjk = GJKAlgorithm<float>().processGJK(GJKConvexObjectWrapper(object1, true), GJKConvexObjectWrapper(object2, true));
    return EPAAlgorithm<float>().processEPA(object1, object2, resultGjk);
}

CppUnit::Test* EPAAlgorithmTest::suite() {
    auto* suite = new CppUnit::TestSuite("EPABoxTest");

    //AABBox
    suite->addTest(new CppUnit::TestCaller("identicalAABBox", &EPAAlgorithmTest::identicalAABBox));
    suite->addTest(new CppUnit::TestCaller("overlapAABBox", &EPAAlgorithmTest::overlapAABBox));

    //OBBox
    suite->addTest(new CppUnit::TestCaller("identicalOBBox", &EPAAlgorithmTest::identicalOBBox));
    suite->addTest(new CppUnit::TestCaller("overlapOnEdgeOBBox", &EPAAlgorithmTest::overlapOnEdgeOBBox));
    suite->addTest(new CppUnit::TestCaller("overlapOnCornerOBBox", &EPAAlgorithmTest::overlapOnCornerOBBox));

    //Convex hull
    suite->addTest(new CppUnit::TestCaller("identicalBox", &EPAAlgorithmTest::identicalBox));
    suite->addTest(new CppUnit::TestCaller("cornerInsideBox", &EPAAlgorithmTest::cornerInsideBox));
    suite->addTest(new CppUnit::TestCaller("faceInsideTrapeze", &EPAAlgorithmTest::faceInsideTrapeze));
    suite->addTest(new CppUnit::TestCaller("cornerInsideHexagon", &EPAAlgorithmTest::cornerInsideHexagon));

    //Sphere
    suite->addTest(new CppUnit::TestCaller("identicalSphere", &EPAAlgorithmTest::identicalSphere));
    suite->addTest(new CppUnit::TestCaller("overlapSphere", &EPAAlgorithmTest::overlapSphere));

    //Different shape type
    suite->addTest(new CppUnit::TestCaller("overlapOBBoxAABBox", &EPAAlgorithmTest::overlapOBBoxAABBox));
    suite->addTest(new CppUnit::TestCaller("overlapSphereAndBox", &EPAAlgorithmTest::overlapSphereAndBox));
    suite->addTest(new CppUnit::TestCaller("overlapCapsuleAndTriangle", &EPAAlgorithmTest::overlapCapsuleAndTriangle));
    suite->addTest(new CppUnit::TestCaller("overlapTriangleAndCapsule", &EPAAlgorithmTest::overlapTriangleAndCapsule));

    return suite;
}
