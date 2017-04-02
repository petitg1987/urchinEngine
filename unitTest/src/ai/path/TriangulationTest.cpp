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

	Triangulation triangulation(ccwPolygonPoints);
	std::vector<IndexedTriangle2D<float>> triangles = triangulation.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 1);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[0], 1);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[1], 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[2], 0);
}

void TriangulationTest::cubeTriangulation()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(0.0, 0.0), Point2<float>(1.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(0.0, 1.0)};

	Triangulation triangulation(ccwPolygonPoints);
	std::vector<IndexedTriangle2D<float>> triangles = triangulation.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[0], 0);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[1], 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[2], 3);
	AssertHelper::assertUnsignedInt(triangles[1].getIndexes()[0], 1);
	AssertHelper::assertUnsignedInt(triangles[1].getIndexes()[1], 2);
	AssertHelper::assertUnsignedInt(triangles[1].getIndexes()[2], 0);
}

void TriangulationTest::cavityTriangulation()
{
	std::vector<Point2<float>> ccwPolygonPoints = {Point2<float>(3.0, 4.0), Point2<float>(0.0, 3.0), Point2<float>(1.0, 2.0), Point2<float>(0.0, 1.75), Point2<float>(2.0, 0.0)};

	Triangulation triangulation(ccwPolygonPoints);
	std::vector<IndexedTriangle2D<float>> triangles = triangulation.triangulate();

	AssertHelper::assertUnsignedInt(triangles.size(), 3);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[0], 2);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[1], 0);
	AssertHelper::assertUnsignedInt(triangles[0].getIndexes()[2], 1);
	AssertHelper::assertUnsignedInt(triangles[1].getIndexes()[0], 4);
	AssertHelper::assertUnsignedInt(triangles[1].getIndexes()[1], 2);
	AssertHelper::assertUnsignedInt(triangles[1].getIndexes()[2], 3);
	AssertHelper::assertUnsignedInt(triangles[2].getIndexes()[0], 4);
	AssertHelper::assertUnsignedInt(triangles[2].getIndexes()[1], 0);
	AssertHelper::assertUnsignedInt(triangles[2].getIndexes()[2], 2);
}

CppUnit::Test *TriangulationTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("TriangulationTest");

	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("triangleTriangulation", &TriangulationTest::triangleTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cubeTriangulation", &TriangulationTest::cubeTriangulation));
	suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cavityTriangulation", &TriangulationTest::cavityTriangulation));

	return suite;
}
