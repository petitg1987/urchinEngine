#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "EdgeJumpDetectionTest.h"
#include "AssertHelper.h"
using namespace urchin;

//TODO add assert
void EdgeJumpDetectionTest::testNearParallelEdges()
{
    LineSegment3D<float> edge1(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> edge2(Point3<float>(-20.0f, 1.0f, 0.0f), Point3<float>(20.0f, 1.0f, 0.0f));

    EdgeJumpDetection edgeJumpDetection;
    edgeJumpDetection.detectJump(edge1, edge2, 1.5f);

    //expected: Point3<float>(-5.???f, 1.0f, 0.0f) & Point3<float>(5.???f, 1.0f, 0.0f)
}

void EdgeJumpDetectionTest::testFarParallelEdges()
{
    LineSegment3D<float> edge1(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> edge2(Point3<float>(-20.0f, 50.0f, 0.0f), Point3<float>(20.0f, 50.0f, 0.0f));

//    EdgeJumpDetection edgeJumpDetection;
//    edgeJumpDetection.detectJump(edge1, edge2, 1.5f);

    //expected: no result
}

void EdgeJumpDetectionTest::testCrossEdges()
{
    LineSegment3D<float> edge1(Point3<float>(-20.0f, -20.0f, 0.0f), Point3<float>(20.0f, 20.0f, 0.0f));
    LineSegment3D<float> edge2(Point3<float>(-20.0f, 20.0f, 0.0f), Point3<float>(20.0f, -20.0f, 0.0f));

//    EdgeJumpDetection edgeJumpDetection;
//    edgeJumpDetection.detectJump(edge1, edge2, 1.5f);

    //expected: Point 1: -1.06066, 1.06066, 0, point 2: 1.06066, -1.06066, 0
}

CppUnit::Test *EdgeJumpDetectionTest::suite()
{
    auto *suite = new CppUnit::TestSuite("EdgeJumpDetectionTest");

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testNearParallelEdges", &EdgeJumpDetectionTest::testNearParallelEdges));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testFarParallelEdges", &EdgeJumpDetectionTest::testFarParallelEdges));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testCrossEdges", &EdgeJumpDetectionTest::testCrossEdges));

    return suite;
}
