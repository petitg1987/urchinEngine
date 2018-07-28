#include <thread>
#include <algorithm>
#include <functional>

#include "TerrainMesh.h"
#include "resources/MediaManager.h"

namespace urchin
{

    TerrainMesh::TerrainMesh(const std::string &heightFilename, float xzScale, float yScale) :
            heightFilename(heightFilename),
            xzScale(xzScale),
            yScale(yScale)
    {
        auto *imgTerrain = MediaManager::instance()->getMedia<Image>(heightFilename);
        if(imgTerrain->getImageFormat() != Image::IMAGE_GRAYSCALE)
        {
            imgTerrain->release();
            throw std::runtime_error("Height map must be grayscale. Image format: " + std::to_string(imgTerrain->getImageFormat()));
        }
        
        xSize = imgTerrain->getWidth();
        zSize = imgTerrain->getHeight();

        buildVertices(imgTerrain);
        buildIndices();
        buildNormals();
        heightfieldPointHelper = std::make_unique<HeightfieldPointHelper<float>>(vertices, xSize);

        imgTerrain->release();
    }

    const std::string &TerrainMesh::getHeightFilename() const
    {
        return heightFilename;
    }

    float TerrainMesh::getXZScale() const
    {
        return xzScale;
    }

    float TerrainMesh::getYScale() const
    {
        return yScale;
    }

    unsigned int TerrainMesh::getXSize() const
    {
        return xSize;
    }

    unsigned int TerrainMesh::getZSize() const
    {
        return zSize;
    }

    const std::vector<Point3<float>> &TerrainMesh::getVertices() const
    {
        return vertices;
    }

    const std::vector<Vector3<float>> &TerrainMesh::getNormals() const
    {
        return normals;
    }

    const std::vector<unsigned int> &TerrainMesh::getIndices() const
    {
        return indices;
    }

    Point3<float> TerrainMesh::findPointAt(const Point2<float> &xzCoordinate) const
    {
        return heightfieldPointHelper->findPointAt(xzCoordinate);
    }

    float TerrainMesh::findHeightAt(const Point2<float> &xzCoordinate) const
    {
        return heightfieldPointHelper->findHeightAt(xzCoordinate);
    }

    std::vector<Point3<float>> TerrainMesh::buildVertices(const Image *imgTerrain)
    {
        unsigned int xLength = imgTerrain->getWidth();;
        unsigned int zLength = imgTerrain->getHeight();

        vertices.reserve(xLength * zLength);

        float xStart = (-(xLength * xzScale) / 2.0) + (xzScale / 2.0);
        float zStart = (-(zLength * xzScale) / 2.0) + (xzScale / 2.0);

        float minElevation = std::numeric_limits<float>::max();
        float maxElevation = -std::numeric_limits<float>::max();
        for(unsigned int z=0; z<zLength; ++z)
        {
            float zFloat = zStart + static_cast<float>(z) * xzScale;
            for (unsigned int x = 0; x < xLength; ++x)
            {
                float elevation = 0.0f;
                if(imgTerrain->getChannelPrecision()==Image::CHANNEL_8)
                {
                    elevation = imgTerrain->getTexels()[x + xLength * z] * yScale;
                }else if(imgTerrain->getChannelPrecision()==Image::CHANNEL_16)
                {
                    constexpr float scale16BitsTo8Bits = 255.0f / 65535.0f;
                    elevation = imgTerrain->getTexels16Bits()[x + xLength * z] * scale16BitsTo8Bits * yScale;
                }

                maxElevation = std::max(maxElevation, elevation);
                minElevation = std::min(minElevation, elevation);

                float xFloat = xStart + static_cast<float>(x) * xzScale;
                vertices.emplace_back(Point3<float>(xFloat, elevation, zFloat));
            }
        }

        //center terrain on Y axis
        float elevationDifference = maxElevation - minElevation;
        float elevationDelta = (elevationDifference / 2.0) - maxElevation;
        for(auto &vertex : vertices)
        {
            vertex.Y += elevationDelta;
        }

        return vertices;
    }

    std::vector<unsigned int> TerrainMesh::buildIndices()
    {
        indices.reserve(((zSize-1) * xSize * 2) + (zSize-1));

        for(unsigned int z = 0; z < zSize - 1; ++z)
        {
            for (unsigned int x = 0; x < xSize; ++x)
            {
                indices.push_back(x + xSize * (z + 1));
                indices.push_back(x + xSize * z);
            }

            indices.push_back(RESTART_INDEX);
        }
        return indices;
    }

    std::vector<Vector3<float>> TerrainMesh::buildNormals()
    {
        const unsigned int NUM_THREADS = std::max(2u, std::thread::hardware_concurrency());

        //1. compute normal of triangles
        unsigned int totalTriangles = ((zSize - 1) * (xSize - 1)) * 2;
        unsigned int xLineQuantity = (xSize * 2) + 1;
        std::vector<Vector3<float>> normalTriangles;
        normalTriangles.resize(totalTriangles);
        unsigned int numLoopNormalTriangle = indices.size() - 2;
        std::vector<std::thread> threadsNormalTriangle(NUM_THREADS);
        for(unsigned int threadI=0; threadI<NUM_THREADS; threadI++)
        {
            unsigned int beginI = threadI * numLoopNormalTriangle / NUM_THREADS;
            unsigned int endI = (threadI + 1)==NUM_THREADS ? numLoopNormalTriangle : (threadI + 1) * numLoopNormalTriangle / NUM_THREADS;
            threadsNormalTriangle[threadI] = std::thread(std::bind([&](unsigned int beginI, unsigned int endI)
            {
                for(unsigned int i = beginI; i<endI; i++)
                {
                    if(indices[i+2] != RESTART_INDEX)
                    {
                        Point3<float> point1 = vertices[indices[i]];
                        Point3<float> point2 = vertices[indices[i+1]];
                        Point3<float> point3 = vertices[indices[i+2]];

                        bool isCwTriangle = (i % xLineQuantity) % 2 == 0;
                        Vector3<float> normal;
                        if(isCwTriangle)
                        {
                            normal = (point1.vector(point2).crossProduct(point3.vector(point1)));
                        }else
                        {
                            normal = (point1.vector(point2).crossProduct(point1.vector(point3)));
                        }

                        unsigned int normalTriangleIndex = i - ((i / xLineQuantity) * 3);
                        normalTriangles[normalTriangleIndex] = normal.normalize();
                    }else
                    {
                        i += 2;
                    }
                }
            }, beginI, endI));
        }
        std::for_each(threadsNormalTriangle.begin(), threadsNormalTriangle.end(), [](std::thread& x){x.join();});
        #ifdef _DEBUG
            assert(totalTriangles == normalTriangles.size());
        #endif

        //2. compute normal of vertex
        normals.resize(vertices.size());
        unsigned int numLoopNormalVertex = vertices.size();
        std::vector<std::thread> threadsNormalVertex(NUM_THREADS);
        for(unsigned int threadI=0; threadI<NUM_THREADS; threadI++)
        {
            unsigned int beginI = threadI * numLoopNormalVertex / NUM_THREADS;
            unsigned int endI = (threadI + 1)==NUM_THREADS ? numLoopNormalVertex : (threadI + 1) * numLoopNormalVertex / NUM_THREADS;

            threadsNormalVertex[threadI] = std::thread(std::bind([&](unsigned int beginI, unsigned int endI)
            {
                for(unsigned int i = beginI; i<endI; i++)
                {
                    std::vector<unsigned int> triangleIndices = findTriangleIndices(i);

                    Vector3<float> vertexNormal(0.0, 0.0, 0.0);
                    for(unsigned int triangleIndex : triangleIndices)
                    {
                        vertexNormal += normalTriangles[triangleIndex];
                    }
                    normals[i] = vertexNormal.normalize();
                }
            }, beginI, endI));
        }
        std::for_each(threadsNormalVertex.begin(), threadsNormalVertex.end(), [](std::thread& x){x.join();});

        return normals;
    }

    std::vector<unsigned int> TerrainMesh::findTriangleIndices(unsigned int vertexIndex) const
    {
        std::vector<unsigned int> triangleIndices;
        triangleIndices.reserve(6);

        unsigned int rowNum = vertexIndex / xSize;
        int squareIndex = vertexIndex - xSize - rowNum;

        bool isLeftBorderVertex = (vertexIndex % xSize) == 0;
        bool isRightBorderVertex = ((vertexIndex + 1) % xSize) == 0;
        bool isFirstRowVertex = rowNum == 0;
        bool isLastRowVertex = rowNum == (zSize - 1);

        //above triangles to the vertex
        if(!isFirstRowVertex)
        {
            int firstLeftTopTriangle = (squareIndex * 2) + 1;

            //left triangle
            if (!isLeftBorderVertex)
            {
                triangleIndices.emplace_back(firstLeftTopTriangle);
            }

            //right triangles
            if (!isRightBorderVertex)
            {
                triangleIndices.emplace_back(firstLeftTopTriangle + 1);
                triangleIndices.emplace_back(firstLeftTopTriangle + 2);
            }
        }

        //below triangles to the vertex
        if(!isLastRowVertex)
        {
            int firstLeftBottomTriangle = (squareIndex * 2) + (xSize - 1) * 2;

            //left triangles
            if (!isLeftBorderVertex)
            {
                triangleIndices.emplace_back(firstLeftBottomTriangle);
                triangleIndices.emplace_back(firstLeftBottomTriangle + 1);
            }

            //right triangle
            if (!isRightBorderVertex)
            {
                triangleIndices.emplace_back(firstLeftBottomTriangle + 2);
            }
        }

        return triangleIndices;
    }
}
