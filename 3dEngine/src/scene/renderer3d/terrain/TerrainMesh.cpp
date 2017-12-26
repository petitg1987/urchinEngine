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
            throw std::runtime_error("Height map must be grayscale. Image format: " + std::to_string(imgTerrain->getImageFormat()));
        }
        
        xSize = imgTerrain->getWidth();
        zSize = imgTerrain->getHeight();

        buildVertices(imgTerrain);
        buildIndices();
        buildNormals();

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
        //1. compute normal of triangles
        unsigned int totalTriangles = ((zSize - 1) * (xSize - 1)) * 2;
        std::vector<Vector3<float>> normalTriangles;
        normalTriangles.reserve(totalTriangles);

        bool isCwTriangle = true;
        for(unsigned int i=0; i<indices.size() - 2; ++i)
        {
            if(indices[i+2] != RESTART_INDEX)
            {
                Point3<float> point1 = vertices[indices[i]];
                Point3<float> point2 = vertices[indices[i+1]];
                Point3<float> point3 = vertices[indices[i+2]];

                Vector3<float> normal;
                if(isCwTriangle)
                {
                    normal = (point1.vector(point2).crossProduct(point3.vector(point1)));
                }else
                {
                    normal = (point1.vector(point2).crossProduct(point1.vector(point3)));
                }

                normalTriangles.push_back(normal.normalize());
                isCwTriangle = !isCwTriangle;
            }else
            {
                i += 2;
            }
        }
        #ifdef _DEBUG
            assert(totalTriangles == normalTriangles.size());
        #endif

        //2. compute normal of vertex
        normals.reserve(vertices.size());
        for(unsigned int i=0; i<vertices.size(); ++i)
        {
            std::vector<unsigned int> triangleIndices = findTriangleIndices(i);

            Vector3<float> vertexNormal(0.0, 0.0, 0.0);
            for(unsigned int triangleIndex : triangleIndices)
            {
                vertexNormal += normalTriangles[triangleIndex];
            }
            vertexNormal /= triangleIndices.size();
            normals.emplace_back(vertexNormal.normalize());
        }

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
