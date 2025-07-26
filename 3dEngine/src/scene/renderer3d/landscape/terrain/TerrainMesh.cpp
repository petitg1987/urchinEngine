#include <thread>

#include "graphics/api/GraphicsApi.h"
#include "scene/renderer3d/landscape/terrain/TerrainMesh.h"
#include "resources/ResourceRetriever.h"

namespace urchin {

    TerrainMesh::TerrainMesh(std::string heightFilename, float xzScale, float yScale, TerrainMeshMode mode) :
            xzScale(xzScale),
            yScale(yScale),
            mode(mode) {
        this->heightFilename = FileUtil::isAbsolutePath(heightFilename) ? std::move(heightFilename) : FileSystem::instance().getResourcesDirectory() + std::move(heightFilename);
        auto imgTerrain = ResourceRetriever::instance().getResource<Image>(this->heightFilename);
        if (imgTerrain->getImageFormat() != Image::IMAGE_GRAYSCALE) {
            throw std::runtime_error("Height map must be grayscale. Image format: " + std::to_string(imgTerrain->getImageFormat()));
        }
        
        xSize = imgTerrain->getWidth();
        zSize = imgTerrain->getHeight();

        buildVertices(*imgTerrain);
        buildIndices();
        buildNormals();

        heightfieldPointHelper = std::make_unique<HeightfieldPointHelper<float>>(rawVertices, xSize);
    }

    const std::string& TerrainMesh::getHeightFilename() const {
        return heightFilename;
    }

    float TerrainMesh::getXZScale() const {
        return xzScale;
    }

    float TerrainMesh::getYScale() const {
        return yScale;
    }

    unsigned int TerrainMesh::getXSize() const {
        return xSize;
    }

    unsigned int TerrainMesh::getZSize() const {
        return zSize;
    }

    TerrainMeshMode TerrainMesh::getMode() const {
        return mode;
    }

    /**
     * @return Terrain local vertices. First point is the far left point (min X, min Z) and the last pint the near right point (max X, max Z).
     */
    const std::vector<Point3<float>>& TerrainMesh::getRawVertices() const {
        return rawVertices;
    }

    const std::vector<Point3<float>>& TerrainMesh::getVertices() const {
        assert(!vertices.empty());
        return vertices;
    }

    const std::vector<unsigned int>& TerrainMesh::getIndices() const {
        assert(!indices.empty());
        return indices;
    }

    const std::vector<Vector3<float>>& TerrainMesh::getNormals() const {
        return normals;
    }

    Point3<float> TerrainMesh::findPointAt(const Point2<float>& xzCoordinate) const {
        return heightfieldPointHelper->findPointAt(xzCoordinate);
    }

    float TerrainMesh::findHeightAt(const Point2<float>& xzCoordinate) const {
        return heightfieldPointHelper->findHeightAt(xzCoordinate);
    }

    unsigned int TerrainMesh::computeNumberRawVertices() const {
        return xSize * zSize;
    }

    unsigned int TerrainMesh::computeNumberVertices() const {
        if (mode == TerrainMeshMode::SMOOTH) {
            return 0;
        } else {
            assert(mode == TerrainMeshMode::FLAT);
            return computeNumberTriangles() * 3;
        }
    }

    unsigned int TerrainMesh::computeNumberIndices() const {
        if (mode == TerrainMeshMode::SMOOTH) {
            unsigned int restartIndices = zSize - 1;
            return ((zSize - 1) * xSize * 2) + restartIndices;
        } else {
            assert(mode == TerrainMeshMode::FLAT);
            return 0;
        }
    }

    unsigned int TerrainMesh::computeNumberTriangles() const {
        unsigned int trianglesByRow = (xSize - 1) * 2;
        return trianglesByRow * (zSize - 1);
    }

    unsigned int TerrainMesh::computeNumberVertexNormals() const {
        if (mode == TerrainMeshMode::SMOOTH) {
            return computeNumberRawVertices();
        } else {
            assert(mode == TerrainMeshMode::FLAT);
            return computeNumberVertices();
        }
    }

    void TerrainMesh::buildVertices(const Image& imgTerrain) {
        std::size_t rawVerticesIndex = 0;
        rawVertices.resize(computeNumberRawVertices());
        float xStart = (-((float)xSize * xzScale) / 2.0f) + (xzScale / 2.0f);
        float zStart = (-((float)zSize * xzScale) / 2.0f) + (xzScale / 2.0f);
        for (unsigned int z = 0; z < zSize; ++z) {
            float zFloat = zStart + (float)z * xzScale;
            for (unsigned int x = 0; x < xSize; ++x) {
                float elevation = 0.0f;
                if (imgTerrain.getChannelPrecision() == Image::CHANNEL_8_INT) {
                    elevation = (float)imgTerrain.getTexels()[x + xSize * z] * yScale;
                } else if (imgTerrain.getChannelPrecision() == Image::CHANNEL_16_INT) {
                    constexpr float scale16BitsTo8Bits = 255.0f / 65535.0f;
                    elevation = (float)imgTerrain.getTexels16Bits()[x + xSize * z] * scale16BitsTo8Bits * yScale;
                }
                float xFloat = xStart + (float)x * xzScale;
                //must match with TerrainMaterial#buildTexCoordinates()
                rawVertices[rawVerticesIndex++] = Point3(xFloat, elevation, zFloat);
            }
        }
        assert(rawVerticesIndex == rawVertices.size());

        if (mode == TerrainMeshMode::FLAT) {
            std::size_t verticesIndex = 0;
            vertices.resize(computeNumberVertices());
            for (unsigned int z = 0; z < zSize - 1; ++z) {
                for (unsigned int x = 0; x < xSize - 1; ++x) {
                    //must match with TerrainMaterial#buildTexCoordinates()
                    vertices[verticesIndex++] = rawVertices[x + xSize * (z + 1)];
                    vertices[verticesIndex++] = rawVertices[x + xSize * z];
                    vertices[verticesIndex++] = rawVertices[x + 1 + xSize * (z + 1)];

                    vertices[verticesIndex++] = rawVertices[x + 1 + xSize * (z + 1)];
                    vertices[verticesIndex++] = rawVertices[x + xSize * z];
                    vertices[verticesIndex++] = rawVertices[x + 1 + xSize * z];
                }
            }
            assert(verticesIndex == vertices.size());
        }
    }

    void TerrainMesh::buildIndices() {
        if (mode == TerrainMeshMode::SMOOTH) {
            std::size_t indicesIndex = 0;
            indices.resize(computeNumberIndices());
            for (unsigned int z = 0; z < zSize - 1; ++z) {
                for (unsigned int x = 0; x < xSize; ++x) {
                    indices[indicesIndex++] = x + xSize * (z + 1);
                    indices[indicesIndex++] = x + xSize * z;
                }
                indices[indicesIndex++] = GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE;
            }
            assert(indicesIndex == indices.size());
        }
    }

    void TerrainMesh::buildNormals() {
        const unsigned int NUM_THREADS = std::max(2u, std::thread::hardware_concurrency());

        //1. compute triangles normal
        unsigned int totalTriangles = computeNumberTriangles();
        unsigned int trianglesByRow = (xSize - 1) * 2;
        std::vector<Vector3<float>> normalTriangles;
        normalTriangles.resize(totalTriangles);
        std::vector<std::jthread> threadsNormalTriangle(NUM_THREADS);
        for (unsigned int threadI = 0; threadI < NUM_THREADS; threadI++) {
            unsigned int beginTriangleIndex = threadI * totalTriangles / NUM_THREADS;
            unsigned int endTriangleIndex = (threadI + 1) == NUM_THREADS ? totalTriangles : (threadI + 1) * totalTriangles / NUM_THREADS;

            threadsNormalTriangle[threadI] = std::jthread([&, beginTriangleIndex, endTriangleIndex] {
                for (unsigned int triangleIndex = beginTriangleIndex; triangleIndex < endTriangleIndex; triangleIndex++) {
                    if (mode == TerrainMeshMode::SMOOTH) {
                        unsigned int indicesByRow = (xSize * 2) + 1 /* strip restart */;
                        unsigned int triangleZValue = triangleIndex / trianglesByRow;
                        unsigned int triangleXValue = triangleIndex % trianglesByRow;
                        unsigned int indicesStartIndex = triangleZValue * indicesByRow + triangleXValue;

                        Point3<float> point1 = rawVertices[indices[indicesStartIndex]];
                        Point3<float> point2 = rawVertices[indices[indicesStartIndex + 1]];
                        Point3<float> point3 = rawVertices[indices[indicesStartIndex + 2]];

                        bool isCwTriangle = (indicesStartIndex % indicesByRow) % 2 == 0;
                        Vector3<float> normal;
                        if (isCwTriangle) {
                            normal = (point1.vector(point2).crossProduct(point3.vector(point1)));
                        } else {
                            normal = (point1.vector(point2).crossProduct(point1.vector(point3)));
                        }

                        normalTriangles[triangleIndex] = normal.normalize();
                    } else {
                        assert(mode == TerrainMeshMode::FLAT);
                        unsigned int startIndex = triangleIndex * 3;

                        Point3<float> point1 = vertices[startIndex];
                        Point3<float> point2 = vertices[startIndex + 1];
                        Point3<float> point3 = vertices[startIndex + 2];

                        Vector3<float> normal = (point1.vector(point2).crossProduct(point3.vector(point1)));
                        normalTriangles[triangleIndex] = normal.normalize();
                    }
                }
            });
        }
        std::ranges::for_each(threadsNormalTriangle, [](std::jthread& x){x.join();});
        assert(totalTriangles == normalTriangles.size());

        //2. compute vertex normal
        if (mode == TerrainMeshMode::SMOOTH) {
            normals.resize(computeNumberRawVertices());
            unsigned int totalVertexNormal = (unsigned int)normals.size();
            std::vector<std::jthread> threadsVertexNormal(NUM_THREADS);
            for (unsigned int threadI = 0; threadI < NUM_THREADS; threadI++) {
                unsigned int beginVertexIndex = threadI * totalVertexNormal / NUM_THREADS;
                unsigned int endVertexIndex = (threadI + 1) == NUM_THREADS ? totalVertexNormal : (threadI + 1) * totalVertexNormal / NUM_THREADS;

                threadsVertexNormal[threadI] = std::jthread([&, beginVertexIndex, endVertexIndex] {
                    for (unsigned int vertexIndex = beginVertexIndex; vertexIndex < endVertexIndex; vertexIndex++) {
                        Vector3<float> vertexNormal(0.0, 0.0, 0.0);
                        for (unsigned int triangleIndex : findTriangleIndices(vertexIndex)) {
                            vertexNormal += normalTriangles[triangleIndex];
                        }
                        normals[vertexIndex] = vertexNormal.normalize();
                    }
                });
            }
            std::ranges::for_each(threadsVertexNormal, [](std::jthread& x){x.join();});
        } else {
            assert(mode == TerrainMeshMode::FLAT);
            normals.resize(computeNumberVertices());
            for (std::size_t vertexIndex = 0; vertexIndex < normals.size(); vertexIndex++) {
                normals[vertexIndex] = normalTriangles[vertexIndex / 3];
            }
        }
    }

    std::vector<unsigned int> TerrainMesh::findTriangleIndices(unsigned int vertexIndex) const {
        assert(mode == TerrainMeshMode::SMOOTH);

        std::vector<unsigned int> triangleIndices;
        triangleIndices.reserve(6);

        unsigned int rowNum = vertexIndex / xSize;
        long squareIndex = vertexIndex - xSize - rowNum;

        bool isLeftBorderVertex = (vertexIndex % xSize) == 0;
        bool isRightBorderVertex = ((vertexIndex + 1) % xSize) == 0;
        bool isFirstRowVertex = rowNum == 0;
        bool isLastRowVertex = rowNum == (zSize - 1);

        //above triangles to the vertex
        if (!isFirstRowVertex) {
            long firstLeftTopTriangle = (squareIndex * 2) + 1;

            //left triangle
            if (!isLeftBorderVertex) {
                triangleIndices.emplace_back(firstLeftTopTriangle);
            }

            //right triangles
            if (!isRightBorderVertex) {
                triangleIndices.emplace_back(firstLeftTopTriangle + 1);
                triangleIndices.emplace_back(firstLeftTopTriangle + 2);
            }
        }

        //below triangles to the vertex
        if (!isLastRowVertex) {
            long firstLeftBottomTriangle = (squareIndex * 2) + (xSize - 1) * 2;

            //left triangles
            if (!isLeftBorderVertex) {
                triangleIndices.emplace_back(firstLeftBottomTriangle);
                triangleIndices.emplace_back(firstLeftBottomTriangle + 1);
            }

            //right triangle
            if (!isRightBorderVertex) {
                triangleIndices.emplace_back(firstLeftBottomTriangle + 2);
            }
        }

        return triangleIndices;
    }

}
