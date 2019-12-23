#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "EdgeJumpDetectionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void EdgeJumpDetectionTest::testNearParallelEdges()
{
    LineSegment3D<float> startJumpEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endJumpEdge(Point3<float>(20.0f, 0.0f, 1.0f), Point3<float>(-20.0f, 0.0f, 1.0f));

    EdgeJumpDetection edgeJumpDetection(1.5f);
    EdgeJumpResult edgeJumpResult = edgeJumpDetection.detectJump(startJumpEdge, endJumpEdge);

    AssertHelper::assertTrue(edgeJumpResult.hasJumpRange());
    AssertHelper::assertFloatEquals(edgeJumpResult.getJumpStartRange(), 1.0f);
    AssertHelper::assertFloatEquals(edgeJumpResult.getJumpEndRange(), 0.0f);
}

void EdgeJumpDetectionTest::testFarParallelEdges()
{
    LineSegment3D<float> startJumpEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endJumpEdge(Point3<float>(20.0f, 0.0f, 50.0f), Point3<float>(-20.0f, 0.0f, 50.0f));

    EdgeJumpDetection edgeJumpDetection(1.5f);
    EdgeJumpResult edgeJumpResult = edgeJumpDetection.detectJump(startJumpEdge, endJumpEdge);

    AssertHelper::assertTrue(!edgeJumpResult.hasJumpRange());
}

void EdgeJumpDetectionTest::testWrongStartJumpEdgeDirection()
{
    LineSegment3D<float> startJumpEdge(Point3<float>(5.0f, 0.0f, 0.0f), Point3<float>(-5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endJumpEdge(Point3<float>(20.0f, 0.0f, 1.0f), Point3<float>(-20.0f, 0.0f, 1.0f));

    EdgeJumpDetection edgeJumpDetection(1.5f);
    EdgeJumpResult edgeJumpResult = edgeJumpDetection.detectJump(startJumpEdge, endJumpEdge);

    //Cannot jump because character will stay inside the polygon represented by startJumpEdge
    AssertHelper::assertTrue(!edgeJumpResult.hasJumpRange());
}

void EdgeJumpDetectionTest::testWrongEndJumpEdgeDirection()
{
    LineSegment3D<float> startJumpEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endJumpEdge(Point3<float>(-20.0f, 0.0f, 1.0f), Point3<float>(20.0f, 0.0f, 1.0f));

    EdgeJumpDetection edgeJumpDetection(1.5f);
    EdgeJumpResult edgeJumpResult = edgeJumpDetection.detectJump(startJumpEdge, endJumpEdge);

    //Cannot jump because character will be on the edge of the polygon represented by endJumpEdge but in direction of the outside.
    AssertHelper::assertTrue(!edgeJumpResult.hasJumpRange());
}

void EdgeJumpDetectionTest::testWrongJumpEdgesDirection()
{
    LineSegment3D<float> startJumpEdge(Point3<float>(5.0f, 0.0f, 0.0f), Point3<float>(-5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endJumpEdge(Point3<float>(-20.0f, 0.0f, 1.0f), Point3<float>(20.0f, 0.0f, 1.0f));

    EdgeJumpDetection edgeJumpDetection(1.5f);
    EdgeJumpResult edgeJumpResult = edgeJumpDetection.detectJump(startJumpEdge, endJumpEdge);

    AssertHelper::assertTrue(!edgeJumpResult.hasJumpRange());
}

void EdgeJumpDetectionTest::testCrossEdges()
{
    LineSegment3D<float> startJumpEdge(Point3<float>(-20.0f, 0.0f, 20.0f), Point3<float>(20.0f, 0.0f, -20.0f));
    LineSegment3D<float> endJumpEdge(Point3<float>(0.0f, 0.0f, -20.0f), Point3<float>(0.0f, 0.0f, 20.0f));

    EdgeJumpDetection edgeJumpDetection(1.5f);
    EdgeJumpResult edgeJumpResult = edgeJumpDetection.detectJump(startJumpEdge, endJumpEdge);

    AssertHelper::assertTrue(edgeJumpResult.hasJumpRange());
    AssertHelper::assertFloatEquals(edgeJumpResult.getJumpStartRange(), 0.5375f, 0.012f);
    AssertHelper::assertFloatEquals(edgeJumpResult.getJumpEndRange(), 0.5f, 0.009f);
}

CppUnit::Test *EdgeJumpDetectionTest::suite()
{
    auto *suite = new CppUnit::TestSuite("EdgeJumpDetectionTest");

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testNearParallelEdges", &EdgeJumpDetectionTest::testNearParallelEdges));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testFarParallelEdges", &EdgeJumpDetectionTest::testFarParallelEdges));

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testWrongStartJumpEdgeDirection", &EdgeJumpDetectionTest::testWrongStartJumpEdgeDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testWrongEndJumpEdgeDirection", &EdgeJumpDetectionTest::testWrongEndJumpEdgeDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testWrongJumpEdgesDirection", &EdgeJumpDetectionTest::testWrongJumpEdgesDirection));

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testCrossEdges", &EdgeJumpDetectionTest::testCrossEdges));

    return suite;
}
