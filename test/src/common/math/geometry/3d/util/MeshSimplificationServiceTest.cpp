#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>

#include <common/math/geometry/3d/util/MeshSimplificationServiceTest.h>
#include <AssertHelper.h>
using namespace urchin;

void MeshSimplificationServiceTest::simplifyOneEdge() {
    std::vector vertices = {
        Point3(2.0f, 0.0f, 0.0f),
        Point3(0.0f, 0.0f, 0.0f),
        Point3(0.0f, 1.0f, 0.0f),
        Point3(0.0f, 1.5f, 0.0f),
        Point3(-2.0f, 0.0f, 0.0f),
    };
    std::vector<std::array<uint32_t, 3>> trianglesIndices = {
        {0, 1, 2},
        {0, 2, 3}, //edge (2, 3) to collapse
        {4, 2, 1}, //required for manifold mesh
        {4, 3, 2} //required for manifold mesh & edge (3, 2) to collapse
    };
    MeshData mesh(vertices, trianglesIndices);

    MeshData simplifiedMesh = MeshSimplificationService({.edgeDistanceThreshold = 0.6f}).simplify(mesh);

    AssertHelper::assertUnsignedIntEquals(simplifiedMesh.getVertices().size(), 4);
    AssertHelper::assertUnsignedIntEquals(simplifiedMesh.getTrianglesIndices().size(), 2);
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(0, simplifiedMesh), std::array{
        Point3(2.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 1.25f, 0.0f)
    });
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(1, simplifiedMesh), std::array{
        Point3(-2.0f, 0.0f, 0.0f), Point3(0.0f, 1.25f, 0.0f), Point3(0.0f, 0.0f, 0.0f)
    });
}

void MeshSimplificationServiceTest::simplifyTwoConsecutiveEdges() {
    std::vector vertices = {
        Point3(2.0f, 0.0f, 0.0f),
        Point3(0.0f, 0.0f, 0.0f),
        Point3(0.0f, 1.0f, 0.0f),
        Point3(0.0f, 1.25f, 0.0f),
        Point3(0.0f, 1.5f, 0.0f),
        Point3(0.0f, 3.0f, 0.0f),
        Point3(-2.0f, 0.0f, 0.0f),
    };
    std::vector<std::array<uint32_t, 3>> trianglesIndices = {
        {0, 1, 2},
        {0, 2, 3}, //edge (2, 3) to collapse
        {0, 3, 4}, //edge (3, 4) to collapse
        {0, 4, 5},
        {6, 2, 1}, //required for manifold mesh
        {6, 3, 2}, //required for manifold mesh & edge (3, 2) to collapse
        {6, 4, 3}, //required for manifold mesh & edge (4, 3) to collapse
        {6, 5, 4} //required for manifold mesh
    };
    MeshData mesh(vertices, trianglesIndices);

    MeshData simplifiedMesh = MeshSimplificationService({.edgeDistanceThreshold = 0.6f}).simplify(mesh);

    AssertHelper::assertUnsignedIntEquals(simplifiedMesh.getVertices().size(), 5);
    AssertHelper::assertUnsignedIntEquals(simplifiedMesh.getTrianglesIndices().size(), 4);
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(0, simplifiedMesh), std::array{
        Point3(2.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 1.3125f, 0.0f)
    });
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(1, simplifiedMesh), std::array{
        Point3(2.0f, 0.0f, 0.0f), Point3(0.0f, 1.3125f, 0.0f), Point3(0.0f, 3.0f, 0.0f)
    });
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(2, simplifiedMesh), std::array{
        Point3(-2.0f, 0.0f, 0.0f), Point3(0.0f, 1.3125f, 0.0f), Point3(0.0f, 0.0f, 0.0f)
    });
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(3, simplifiedMesh), std::array{
        Point3(-2.0f, 0.0f, 0.0f), Point3(0.0f, 3.0f, 0.0f), Point3(0.0f, 1.3125f, 0.0f)
    });
}

std::array<Point3<float>, 3> MeshSimplificationServiceTest::extractTrianglePoints(std::size_t triangleIndex, const MeshData& mesh) const {
    const std::array<uint32_t, 3>& triangleIndices = mesh.getTrianglesIndices()[triangleIndex];
    return {mesh.getVertices()[triangleIndices[0]], mesh.getVertices()[triangleIndices[1]], mesh.getVertices()[triangleIndices[2]]};
}

CppUnit::Test* MeshSimplificationServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("MeshSimplificationServiceTest");

    suite->addTest(new CppUnit::TestCaller("simplifyOneEdge", &MeshSimplificationServiceTest::simplifyOneEdge));
    suite->addTest(new CppUnit::TestCaller("simplifyTwoConsecutiveEdges", &MeshSimplificationServiceTest::simplifyTwoConsecutiveEdges));

    return suite;
}

