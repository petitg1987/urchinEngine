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

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 1);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{1, 2, 0});
	AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0).empty());
	AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1).empty());
	AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());
}

void TriangulationTest::cubeTriangulation()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(1.0, 0.0),
												   Point2<float>(1.0, 1.0), Point2<float>(0.0, 1.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 2);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{0, 2, 3});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{1, 2, 0});
	AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0).empty());
	AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[0]);
	AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2).empty());
}

void TriangulationTest::twoNearPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(500.0, 500.0), Point2<float>(0.0, 3.0), Point2<float>(2.0, 3.0), Point2<float>(2.0, 2.0),
												   Point2<float>(2.0000002, 2.0), Point2<float>(3.0, 1.5), Point2<float>(5.0, 1.5), Point2<float>(0.0, 0.0),
												   Point2<float>(6.0, 0.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 7);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{2, 0, 1});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{3, 0, 2});
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[0]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{4, 0, 3});
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[3]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{5, 0, 4});
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[4]);
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{6, 0, 5});
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[6]);
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[3]);
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[5]->getIndices(), new std::size_t[3]{8, 6, 7});
    AssertHelper::assertTrue(triangles[5]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[6]);
    AssertHelper::assertTrue(triangles[5]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[5]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[6]->getIndices(), new std::size_t[3]{8, 0, 6});
    AssertHelper::assertTrue(triangles[6]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[6]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[4]);
    AssertHelper::assertTrue(triangles[6]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[5]);
}

void TriangulationTest::threeAlignedPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 5.0), Point2<float>(-1.0, 4.0), Point2<float>(0.0, 0.0),
												   Point2<float>(1.0, 1.0), Point2<float>(1.0, 2.0), Point2<float>(1.0, 3.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 4);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{1, 5, 0});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{1, 4, 5});
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[0]);

	AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{1, 3, 4});
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[3]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[1]);

	AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{1, 2, 3});
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[2]);
}

void TriangulationTest::alternationPoints()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 6.0), Point2<float>(-1.0, 4.0), Point2<float>(-1.0, 2.0),
												   Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 3.0),
												   Point2<float>(1.0, 5.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 5);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{1, 6, 0});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{1, 5, 6});
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[0]);

	AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{2, 5, 1});
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[3]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{2, 4, 5});
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[4]);
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[2]);

	AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{2, 3, 4});
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[3]);
}

void TriangulationTest::cavityTriangulation1()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(3.0, 4.0), Point2<float>(0.0, 3.0), Point2<float>(1.0, 2.0),
												   Point2<float>(0.0, 1.75), Point2<float>(2.0, 0.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 3);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{2, 0, 1});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{4, 2, 3});
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{4, 0, 2});
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[0]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[1]);
}

void TriangulationTest::cavityTriangulation2()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(2.0, 0.0), Point2<float>(3.0, 5.0), Point2<float>(0.0, 3.0),
												   Point2<float>(1.0, 2.5), Point2<float>(0.0, 2.0), Point2<float>(1.0, 1.5),
												   Point2<float>(0.0, 1.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 5);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{3, 1, 2});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{5, 3, 4});
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{5, 1, 3});
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[4]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[0]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[1]);

	AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{0, 5, 6});
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[4]);
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{0, 1, 5});
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[3]);
}

void TriangulationTest::twoMonotonePolygons()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(4.0, 0.0), Point2<float>(3.0, 2.0),
												   Point2<float>(2.0, 1.0), Point2<float>(1.0, 2.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 3);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{4, 0, 3});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{1, 3, 0});
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[0]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{1, 2, 3});
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[1]);
}

void TriangulationTest::threeMonotonePolygons()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(4.0, 0.0), Point2<float>(4.0, 3.0),
												   Point2<float>(3.0, 1.0), Point2<float>(2.0, 2.0), Point2<float>(1.0, 1.0),
												   Point2<float>(0.0, 2.0)};

	TriangulationAlgorithm triangulationAlgorithm(std::move(ccwPolygonPoints), "test", TriangulationAlgorithm::CCW);
	std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 5);
	AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{5, 0, 3});
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[1]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[2]);
    AssertHelper::assertTrue(triangles[0]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[4]);

	AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{6, 0, 5});
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[0]);
    AssertHelper::assertTrue(triangles[1]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{1, 3, 0});
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(0)[0].getTargetTriangle() == triangles[3]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(1)[0].getTargetTriangle() == triangles[0]);
    AssertHelper::assertTrue(triangles[2]->getEdgeLinks(2).empty());

	AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{1, 2, 3});
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[3]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[2]);

	AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{3, 4, 5});
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(0).empty());
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(1).empty());
    AssertHelper::assertTrue(triangles[4]->getEdgeLinks(2)[0].getTargetTriangle() == triangles[0]);
}

CppUnit::Test *TriangulationTest::suite()
{
	auto *suite = new CppUnit::TestSuite("TriangulationTest");

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
