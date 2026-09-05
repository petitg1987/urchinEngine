#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>

#include "common/math/geometry/3d/util/MeshDataServiceTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MeshDataServiceTest::mergeDuplicateVertices() {
    std::vector vertices = {
        Point3(0.0f, 0.0f, 0.0f),
        Point3(0.0f, 0.0f, 0.0f),
        Point3(2.0f, 0.0f, 0.0f),
        Point3(0.0f, 2.0f, 0.0f),
        Point3(-2.0f, 0.0f, 0.0f)
    };
    std::vector<std::array<uint32_t, 3>> trianglesIndices = {
        {0, 3, 2},
        {1, 4, 3},
    };
    MeshData mesh(vertices, trianglesIndices);

    MeshData simplifiedMesh = MeshDataService::mergeDuplicateVertices(mesh);

    AssertHelper::assertUnsignedIntEquals(simplifiedMesh.getVertices().size(), 4);
    AssertHelper::assertUnsignedIntEquals(simplifiedMesh.getTrianglesIndices().size(), 2);
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(0, simplifiedMesh), std::array{
        Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 2.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f)
    });
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(1, simplifiedMesh), std::array{
        Point3(0.0f, 0.0f, 0.0f), Point3(-2.0f, 0.0f, 0.0f), Point3(0.0f, 2.0f, 0.0f)
    });
}

void MeshDataServiceTest::mergeDuplicateVerticesWithCollapsedTriangle() {
    std::vector vertices = {
        Point3(0.0f, 0.0f, 0.0f),
        Point3(2.0f, 0.0f, 0.0f),
        Point3(0.0f, 2.0f, 0.0f),
        Point3(5.0f, 5.0f, 5.0f),
        Point3(5.0f, 5.0f, 5.0f),
        Point3(5.0f, 5.0f, 5.0f),
        Point3(0.0f, 0.0f, 0.0f)
    };
    std::vector<std::array<uint32_t, 3>> trianglesIndices = {
        {0, 2, 1},
        {3, 4, 5}, //fully collapsed
        {0, 6, 1} //partially collapsed
    };
    MeshData mesh(vertices, trianglesIndices);

    MeshData simplifiedMesh = MeshDataService::mergeDuplicateVertices(mesh);

    AssertHelper::assertUnsignedIntEquals(simplifiedMesh.getTrianglesIndices().size(), 1);
    AssertHelper::assertPoints3FloatEquals(extractTrianglePoints(0, simplifiedMesh), std::array{
        Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 2.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f)
    });
}

void MeshDataServiceTest::downsampleVertices() {
    std::vector vertices = {
        Point3(0.0f, 0.0f, 0.0f),
        Point3(0.05f, 0.0f, 0.0f), //too close from the first vertex
        Point3(1.0f, 0.0f, 0.0f),
        Point3(0.0f, 1.0f, 0.0f),
        Point3(0.99f, 0.0f, 0.0f) //too close from the third vertex
    };

    std::vector<Point3<float>> simplifiedVertices = MeshDataService::downsampleVertices(vertices, 0.1f);

    AssertHelper::assertUnsignedIntEquals(simplifiedVertices.size(), 3);
    AssertHelper::assertPoints3FloatEquals(simplifiedVertices, std::array{
        Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f), Point3(0.0f, 1.0f, 0.0f)
    });
}

std::array<Point3<float>, 3> MeshDataServiceTest::extractTrianglePoints(std::size_t triangleIndex, const MeshData& mesh) const {
    const std::array<uint32_t, 3>& triangleIndices = mesh.getTrianglesIndices()[triangleIndex];
    return {mesh.getVertices()[triangleIndices[0]], mesh.getVertices()[triangleIndices[1]], mesh.getVertices()[triangleIndices[2]]};
}

CppUnit::Test* MeshDataServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("MeshDataServiceTest");

    suite->addTest(new CppUnit::TestCaller("mergeDuplicateVertices", &MeshDataServiceTest::mergeDuplicateVertices));
    suite->addTest(new CppUnit::TestCaller("mergeDuplicateVerticesWithCollapsedTriangle", &MeshDataServiceTest::mergeDuplicateVerticesWithCollapsedTriangle));

    suite->addTest(new CppUnit::TestCaller("downsampleVertices", &MeshDataServiceTest::downsampleVertices));

    return suite;
}

