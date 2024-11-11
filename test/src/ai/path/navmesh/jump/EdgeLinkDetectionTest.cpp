#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinAIEngine.h>

#include <ai/path/navmesh/jump/EdgeLinkDetectionTest.h>
#include <AssertHelper.h>
using namespace urchin;

void EdgeLinkDetectionTest::testNearParallelEdges() {
    LineSegment3D startEdge(Point3(-5.0f, 0.0f, 0.0f), Point3(5.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(20.0f, 0.0f, 1.0f), Point3(-20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 1.0f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.0f);
}

void EdgeLinkDetectionTest::testFarParallelEdges() {
    LineSegment3D startEdge(Point3(-5.0f, 0.0f, 0.0f), Point3(5.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(20.0f, 0.0f, 50.0f), Point3(-20.0f, 0.0f, 50.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeLinkDetectionTest::testIdenticalEdges() {
    LineSegment3D startEdge(Point3(-5.0f, 0.0f, 0.0f), Point3(5.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(5.0f, 0.0f, 0.0f), Point3(-5.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(!edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 1.0f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.0f);

}

void EdgeLinkDetectionTest::testIdenticalEdgesWrongDirection() {
    LineSegment3D startEdge(Point3(-5.0f, 0.0f, 0.0f), Point3(5.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(-5.0f, 0.0f, 0.0f), Point3(5.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeLinkDetectionTest::testCollinearEdges1() {
    LineSegment3D startEdge(Point3(0.0f, 0.0f, 0.0f), Point3(3.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(2.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(!edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 0.666666666f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.333333333f);
}

void EdgeLinkDetectionTest::testCollinearEdges2() {
    LineSegment3D startEdge(Point3(0.0f, 0.0f, 0.0f), Point3(3.0f, 0.0f, -1.0f));
    LineSegment3D endEdge(Point3(6.0f, 0.0f, -2.0f), Point3(1.5f, 0.0f, -0.5f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(!edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 0.5f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.0f);
}

void EdgeLinkDetectionTest::testCollinearEdges3() {
    LineSegment3D startEdge(Point3(-50.0f, 0.0f, 0.0f), Point3(-100.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(-150.0f, 0.0f, 0.0f), Point3(-99.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(!edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 0.02f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.0f);
}

void EdgeLinkDetectionTest::testCollinearEdgesNoLink() {
    LineSegment3D startEdge(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(5.0f, 0.0f, 0.0f), Point3(4.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeLinkDetectionTest::testCollinearEdgesTouchingInOnePoint() {
    LineSegment3D startEdge(Point3(-50.0f, 0.0f, 0.0f), Point3(-100.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(-150.0f, 0.0f, 0.0f), Point3(-100.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeLinkDetectionTest::testCollinearEdgesWrongDirection() {
    LineSegment3D startEdge(Point3(0.0f, 0.0f, 0.0f), Point3(3.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(1.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeLinkDetectionTest::testWrongStartJumpEdgeDirection() {
    LineSegment3D startEdge(Point3(5.0f, 0.0f, 0.0f), Point3(-5.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(20.0f, 0.0f, 1.0f), Point3(-20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink()); //Cannot jump because character will stay inside the polygon represented by startEdge
}

void EdgeLinkDetectionTest::testWrongEndJumpEdgeDirection() {
    LineSegment3D startEdge(Point3(-5.0f, 0.0f, 0.0f), Point3(5.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(-20.0f, 0.0f, 1.0f), Point3(20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink()); //Cannot jump because character will be on the edge of the polygon represented by endEdge but in direction of the outside
}

void EdgeLinkDetectionTest::testWrongJumpEdgesDirection() {
    LineSegment3D startEdge(Point3(5.0f, 0.0f, 0.0f), Point3(-5.0f, 0.0f, 0.0f));
    LineSegment3D endEdge(Point3(-20.0f, 0.0f, 1.0f), Point3(20.0f, 0.0f, 1.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(!edgeLinkResult.hasEdgesLink());
}

void EdgeLinkDetectionTest::testCrossEdges() {
    LineSegment3D startEdge(Point3(-20.0f, 0.0f, 20.0f), Point3(20.0f, 0.0f, -20.0f));
    LineSegment3D endEdge(Point3(0.0f, 0.0f, -20.0f), Point3(0.0f, 0.0f, 20.0f));

    EdgeLinkDetection edgeLinkDetection(1.5f);
    EdgeLinkResult edgeLinkResult = edgeLinkDetection.detectLink(startEdge, endEdge);

    AssertHelper::assertTrue(edgeLinkResult.hasEdgesLink());
    AssertHelper::assertTrue(edgeLinkResult.isJumpLink());
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkStartRange(), 0.5375f, 0.012f);
    AssertHelper::assertFloatEquals(edgeLinkResult.getLinkEndRange(), 0.5f, 0.009f);
}

CppUnit::Test* EdgeLinkDetectionTest::suite() {
    auto* suite = new CppUnit::TestSuite("EdgeJumpDetectionTest");

    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testNearParallelEdges", &EdgeLinkDetectionTest::testNearParallelEdges));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testFarParallelEdges", &EdgeLinkDetectionTest::testFarParallelEdges));

    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testIdenticalEdges", &EdgeLinkDetectionTest::testIdenticalEdges));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testIdenticalEdgesWrongDirection", &EdgeLinkDetectionTest::testIdenticalEdgesWrongDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testCollinearEdges1", &EdgeLinkDetectionTest::testCollinearEdges1));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testCollinearEdges2", &EdgeLinkDetectionTest::testCollinearEdges2));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testCollinearEdges3", &EdgeLinkDetectionTest::testCollinearEdges3));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testCollinearEdgesNoLink", &EdgeLinkDetectionTest::testCollinearEdgesNoLink));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testCollinearEdgesTouchingInOnePoint", &EdgeLinkDetectionTest::testCollinearEdgesTouchingInOnePoint));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testCollinearEdgesWrongDirection", &EdgeLinkDetectionTest::testCollinearEdgesWrongDirection));

    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testWrongStartJumpEdgeDirection", &EdgeLinkDetectionTest::testWrongStartJumpEdgeDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testWrongEndJumpEdgeDirection", &EdgeLinkDetectionTest::testWrongEndJumpEdgeDirection));
    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testWrongJumpEdgesDirection", &EdgeLinkDetectionTest::testWrongJumpEdgesDirection));

    suite->addTest(new CppUnit::TestCaller<EdgeLinkDetectionTest>("testCrossEdges", &EdgeLinkDetectionTest::testCrossEdges));

    return suite;
}
