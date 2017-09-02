#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "TriangulationTest.h"
#include "AssertHelper.h"
using namespace urchin;

void TriangulationTest::triangleTriangulation()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(2.0, 0.0), Point2<float>(1.0, 1.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints);
	std::vector<IndexedTriangleMesh> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 1);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(0), 1);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(1), 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(2), 0);
}

void TriangulationTest::cubeTriangulation()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(1.0, 0.0),
												   Point2<float>(1.0, 1.0), Point2<float>(0.0, 1.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints);
	std::vector<IndexedTriangleMesh> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(0), 0);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(1), 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(2), 3);

	AssertHelper::assertUnsignedInt(triangles[1].getIndex(0), 1);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(1), 2);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(2), 0);
}

void TriangulationTest::cavityTriangulation()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(3.0, 4.0), Point2<float>(0.0, 3.0), Point2<float>(1.0, 2.0),
												   Point2<float>(0.0, 1.75), Point2<float>(2.0, 0.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints);
	std::vector<IndexedTriangleMesh> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 3);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(0), 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(2), 1);

	AssertHelper::assertUnsignedInt(triangles[1].getIndex(0), 4);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(1), 2);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(2), 3);

	AssertHelper::assertUnsignedInt(triangles[2].getIndex(0), 4);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(2), 2);
}

void TriangulationTest::twoNearPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(500.0, 500.0), Point2<float>(0.0, 3.0), Point2<float>(2.0, 3.0), Point2<float>(2.0, 2.0),
												   Point2<float>(2.0000002, 2.0), Point2<float>(3.0, 1.5), Point2<float>(5.0, 1.5), Point2<float>(0.0, 0.0),
												   Point2<float>(6.0, 0.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints);
	std::vector<IndexedTriangleMesh> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 7);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(0), 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(2), 1);

	AssertHelper::assertUnsignedInt(triangles[1].getIndex(0), 3);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(2), 2);

	AssertHelper::assertUnsignedInt(triangles[2].getIndex(0), 4);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(2), 3);

	AssertHelper::assertUnsignedInt(triangles[3].getIndex(0), 5);
	AssertHelper::assertUnsignedInt(triangles[3].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[3].getIndex(2), 4);

	AssertHelper::assertUnsignedInt(triangles[4].getIndex(0), 6);
	AssertHelper::assertUnsignedInt(triangles[4].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[4].getIndex(2), 5);

	AssertHelper::assertUnsignedInt(triangles[5].getIndex(0), 8);
	AssertHelper::assertUnsignedInt(triangles[5].getIndex(1), 6);
	AssertHelper::assertUnsignedInt(triangles[5].getIndex(2), 7);

	AssertHelper::assertUnsignedInt(triangles[6].getIndex(0), 8);
	AssertHelper::assertUnsignedInt(triangles[6].getIndex(1), 0);
	AssertHelper::assertUnsignedInt(triangles[6].getIndex(2), 6);
}

void TriangulationTest::threeAlignedPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 5.0), Point2<float>(-1.0, 4.0), Point2<float>(0.0, 0.0),
												   Point2<float>(1.0, 1.0), Point2<float>(1.0, 2.0), Point2<float>(1.0, 3.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints);
	std::vector<IndexedTriangleMesh> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 4);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(0), 5);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(1), 1);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(2), 0);

	AssertHelper::assertUnsignedInt(triangles[1].getIndex(0), 4);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(1), 1);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(2), 5);

	AssertHelper::assertUnsignedInt(triangles[2].getIndex(0), 3);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(1), 1);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(2), 4);

	AssertHelper::assertUnsignedInt(triangles[3].getIndex(0), 2);
	AssertHelper::assertUnsignedInt(triangles[3].getIndex(1), 1);
	AssertHelper::assertUnsignedInt(triangles[3].getIndex(2), 3);
}

void TriangulationTest::alternationPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 6.0), Point2<float>(-1.0, 4.0), Point2<float>(-1.0, 2.0),
												   Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 3.0),
												   Point2<float>(1.0, 5.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints);
	std::vector<IndexedTriangleMesh> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 5);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(0), 1);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(1), 6);
	AssertHelper::assertUnsignedInt(triangles[0].getIndex(2), 0);

	AssertHelper::assertUnsignedInt(triangles[1].getIndex(0), 5);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(1), 1);
	AssertHelper::assertUnsignedInt(triangles[1].getIndex(2), 6);

	AssertHelper::assertUnsignedInt(triangles[2].getIndex(0), 2);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(1), 5);
	AssertHelper::assertUnsignedInt(triangles[2].getIndex(2), 1);

	AssertHelper::assertUnsignedInt(triangles[3].getIndex(0), 4);
	AssertHelper::assertUnsignedInt(triangles[3].getIndex(1), 2);
	AssertHelper::assertUnsignedInt(triangles[3].getIndex(2), 5);

	AssertHelper::assertUnsignedInt(triangles[4].getIndex(0), 3);
	AssertHelper::assertUnsignedInt(triangles[4].getIndex(1), 2);
	AssertHelper::assertUnsignedInt(triangles[4].getIndex(2), 4);
}

CppUnit::Test *TriangulationTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("TriangulationTest");

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("triangleTriangulation", &TriangulationTest::triangleTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cubeTriangulation", &TriangulationTest::cubeTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cavityTriangulation", &TriangulationTest::cavityTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("twoNearPoints", &TriangulationTest::twoNearPoints));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("threeAlignedPoints", &TriangulationTest::threeAlignedPoints));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("alternationPoints", &TriangulationTest::alternationPoints));

	return suite;
}
