#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/collision/GJKAlgorithmTest.h"
#include "AssertHelper.h"
using namespace urchin;

void GJKAlgorithmTest::identicalAABBox() {
    AABBox aabbox1(Point3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
    AABBox aabbox2(Point3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(aabbox1, aabbox2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::separateAABBox() {
    AABBox aabbox1(Point3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
    AABBox aabbox2(Point3(-1.1f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(aabbox1, aabbox2);

    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 0.1f);
    AssertHelper::assertFloatEquals(result.getClosestPointA().X, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().X, -0.1f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().vector(result.getClosestPointA()).length(), 0.1f);
}

void GJKAlgorithmTest::overlapAABBox() {
    AABBox aabbox1(Point3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
    AABBox aabbox2(Point3(-0.5f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(aabbox1, aabbox2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::identicalOBBox() {
    OBBox obbox1(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    OBBox obbox2(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(obbox1, obbox2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::separateEdgeOBBox() {
    OBBox obbox1(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    OBBox obbox2(Vector3(1.0f, 1.0f, 1.0f), Point3(3.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(obbox1, obbox2);

    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getClosestPointA().Y, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().X, 1.58578643763f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().Y, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().vector(result.getClosestPointA()).length(), 0.17157287526f);
}

void GJKAlgorithmTest::separateCornerOBBox() {
    OBBox obbox1(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::fromAxisAngle(Vector3(0.245f, 0.769f, -0.59f), 0.987859f));
    OBBox obbox2(Vector3(1.0f, 1.0f, 1.0f), Point3(3.0f, 0.0f, 0.0f), Quaternion<float>());

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(obbox1, obbox2);

    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 0.26794919243f);
    AssertHelper::assertPoint3FloatEquals(result.getClosestPointA(), Point3(1.73205080757f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(result.getClosestPointB(), Point3(2.0f, 0.0f, 0.0f));
}

void GJKAlgorithmTest::overlapOnEdgeOBBox() {
    OBBox obbox1(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    OBBox obbox2(Vector3(1.0f, 1.0f, 1.0f), Point3(2.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(obbox1, obbox2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::identicalBox() {
    std::vector obbPoints1 = {
            Point3(-0.3f, 1.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(-0.3f, -1.0f, 0.0f), Point3(-0.8f, 0.0f, 0.0f),
            Point3(-0.3f, 1.0f, -1.0f), Point3(0.2f, 0.0f, -1.0f), Point3(-0.3f, -1.0f, -1.0f), Point3(-0.8f, 0.0f, -1.0f)
    };
    std::vector obbPoints2 = {
            Point3(-0.3f, 1.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(-0.3f, -1.0f, 0.0f), Point3(-0.8f, 0.0f, 0.0f),
            Point3(-0.3f, 1.0f, -1.0f), Point3(0.2f, 0.0f, -1.0f), Point3(-0.3f, -1.0f, -1.0f), Point3(-0.8f, 0.0f, -1.0f)
    };
    ConvexHull3D ch1(obbPoints1);
    ConvexHull3D ch2(obbPoints2);

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(ch1, ch2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::separateBox() {
    std::vector aabbPoints = {
            Point3(0.0f, 1.0f, 0.0f), Point3(1.0f, 1.0f, 0.0f), Point3(1.0f, -1.0f, 0.0f), Point3(0.0f, -1.0f, 0.0f),
            Point3(0.0f, 1.0f, -1.0f), Point3(1.0f, 1.0f, -1.0f), Point3(1.0f, -1.0f, -1.0f), Point3(0.0f, -1.0f, -1.0f)
    };
    std::vector obbPoints = {
            Point3(-0.6f, 1.0f, 0.0f), Point3(-0.1f, 0.0f, 0.0f), Point3(-0.6f, -1.0f, 0.0f), Point3(-1.1f, 0.0f, 0.0f),
            Point3(-0.6f, 1.0f, -1.0f), Point3(-0.1f, 0.0f, -1.0f), Point3(-0.6f, -1.0f, -1.0f), Point3(-1.1f, 0.0f, -1.0f)
    };
    ConvexHull3D ch1(aabbPoints);
    ConvexHull3D ch2(obbPoints);

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(ch1, ch2);

    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 0.1f);
    AssertHelper::assertFloatEquals(result.getClosestPointA().X, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointA().Y, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().X, -0.1f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().Y, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().vector(result.getClosestPointA()).length(), 0.1f);
}

void GJKAlgorithmTest::cornerInsideBox() {
    std::vector aabbPoints = {
            Point3(0.0f, 1.0f, 0.0f), Point3(1.0f, 1.0f, 0.0f), Point3(1.0f, -1.0f, 0.0f), Point3(0.0f, -1.0f, 0.0f),
            Point3(0.0f, 1.0f, -1.0f), Point3(1.0f, 1.0f, -1.0f), Point3(1.0f, -1.0f, -1.0f), Point3(0.0f, -1.0f, -1.0f)
    };
    std::vector obbPoints = {
            Point3(-0.3f, 1.0f, 0.0f), Point3(0.2f, 0.0f, 0.0f), Point3(-0.3f, -1.0f, 0.0f), Point3(-0.8f, 0.0f, 0.0f),
            Point3(-0.3f, 1.0f, -1.0f), Point3(0.2f, 0.0f, -1.0f), Point3(-0.3f, -1.0f, -1.0f), Point3(-0.8f, 0.0f, -1.0f)
    };
    ConvexHull3D ch1(aabbPoints);
    ConvexHull3D ch2(obbPoints);

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(ch1, ch2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::faceInsideTrapeze() {
    std::vector trapezePoints1 = {
            Point3(1.0f, 0.0f, 0.0f), Point3(3.0f, 0.0f, 0.0f), Point3(0.0f, 3.0f, 0.0f), Point3(0.0f, 1.0f, 0.0f),
            Point3(1.0f, 0.0f, -1.0f), Point3(3.0f, 0.0f, -1.0f), Point3(0.0f, 3.0f, -1.0f), Point3(0.0f, 1.0f, -1.0f),
    };
    std::vector trapezePoints2 = {
            Point3(0.0f, -1.0f, 0.0f), Point3(1.0f, 0.8f, 0.0f), Point3(0.8f, 1.0f, 0.0f), Point3(-1.0f, 0.0f, 0.0f),
            Point3(0.0f, -1.0f, -1.0f), Point3(1.0f, 0.8f, -1.0f), Point3(0.8f, 1.0f, -1.0f), Point3(-1.0f, 0.0f, -1.0f),
    };
    ConvexHull3D ch1(trapezePoints1);
    ConvexHull3D ch2(trapezePoints2);

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(ch1, ch2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::separateHexagon() {
    std::vector hexagonPoints1 = {
            Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f), Point3(1.5f, 1.0f, 0.0f), Point3(1.0f, 2.0f, 0.0f), Point3(0.0f, 2.0f, 0.0f), Point3(-0.5f, 1.0f, 0.0f),
            Point3(0.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f), Point3(1.5f, 1.0f, -1.0f), Point3(1.0f, 2.0f, -1.0f), Point3(0.0f, 2.0f, -1.0f), Point3(-0.5f, 1.0f, -1.0f),
    };
    std::vector hexagonPoints2 = {
            Point3(-2.5f, 0.0f, 0.0f), Point3(-1.5f, 0.0f, 0.0f), Point3(-1.0f, 1.0f, 0.0f), Point3(-1.5f, 2.0f, 0.0f), Point3(-2.5f, 2.0f, 0.0f), Point3(-3.0f, 1.0f, 0.0f),
            Point3(-2.5f, 0.0f, -1.0f), Point3(-1.5f, 0.0f, -1.0f), Point3(-1.0f, 1.0f, -1.0f), Point3(-1.5f, 2.0f, -1.0f), Point3(-2.5f, 2.0f, -1.0f), Point3(-3.0f, 1.0f, -1.0f),
    };
    ConvexHull3D ch1(hexagonPoints1);
    ConvexHull3D ch2(hexagonPoints2);

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(ch1, ch2);

    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 0.5f);
    AssertHelper::assertFloatEquals(result.getClosestPointA().X, -0.5f);
    AssertHelper::assertFloatEquals(result.getClosestPointA().Y, 1.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().X, -1.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().Y, 1.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().vector(result.getClosestPointA()).length(), 0.5f);
}

void GJKAlgorithmTest::cornerInsideHexagon() {
    std::vector hexagonPoints1 = {
            Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f), Point3(1.5f, 1.0f, 0.0f), Point3(1.0f, 2.0f, 0.0f), Point3(0.0f, 2.0f, 0.0f), Point3(-0.5f, 1.0f, 0.0f),
            Point3(0.0f, 0.0f, -1.0f), Point3(1.0f, 0.0f, -1.0f), Point3(1.5f, 1.0f, -1.0f), Point3(1.0f, 2.0f, -1.0f), Point3(0.0f, 2.0f, -1.0f), Point3(-0.5f, 1.0f, -1.0f),
    };
    std::vector hexagonPoints2 = {
            Point3(-1.5f, 0.0f, 0.0f), Point3(-0.5f, 0.0f, 0.0f), Point3(0.0f, 1.0f, 0.0f), Point3(-0.5f, 2.0f, 0.0f), Point3(-1.5f, 2.0f, 0.0f), Point3(-2.0f, 1.0f, 0.0f),
            Point3(-1.5f, 0.0f, -1.0f), Point3(-0.5f, 0.0f, -1.0f), Point3(0.0f, 1.0f, -1.0f), Point3(-0.5f, 2.0f, -1.0f), Point3(-1.5f, 2.0f, -1.0f), Point3(-2.0f, 1.0f, -1.0f),
    };
    ConvexHull3D ch1(hexagonPoints1);
    ConvexHull3D ch2(hexagonPoints2);

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(ch1, ch2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::identicalSphere() {
    Sphere sphere1(10.0f, Point3(1.0f, 0.0f, 0.0f));
    Sphere sphere2(10.0f, Point3(1.0f, 0.0f, 0.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(sphere1, sphere2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::separateSphere() {
    Sphere sphere1(10.0f, Point3(0.0f, 0.0f, 0.0f));
    Sphere sphere2(10.0f, Point3(7.0710678118f + 8.0f, 7.0710678118f + 8.0f, 0.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(sphere1, sphere2);

    float epsilon = 0.01f; //high epsilon used because curved shapes are bad case for GJK
    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 1.31370849889f, epsilon);
    AssertHelper::assertPoint3FloatEquals(result.getClosestPointA(), Point3(7.0710678118f, 7.0710678118f, 0.0f), epsilon);
    AssertHelper::assertPoint3FloatEquals(result.getClosestPointB(), Point3(8.0f, 8.0f, 0.0f), epsilon);
}

void GJKAlgorithmTest::overlapSphere() {
    Sphere sphere1(10.0f, Point3(0.0f, 0.0f, 0.0f));
    Sphere sphere2(10.0f, Point3(7.0710678118f + 7.0f, 7.0710678118f + 7.0f, 0.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(sphere1, sphere2);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::separateOBBoxAABBox() {
    OBBox obbox(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    AABBox aabbox(Point3(2.0f, -1.0f, -1.0f), Vector3(2.0f, 2.0f, 2.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(obbox, aabbox);

    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 0.58578643763f);
    AssertHelper::assertFloatEquals(result.getClosestPointA().X, 1.41421356237f);
    AssertHelper::assertFloatEquals(result.getClosestPointA().Y, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().X, 2.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().Y, 0.0f);
    AssertHelper::assertFloatEquals(result.getClosestPointB().vector(result.getClosestPointA()).length(), 0.58578643763f);
}

void GJKAlgorithmTest::overlapOBBoxAABBox() {
    OBBox obbox(Vector3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 0.0f), Quaternion<float>::rotationZ(2.35619449f));
    AABBox aabbox(Point3(1.0f, -1.0f, -1.0f), Vector3(2.0f, 2.0f, 2.0f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(obbox, aabbox);

    AssertHelper::assertTrue(result.isCollide());
}

void GJKAlgorithmTest::separateSphereAndAABBox() {
    Sphere sphere(1.0f, Point3(0.0f, 0.0f, 0.0f));
    AABBox aabbox(Point3(1.1f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(sphere, aabbox);

    AssertHelper::assertFalse(result.isCollide());
    AssertHelper::assertFloatEquals(result.getSeparatingDistance(), 0.1f);
}

void GJKAlgorithmTest::overlapSphereAndAABBox() {
    Sphere sphere(1.0f, Point3(0.0f, 0.0f, 0.0f));
    AABBox aabbox(Point3(0.9f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f));

    GJKResult<float> result = GJKAlgorithm<float>().processGJK(sphere, aabbox);

    AssertHelper::assertTrue(result.isCollide());
}

CppUnit::Test* GJKAlgorithmTest::suite() {
    auto* suite = new CppUnit::TestSuite("GJKAlgorithmTest");

    //AABBox
    suite->addTest(new CppUnit::TestCaller("identicalAABBox", &GJKAlgorithmTest::identicalAABBox));
    suite->addTest(new CppUnit::TestCaller("separateAABBox", &GJKAlgorithmTest::separateAABBox));
    suite->addTest(new CppUnit::TestCaller("overlapAABBox", &GJKAlgorithmTest::overlapAABBox));

    //OBBox
    suite->addTest(new CppUnit::TestCaller("identicalOBBox", &GJKAlgorithmTest::identicalOBBox));
    suite->addTest(new CppUnit::TestCaller("separateEdgeOBBox", &GJKAlgorithmTest::separateEdgeOBBox));
    suite->addTest(new CppUnit::TestCaller("separateCornerOBBox", &GJKAlgorithmTest::separateCornerOBBox));
    suite->addTest(new CppUnit::TestCaller("overlapOnEdgeOBBox", &GJKAlgorithmTest::overlapOnEdgeOBBox));

    //Convex hull
    suite->addTest(new CppUnit::TestCaller("identicalBox", &GJKAlgorithmTest::identicalBox));
    suite->addTest(new CppUnit::TestCaller("separateBox", &GJKAlgorithmTest::separateBox));
    suite->addTest(new CppUnit::TestCaller("cornerInsideBox", &GJKAlgorithmTest::cornerInsideBox));
    suite->addTest(new CppUnit::TestCaller("faceInsideTrapeze", &GJKAlgorithmTest::faceInsideTrapeze));
    suite->addTest(new CppUnit::TestCaller("separateHexagon", &GJKAlgorithmTest::separateHexagon));
    suite->addTest(new CppUnit::TestCaller("cornerInsideHexagon", &GJKAlgorithmTest::cornerInsideHexagon));

    //Sphere
    suite->addTest(new CppUnit::TestCaller("identicalSphere", &GJKAlgorithmTest::identicalSphere));
    suite->addTest(new CppUnit::TestCaller("separateSphere", &GJKAlgorithmTest::separateSphere));
    suite->addTest(new CppUnit::TestCaller("overlapSphere", &GJKAlgorithmTest::overlapSphere));

    //Different shape type
    suite->addTest(new CppUnit::TestCaller("separateOBBoxAABBox", &GJKAlgorithmTest::separateOBBoxAABBox));
    suite->addTest(new CppUnit::TestCaller("overlapOBBoxAABBox", &GJKAlgorithmTest::overlapOBBoxAABBox));
    suite->addTest(new CppUnit::TestCaller("separateSphereAndAABBox", &GJKAlgorithmTest::separateSphereAndAABBox));
    suite->addTest(new CppUnit::TestCaller("overlapSphereAndAABBox", &GJKAlgorithmTest::overlapSphereAndAABBox));

    return suite;
}
