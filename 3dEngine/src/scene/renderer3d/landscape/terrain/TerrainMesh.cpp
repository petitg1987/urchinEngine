#include <thread>
#include <functional>
#include <fstream>

#include "graphics/api/GraphicsApi.h"
#include "scene/renderer3d/landscape/terrain/TerrainMesh.h"
#include "resources/ResourceRetriever.h"

namespace urchin {

    TerrainMesh::TerrainMesh(std::string heightFilename, float xzScale, float yScale) :
            xzScale(xzScale),
            yScale(yScale) {
        this->heightFilename = FileUtil::isAbsolutePath(heightFilename) ? std::move(heightFilename) : FileSystem::instance().getResourcesDirectory() + std::move(heightFilename);
        auto imgTerrain = ResourceRetriever::instance().getResource<Image>(this->heightFilename);
        if (imgTerrain->getImageFormat() != Image::IMAGE_GRAYSCALE) {
            throw std::runtime_error("Height map must be grayscale. Image format: " + std::to_string(imgTerrain->getImageFormat()));
        }
        
        xSize = imgTerrain->getWidth();
        zSize = imgTerrain->getHeight();

        std::string terrainHash = generateTerrainMeshHash(this->heightFilename, xzScale, yScale);

        std::string terrainFrlFilePath = FileSystem::instance().getEngineUserCacheDirectory()
                + FileUtil::getFileNameNoExtension(this->heightFilename)
                + "_" + std::to_string(std::hash<std::string>{}(this->heightFilename))
                + ".frl"; //extension for FRL files (Fast Resource Loading)
        std::ifstream terrainFrlFile;
        terrainFrlFile.open(terrainFrlFilePath, std::ios::in | std::ios::binary);

        if (terrainFrlFile.is_open() && readVersion(terrainFrlFile) == TERRAIN_FRL_FILE_VERSION && readHash(terrainFrlFile) == terrainHash) {
            loadTerrainMeshFile(terrainFrlFile);
        } else {
            terrainFrlFile.close();

            buildVertices(*imgTerrain);
            buildIndices();
            buildNormals();

            writeTerrainMeshFile(terrainFrlFilePath, terrainHash);
        }

        heightfieldPointHelper = std::make_unique<HeightfieldPointHelper<float>>(vertices, xSize);
    }

    std::string TerrainMesh::generateTerrainMeshHash(const std::string& terrainFilePath, float xzScale, float yScale) {
        std::size_t terrainHashInt = std::hash<std::string>{}(FileReader::readFile(terrainFilePath) + "#" + std::to_string(xzScale) + "#" + std::to_string(yScale));
        std::string terrainHash = std::to_string(terrainHashInt);
        return std::string(TERRAIN_HASH_SIZE - terrainHash.length(), '0') + terrainHash;
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

    /**
     * @return Terrain local vertices. First point is the far left point (min X, min Z) and the last pint the near right point (max X, max Z).
     */
    const std::vector<Point3<float>>& TerrainMesh::getVertices() const {
        return vertices;
    }

    const std::vector<Vector3<float>>& TerrainMesh::getNormals() const {
        return normals;
    }

    const std::vector<unsigned int>& TerrainMesh::getIndices() const {
        return indices;
    }

    Point3<float> TerrainMesh::findPointAt(const Point2<float>& xzCoordinate) const {
        return heightfieldPointHelper->findPointAt(xzCoordinate);
    }

    float TerrainMesh::findHeightAt(const Point2<float>& xzCoordinate) const {
        return heightfieldPointHelper->findHeightAt(xzCoordinate);
    }

    unsigned int TerrainMesh::computeNumberVertices() const {
        return xSize * zSize;
    }

    unsigned int TerrainMesh::computeNumberIndices() const {
        unsigned int restartIndices = zSize - 1;
        return ((zSize - 1) * xSize * 2) + restartIndices;
    }

    unsigned int TerrainMesh::computeNumberTriangles() const {
        unsigned int trianglesByRow = (xSize - 1) * 2;
        return trianglesByRow * (zSize - 1);
    }

    unsigned int TerrainMesh::computeNumberVertexNormals() const {
        return xSize * zSize;
    }

    std::vector<Point3<float>> TerrainMesh::buildVertices(const Image& imgTerrain) {
        vertices.reserve(computeNumberVertices());

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
                vertices.emplace_back(xFloat, elevation, zFloat);
            }
        }

        return vertices;
    }

    std::vector<unsigned int> TerrainMesh::buildIndices() {
        indices.reserve(computeNumberIndices());

        for (unsigned int z = 0; z < zSize - 1; ++z) {
            for (unsigned int x = 0; x < xSize; ++x) {
                indices.push_back(x + xSize * (z + 1));
                indices.push_back(x + xSize * z);
            }

            indices.push_back(GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE);
        }
        return indices;
    }

    std::vector<Vector3<float>> TerrainMesh::buildNormals() {
        const unsigned int NUM_THREADS = std::max(2u, std::thread::hardware_concurrency());

        //1. compute normal of triangles
        unsigned int totalTriangles = computeNumberTriangles();
        unsigned int indicesByRow = (xSize * 2) + 1 /* strip restart */;
        unsigned int trianglesByRow = (xSize - 1) * 2;
        std::vector<Vector3<float>> normalTriangles;
        normalTriangles.resize(totalTriangles);
        std::vector<std::jthread> threadsNormalTriangle(NUM_THREADS);
        for (unsigned int threadI = 0; threadI < NUM_THREADS; threadI++) {
            unsigned int beginTriangleIndex = threadI * totalTriangles / NUM_THREADS;
            unsigned int endTriangleIndex = (threadI + 1) == NUM_THREADS ? totalTriangles : (threadI + 1) * totalTriangles / NUM_THREADS;

            threadsNormalTriangle[threadI] = std::jthread([&] {
                for (unsigned int triangleIndex = beginTriangleIndex; triangleIndex < endTriangleIndex; triangleIndex++) {
                    unsigned int triangleZValue = triangleIndex / trianglesByRow;
                    unsigned int triangleXValue = triangleIndex % trianglesByRow;
                    unsigned int indicesStartIndex = triangleZValue * indicesByRow + triangleXValue;

                    Point3<float> point1 = vertices[indices[indicesStartIndex]];
                    Point3<float> point2 = vertices[indices[indicesStartIndex + 1]];
                    Point3<float> point3 = vertices[indices[indicesStartIndex + 2]];

                    bool isCwTriangle = (indicesStartIndex % indicesByRow) % 2 == 0;
                    Vector3<float> normal;
                    if (isCwTriangle) {
                        normal = (point1.vector(point2).crossProduct(point3.vector(point1)));
                    } else {
                        normal = (point1.vector(point2).crossProduct(point1.vector(point3)));
                    }

                    normalTriangles[triangleIndex] = normal.normalize();
                }
            });
        }
        std::ranges::for_each(threadsNormalTriangle, [](std::jthread& x){x.join();});
        assert(totalTriangles == normalTriangles.size());

        //2. compute normal of vertex
        normals.resize(computeNumberVertexNormals());
        auto totalNormalVertex = (unsigned int)vertices.size();
        std::vector<std::jthread> threadsNormalVertex(NUM_THREADS);
        for (unsigned int threadI = 0; threadI < NUM_THREADS; threadI++) {
            unsigned int beginVertexIndex = threadI * totalNormalVertex / NUM_THREADS;
            unsigned int endVertexIndex = (threadI + 1) == NUM_THREADS ? totalNormalVertex : (threadI + 1) * totalNormalVertex / NUM_THREADS;

            threadsNormalVertex[threadI] = std::jthread([&] {
                for (unsigned int vertexIndex = beginVertexIndex; vertexIndex < endVertexIndex; vertexIndex++) {
                    Vector3<float> vertexNormal(0.0, 0.0, 0.0);
                    for (unsigned int triangleIndex : findTriangleIndices(vertexIndex)) {
                        vertexNormal += normalTriangles[triangleIndex];
                    }
                    normals[vertexIndex] = vertexNormal.normalize();
                }
            });
        }
        std::ranges::for_each(threadsNormalVertex, [](std::jthread& x){x.join();});

        return normals;
    }

    std::vector<unsigned int> TerrainMesh::findTriangleIndices(unsigned int vertexIndex) const {
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

    void TerrainMesh::writeTerrainMeshFile(const std::string& filePath, const std::string& terrainHash) const {
        std::ofstream file;
        file.open(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            throw UserAuthorityException("Unable to open file: " + filePath, "Check that the application has enough right to create the file: " + filePath);
        }

        writeVersion(file, TERRAIN_FRL_FILE_VERSION);
        writeHash(file, terrainHash);

        file.write(reinterpret_cast<const char*>(vertices.data()), (int)(vertices.size() * sizeof(float) * 3));
        file.write(reinterpret_cast<const char*>(indices.data()), (int)(indices.size() * sizeof(unsigned int)));
        file.write(reinterpret_cast<const char*>(normals.data()), (int)(normals.size() * sizeof(float) * 3));

        file.close();
    }

    void TerrainMesh::writeVersion(std::ofstream& file, unsigned int version) const {
        file.write(reinterpret_cast<char*>(&version), sizeof(version));
    }

    void TerrainMesh::writeHash(std::ofstream& file, const std::string& hash) const {
        file.write(hash.c_str(), (int)(hash.size() * sizeof(char)));
    }

    void TerrainMesh::loadTerrainMeshFile(std::ifstream& file) {
        vertices.resize(computeNumberVertices());
        file.read(reinterpret_cast<char*>(vertices.data()), (int)(vertices.size() * sizeof(float) * 3));

        indices.resize(computeNumberIndices());
        file.read(reinterpret_cast<char*>(indices.data()), (int)(indices.size() * sizeof(unsigned int)));

        normals.resize(computeNumberVertexNormals());
        file.read(reinterpret_cast<char*>(normals.data()), (int)(normals.size() * sizeof(float) * 3));

        file.close();
    }

    unsigned int TerrainMesh::readVersion(std::ifstream& file) const {
        unsigned int version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        return version;
    }

    std::string TerrainMesh::readHash(std::ifstream& file) const {
        std::string hash(TERRAIN_HASH_SIZE, '\0');
        file.read(hash.data(), TERRAIN_HASH_SIZE);
        return hash;
    }

}
