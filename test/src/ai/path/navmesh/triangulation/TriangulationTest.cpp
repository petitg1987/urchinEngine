#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include <ai/path/navmesh/triangulation/TriangulationTest.h>
#include <AssertHelper.h>
using namespace urchin;

void TriangulationTest::triangleTriangulation() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(0.0f, 0.0f), Point2<float>(2.0f, 0.0f), Point2<float>(1.0f, 1.0f)}
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 1);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{1, 2, 0});
    AssertHelper::assertTrue(triangles[0]->getLinks().empty());
}

void TriangulationTest::cubeTriangulation() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)}
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 2);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{2, 3, 0});
    assertUniqueLink(triangles[0], 2, triangles[1]);

    AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{0, 1, 2});
    assertUniqueLink(triangles[1], 2, triangles[0]);
}

void TriangulationTest::twoNearPoints() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(500.0f, 500.0f), Point2<float>(0.0f, 3.0f), Point2<float>(2.0f, 3.0f), Point2<float>(2.0f, 2.0f),
             Point2<float>(2.0000002f, 2.0f), Point2<float>(3.0f, 1.5f), Point2<float>(5.0f, 1.5f), Point2<float>(0.0f, 0.0f),
             Point2<float>(6.0f, 0.0f)}
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 7);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{0, 1, 2});
    assertUniqueLink(triangles[0], 2, triangles[4]);

    AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{2, 3, 4});
    assertUniqueLink(triangles[1], 2, triangles[4]);

    AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{4, 5, 6});
    assertUniqueLink(triangles[2], 2, triangles[6]);

    AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{6, 7, 8});
    assertUniqueLink(triangles[3], 2, triangles[5]);

    AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{0, 2, 4});
    AssertHelper::assertUnsignedInt(triangles[4]->getLinks().size(), 3);
    assertLink(triangles[4]->getLinks()[0], 0, triangles[0]);
    assertLink(triangles[4]->getLinks()[1], 1, triangles[1]);
    assertLink(triangles[4]->getLinks()[2], 2, triangles[6]);

    AssertHelper::assert3Sizes(triangles[5]->getIndices(), new std::size_t[3]{6, 8, 0});
    AssertHelper::assertUnsignedInt(triangles[5]->getLinks().size(), 2);
    assertLink(triangles[5]->getLinks()[0], 0, triangles[3]);
    assertLink(triangles[5]->getLinks()[1], 2, triangles[6]);

    AssertHelper::assert3Sizes(triangles[6]->getIndices(), new std::size_t[3]{0, 4, 6});
    AssertHelper::assertUnsignedInt(triangles[6]->getLinks().size(), 3);
    assertLink(triangles[6]->getLinks()[0], 0, triangles[4]);
    assertLink(triangles[6]->getLinks()[1], 1, triangles[2]);
    assertLink(triangles[6]->getLinks()[2], 2, triangles[5]);
}

void TriangulationTest::threeAlignedPoints() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(0.0f, 5.0f), Point2<float>(-1.0f, 4.0f), Point2<float>(0.0f, 0.0f),
             Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 2.0f), Point2<float>(1.0f, 3.0f)}
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 4);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{4, 5, 0});
    assertUniqueLink(triangles[0], 2, triangles[3]);

    AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{0, 1, 2});
    assertUniqueLink(triangles[1], 2, triangles[3]);

    AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{2, 3, 4});
    assertUniqueLink(triangles[2], 2, triangles[3]);

    AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{4, 0, 2});
    AssertHelper::assertUnsignedInt(triangles[3]->getLinks().size(), 3);
    assertLink(triangles[3]->getLinks()[0], 0, triangles[0]);
    assertLink(triangles[3]->getLinks()[1], 1, triangles[1]);
    assertLink(triangles[3]->getLinks()[2], 2, triangles[2]);
}

void TriangulationTest::alternationPoints() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(0.0f, 6.0f), Point2<float>(-1.0f, 4.0f), Point2<float>(-1.0f, 2.0f),
             Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 3.0f),
             Point2<float>(1.0f, 5.0f)}
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 5);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{5, 6, 0});
    assertUniqueLink(triangles[0], 2, triangles[3]);

    AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{0, 1, 2});
    assertUniqueLink(triangles[1], 2, triangles[4]);

    AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{2, 3, 4});
    assertUniqueLink(triangles[2], 2, triangles[4]);

    AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{4, 5, 0});
    AssertHelper::assertUnsignedInt(triangles[3]->getLinks().size(), 2);
    assertLink(triangles[3]->getLinks()[0], 1, triangles[0]);
    assertLink(triangles[3]->getLinks()[1], 2, triangles[4]);

    AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{0, 2, 4});
    AssertHelper::assertUnsignedInt(triangles[4]->getLinks().size(), 3);
    assertLink(triangles[4]->getLinks()[0], 0, triangles[1]);
    assertLink(triangles[4]->getLinks()[1], 1, triangles[2]);
    assertLink(triangles[4]->getLinks()[2], 2, triangles[3]);
}

void TriangulationTest::cavityTriangulation1() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(3.0f, 4.0f), Point2<float>(0.0f, 3.0f), Point2<float>(1.0f, 2.0f),
             Point2<float>(0.0f, 1.75f), Point2<float>(2.0f, 0.0f)}
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 3);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{0, 1, 2});
    AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{2, 3, 4});
    AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{4, 0, 2});
}

void TriangulationTest::cavityTriangulation2() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(2.0f, 0.0f), Point2<float>(3.0f, 5.0f), Point2<float>(0.0f, 3.0f),
             Point2<float>(1.0f, 2.5f), Point2<float>(0.0f, 2.0f), Point2<float>(1.0f, 1.5f),
             Point2<float>(0.0f, 1.0f)}
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 5);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{5, 6, 0});
    AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{1, 2, 3});
    AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{3, 4, 5});
    AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{5, 0, 1});
    AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{1, 3, 5});
}

void TriangulationTest::holeInTriangle() {
    std::vector<std::vector<Point2<float>>> polygonPoints = {
            {Point2<float>(0.0f, 6.0f), Point2<float>(-5.0f, 0.0f), Point2<float>(5.0f, 0.0f)},
            {Point2<float>(0.0f, 3.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)}, //hole
    };

    TriangulationAlgorithm triangulationAlgorithm(std::move(polygonPoints));
    std::vector<std::shared_ptr<NavTriangle>> triangles = triangulationAlgorithm.triangulate();

    AssertHelper::assertUnsignedInt(triangles.size(), 6);
    AssertHelper::assert3Sizes(triangles[0]->getIndices(), new std::size_t[3]{0, 1, 5});
    AssertHelper::assert3Sizes(triangles[1]->getIndices(), new std::size_t[3]{4, 5, 1});
    AssertHelper::assert3Sizes(triangles[2]->getIndices(), new std::size_t[3]{0, 5, 3});
    AssertHelper::assert3Sizes(triangles[3]->getIndices(), new std::size_t[3]{4, 1, 2});
    AssertHelper::assert3Sizes(triangles[4]->getIndices(), new std::size_t[3]{2, 0, 3});
    AssertHelper::assert3Sizes(triangles[5]->getIndices(), new std::size_t[3]{3, 4, 2});
}

CppUnit::Test* TriangulationTest::suite() {
    auto* suite = new CppUnit::TestSuite("TriangulationTest");

    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("triangleTriangulation", &TriangulationTest::triangleTriangulation));
    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cubeTriangulation", &TriangulationTest::cubeTriangulation));
    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("twoNearPoints", &TriangulationTest::twoNearPoints));
    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("threeAlignedPoints", &TriangulationTest::threeAlignedPoints));
    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("alternationPoints", &TriangulationTest::alternationPoints));

    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cavityTriangulation1", &TriangulationTest::cavityTriangulation1));
    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("cavityTriangulation2", &TriangulationTest::cavityTriangulation2));

    suite->addTest(new CppUnit::TestCaller<TriangulationTest>("holeInTriangle", &TriangulationTest::holeInTriangle));

    return suite;
}

void TriangulationTest::assertUniqueLink(const std::shared_ptr<NavTriangle>& sourceTriangle, std::size_t sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle) {
    AssertHelper::assertUnsignedInt(sourceTriangle->getLinks().size(), 1);
    assertLink(sourceTriangle->getLinks()[0], sourceEdgeIndex, targetTriangle);
}

void TriangulationTest::assertLink(const std::shared_ptr<NavLink>& link, std::size_t sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle) {
    AssertHelper::assertUnsignedInt(link->getSourceEdgeIndex(), sourceEdgeIndex);
    AssertHelper::assertTrue(link->getTargetTriangle() == targetTriangle);
}
