#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "EdgeJumpDetectionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void EdgeJumpDetectionTest::testNearParallelEdges()
{
    LineSegment3D<float> startEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(20.0f, 0.0f, 1.0f), Point3<float>(-20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 1.0f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.0f);
}

void EdgeJumpDetectionTest::testFarParallelEdges()
{
    LineSegment3D<float> startEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(20.0f, 0.0f, 50.0f), Point3<float>(-20.0f, 0.0f, 50.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeJumpDetectionTest::testIdenticalEdges()
{
    LineSegment3D<float> startEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(5.0f, 0.0f, 0.0f), Point3<float>(-5.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(!edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 1.0f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.0f);

}

void EdgeJumpDetectionTest::testIdenticalEdgesWrongDirection()
{
    LineSegment3D<float> startEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeJumpDetectionTest::testCollinearEdges1()
{
    LineSegment3D<float> startEdge(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(3.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(2.0f, 0.0f, 0.0f), Point3<float>(1.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(!edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 0.666666666f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.333333333f);
}

void EdgeJumpDetectionTest::testCollinearEdges2()
{
    LineSegment3D<float> startEdge(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(3.0f, 0.0f, -1.0f));
    LineSegment3D<float> endEdge(Point3<float>(6.0f, 0.0f, -2.0f), Point3<float>(1.5f, 0.0f, -0.5f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(!edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 0.5f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.0f);
}

void EdgeJumpDetectionTest::testCollinearEdgesNoLink()
{
    LineSegment3D<float> startEdge(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(5.0f, 0.0f, 0.0f), Point3<float>(4.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeJumpDetectionTest::testCollinearEdgesWrongDirection()
{
    LineSegment3D<float> startEdge(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(3.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(1.0f, 0.0f, 0.0f), Point3<float>(2.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeJumpDetectionTest::testWrongStartJumpEdgeDirection()
{
    LineSegment3D<float> startEdge(Point3<float>(5.0f, 0.0f, 0.0f), Point3<float>(-5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(20.0f, 0.0f, 1.0f), Point3<float>(-20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink()); //Cannot jump because character will stay inside the polygon represented by startEdge
}

void EdgeJumpDetectionTest::testWrongEndJumpEdgeDirection()
{
    LineSegment3D<float> startEdge(Point3<float>(-5.0f, 0.0f, 0.0f), Point3<float>(5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(-20.0f, 0.0f, 1.0f), Point3<float>(20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink()); //Cannot jump because character will be on the edge of the polygon represented by endEdge but in direction of the outside
}

void EdgeJumpDetectionTest::testWrongJumpEdgesDirection()
{
    LineSegment3D<float> startEdge(Point3<float>(5.0f, 0.0f, 0.0f), Point3<float>(-5.0f, 0.0f, 0.0f));
    LineSegment3D<float> endEdge(Point3<float>(-20.0f, 0.0f, 1.0f), Point3<float>(20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeJumpDetectionTest::testCrossEdges()
{
    LineSegment3D<float> startEdge(Point3<float>(-20.0f, 0.0f, 20.0f), Point3<float>(20.0f, 0.0f, -20.0f));
    LineSegment3D<float> endEdge(Point3<float>(0.0f, 0.0f, -20.0f), Point3<float>(0.0f, 0.0f, 20.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 0.5375f, 0.012f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.5f, 0.009f);
}

CppUnit::Test *EdgeJumpDetectionTest::suite()
{
    auto *suite = new CppUnit::TestSuite("EdgeJumpDetectionTest");

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testNearParallelEdges", &EdgeJumpDetectionTest::testNearParallelEdges));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testFarParallelEdges", &EdgeJumpDetectionTest::testFarParallelEdges));

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testIdenticalEdges", &EdgeJumpDetectionTest::testIdenticalEdges));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testIdenticalEdgesWrongDirection", &EdgeJumpDetectionTest::testIdenticalEdgesWrongDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testCollinearEdges1", &EdgeJumpDetectionTest::testCollinearEdges1));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testCollinearEdges2", &EdgeJumpDetectionTest::testCollinearEdges2));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testCollinearEdgesNoLink", &EdgeJumpDetectionTest::testCollinearEdgesNoLink));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testCollinearEdgesWrongDirection", &EdgeJumpDetectionTest::testCollinearEdgesWrongDirection));

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testWrongStartJumpEdgeDirection", &EdgeJumpDetectionTest::testWrongStartJumpEdgeDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testWrongEndJumpEdgeDirection", &EdgeJumpDetectionTest::testWrongEndJumpEdgeDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testWrongJumpEdgesDirection", &EdgeJumpDetectionTest::testWrongJumpEdgesDirection));

    suite->addTest(new CppUnit::TestCaller<EdgeJumpDetectionTest>("testCrossEdges", &EdgeJumpDetectionTest::testCrossEdges));

    return suite;
}
