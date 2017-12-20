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

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 1);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{1, 2, 0});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{-1, -1, -1});
}

void TriangulationTest::cubeTriangulation()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(1.0, 0.0),
												   Point2<float>(1.0, 1.0), Point2<float>(0.0, 1.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 2);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{0, 2, 3});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{1, 2, 0});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{1, -1, -1});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{-1, 0, -1});
}

void TriangulationTest::twoNearPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(500.0, 500.0), Point2<float>(0.0, 3.0), Point2<float>(2.0, 3.0), Point2<float>(2.0, 2.0),
												   Point2<float>(2.0000002, 2.0), Point2<float>(3.0, 1.5), Point2<float>(5.0, 1.5), Point2<float>(0.0, 0.0),
												   Point2<float>(6.0, 0.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 7);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{2, 0, 1});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{3, 0, 2});
	AssertHelper::assert3UnsignedInts(triangles[2].getIndices(), new unsigned int[3]{4, 0, 3});
	AssertHelper::assert3UnsignedInts(triangles[3].getIndices(), new unsigned int[3]{5, 0, 4});
	AssertHelper::assert3UnsignedInts(triangles[4].getIndices(), new unsigned int[3]{6, 0, 5});
	AssertHelper::assert3UnsignedInts(triangles[5].getIndices(), new unsigned int[3]{8, 6, 7});
	AssertHelper::assert3UnsignedInts(triangles[6].getIndices(), new unsigned int[3]{8, 0, 6});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{1, -1, -1});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{2, 0, -1});
	AssertHelper::assert3Ints(triangles[2].getNeighbors(), new int[3]{3, 1, -1});
	AssertHelper::assert3Ints(triangles[3].getNeighbors(), new int[3]{4, 2, -1});
	AssertHelper::assert3Ints(triangles[4].getNeighbors(), new int[3]{6, 3, -1});
	AssertHelper::assert3Ints(triangles[5].getNeighbors(), new int[3]{6, -1, -1});
	AssertHelper::assert3Ints(triangles[6].getNeighbors(), new int[3]{-1, 4, 5});
}

void TriangulationTest::threeAlignedPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 5.0), Point2<float>(-1.0, 4.0), Point2<float>(0.0, 0.0),
												   Point2<float>(1.0, 1.0), Point2<float>(1.0, 2.0), Point2<float>(1.0, 3.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 4);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{1, 5, 0});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{1, 4, 5});
	AssertHelper::assert3UnsignedInts(triangles[2].getIndices(), new unsigned int[3]{1, 3, 4});
	AssertHelper::assert3UnsignedInts(triangles[3].getIndices(), new unsigned int[3]{1, 2, 3});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{1, -1, -1});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{2, -1, 0});
	AssertHelper::assert3Ints(triangles[2].getNeighbors(), new int[3]{3, -1, 1});
	AssertHelper::assert3Ints(triangles[3].getNeighbors(), new int[3]{-1, -1, 2});
}

void TriangulationTest::alternationPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 6.0), Point2<float>(-1.0, 4.0), Point2<float>(-1.0, 2.0),
												   Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 3.0),
												   Point2<float>(1.0, 5.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 5);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{1, 6, 0});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{1, 5, 6});
	AssertHelper::assert3UnsignedInts(triangles[2].getIndices(), new unsigned int[3]{2, 5, 1});
	AssertHelper::assert3UnsignedInts(triangles[3].getIndices(), new unsigned int[3]{2, 4, 5});
	AssertHelper::assert3UnsignedInts(triangles[4].getIndices(), new unsigned int[3]{2, 3, 4});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{1, -1, -1});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{2, -1, 0});
	AssertHelper::assert3Ints(triangles[2].getNeighbors(), new int[3]{3, 1, -1});
	AssertHelper::assert3Ints(triangles[3].getNeighbors(), new int[3]{4, -1, 2});
	AssertHelper::assert3Ints(triangles[4].getNeighbors(), new int[3]{-1, -1, 3});
}

void TriangulationTest::cavityTriangulation1()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(3.0, 4.0), Point2<float>(0.0, 3.0), Point2<float>(1.0, 2.0),
												   Point2<float>(0.0, 1.75), Point2<float>(2.0, 0.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 3);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{2, 0, 1});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{4, 2, 3});
	AssertHelper::assert3UnsignedInts(triangles[2].getIndices(), new unsigned int[3]{4, 0, 2});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{2, -1, -1});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{2, -1, -1});
	AssertHelper::assert3Ints(triangles[2].getNeighbors(), new int[3]{-1, 0, 1});
}

void TriangulationTest::cavityTriangulation2()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(2.0, 0.0), Point2<float>(3.0, 5.0), Point2<float>(0.0, 3.0),
												   Point2<float>(1.0, 2.5), Point2<float>(0.0, 2.0), Point2<float>(1.0, 1.5),
												   Point2<float>(0.0, 1.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 5);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{3, 1, 2});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{5, 3, 4});
	AssertHelper::assert3UnsignedInts(triangles[2].getIndices(), new unsigned int[3]{5, 1, 3});
	AssertHelper::assert3UnsignedInts(triangles[3].getIndices(), new unsigned int[3]{0, 5, 6});
	AssertHelper::assert3UnsignedInts(triangles[4].getIndices(), new unsigned int[3]{0, 1, 5});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{2, -1, -1});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{2, -1, -1});
	AssertHelper::assert3Ints(triangles[2].getNeighbors(), new int[3]{4, 0, 1});
	AssertHelper::assert3Ints(triangles[3].getNeighbors(), new int[3]{4, -1, -1});
	AssertHelper::assert3Ints(triangles[4].getNeighbors(), new int[3]{-1, 2, 3});
}

void TriangulationTest::twoMonotonePolygons()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(4.0, 0.0), Point2<float>(3.0, 2.0),
												   Point2<float>(2.0, 1.0), Point2<float>(1.0, 2.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 3);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{4, 0, 3});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{1, 3, 0});
	AssertHelper::assert3UnsignedInts(triangles[2].getIndices(), new unsigned int[3]{1, 2, 3});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{-1, 1, -1});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{2, 0, -1});
	AssertHelper::assert3Ints(triangles[2].getNeighbors(), new int[3]{-1, -1, 1});
}

void TriangulationTest::threeMonotonePolygons()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(4.0, 0.0), Point2<float>(4.0, 3.0),
												   Point2<float>(3.0, 1.0), Point2<float>(2.0, 2.0), Point2<float>(1.0, 1.0),
												   Point2<float>(0.0, 2.0)};

	TriangulationAlgorithm triangulationAlgorithm(ccwPolygonPoints, "test", TriangulationAlgorithm::CCW);
	std::vector<NavTriangle> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 5);
	AssertHelper::assert3UnsignedInts(triangles[0].getIndices(), new unsigned int[3]{5, 0, 3});
	AssertHelper::assert3UnsignedInts(triangles[1].getIndices(), new unsigned int[3]{6, 0, 5});
	AssertHelper::assert3UnsignedInts(triangles[2].getIndices(), new unsigned int[3]{1, 3, 0});
	AssertHelper::assert3UnsignedInts(triangles[3].getIndices(), new unsigned int[3]{1, 2, 3});
	AssertHelper::assert3UnsignedInts(triangles[4].getIndices(), new unsigned int[3]{3, 4, 5});
	AssertHelper::assert3Ints(triangles[0].getNeighbors(), new int[3]{1, 2, 4});
	AssertHelper::assert3Ints(triangles[1].getNeighbors(), new int[3]{-1, 0, -1});
	AssertHelper::assert3Ints(triangles[2].getNeighbors(), new int[3]{3, 0, -1});
	AssertHelper::assert3Ints(triangles[3].getNeighbors(), new int[3]{-1, -1, 2});
	AssertHelper::assert3Ints(triangles[4].getNeighbors(), new int[3]{-1, -1, 0});
}

CppUnit::Test *TriangulationTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("TriangulationTest");

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("triangleTriangulation", &TriangulationTest::triangleTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cubeTriangulation", &TriangulationTest::cubeTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("twoNearPoints", &TriangulationTest::twoNearPoints));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("threeAlignedPoints", &TriangulationTest::threeAlignedPoints));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("alternationPoints", &TriangulationTest::alternationPoints));

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cavityTriangulation1", &TriangulationTest::cavityTriangulation1));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cavityTriangulation2", &TriangulationTest::cavityTriangulation2));

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("twoMonotonePolygons", &TriangulationTest::twoMonotonePolygons));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("threeMonotonePolygons", &TriangulationTest::threeMonotonePolygons));

	return suite;
}
