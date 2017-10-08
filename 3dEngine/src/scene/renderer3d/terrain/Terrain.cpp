#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>

#include "Terrain.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"

#define RESTART_INDEX 4294967295 //(2^32)-1

namespace urchin
{
    Terrain::Terrain(const std::string &filename)
    {
        auto *imgTerrain = MediaManager::instance()->getMedia<Image>(filename);
        if(imgTerrain->getImageFormat()!=Image::IMAGE_LUMINANCE)
        {
            throw std::invalid_argument("Unsupported image format for terrain rendering: " + imgTerrain->getImageFormat());
        }

        vertices = buildVertices(imgTerrain);
        indices = buildIndices(imgTerrain);
        normals = buildNormals(imgTerrain);
        imgTerrain->release();

        glGenBuffers(2, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float)*3, &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[VAO_INDEX]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        shader = ShaderManager::instance()->createProgram("terrain.vert", "terrain.frag");
        ShaderManager::instance()->bind(shader);

        mProjectionLoc = glGetUniformLocation(shader, "mProjection");
        mViewLoc = glGetUniformLocation(shader, "mView");
    }

    Terrain::~Terrain()
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(2, bufferIDs);

        ShaderManager::instance()->removeProgram(shader);
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;
    }

    std::vector<Point3<float>> Terrain::buildVertices(const Image *imgTerrain) const
    {
        std::vector<Point3<float>> vertices;
        vertices.reserve(imgTerrain->getHeight() * imgTerrain->getWidth());
        for(unsigned int z=0; z<imgTerrain->getHeight(); ++z)
        {
            float zFloat = static_cast<float>(z) / 5.0f; //TODO hard coded
            for (unsigned int x = 0; x < imgTerrain->getWidth(); ++x)
            {
                float elevation = -2.5 + imgTerrain->getTexels()[x + imgTerrain->getWidth() * z] / 70.0f; //TODO hard coded
                float xFloat = static_cast<float>(x) / 5.0f; //TODO hard coded
                vertices.emplace_back(Point3<float>(xFloat, elevation, zFloat));
            }
        }
        return vertices;
    }

    std::vector<Vector3<float>> Terrain::buildNormals(const Image *imgTerrain) const
    {
        //1. compute normal of triangles
        std::vector<Vector3<float>> normalTriangles;
        //TODO reserve
        //TODO normal by triangle

        //2. compute normal of vertex
        std::vector<Vector3<float>> normalVertex;
        normalVertex.reserve(vertices.size());
        for(unsigned int i=0; i<vertices.size(); ++i)
        {
            std::vector<unsigned int> triangleIndices = findTriangleIndices(i, imgTerrain);

            Vector3<float> vertexNormal(0.0, 0.0, 0.0);
            for(unsigned int triangleIndex : triangleIndices)
            {
                vertexNormal += normalTriangles[triangleIndex];
            }
            vertexNormal /= triangleIndices.size(); //TODO normalize ???

            normalVertex.emplace_back(vertexNormal);
        }

        return normalVertex;
    }

    std::vector<unsigned int> Terrain::findTriangleIndices(unsigned int vertexIndex, const Image *imgTerrain) const
    {
        std::vector<unsigned int> triangleIndices;
        triangleIndices.reserve(6);

        unsigned int rowNum = vertexIndex / imgTerrain->getWidth();
        int squareIndex = vertexIndex - imgTerrain->getWidth() - rowNum;

        bool isLeftBorderVertex = (vertexIndex % imgTerrain->getWidth()) == 0;
        bool isRightBorderVertex = ((vertexIndex + 1) % imgTerrain->getWidth()) == 0;
        bool isFirstRowVertex = rowNum == 0;
        bool isLastRowVertex = rowNum == (imgTerrain->getHeight() - 1);

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
            int firstLeftBottomTriangle = (squareIndex * 2) + (imgTerrain->getWidth() - 1) * 2;

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

    std::vector<unsigned int> Terrain::buildIndices(const Image *imgTerrain) const
    {
        std::vector<unsigned int> indices;
        indices.reserve(((imgTerrain->getHeight()-1) * imgTerrain->getWidth() * 2) + (imgTerrain->getHeight()-1));
        for(unsigned int z=0; z<imgTerrain->getHeight()-1; ++z)
        {
            for (unsigned int x = 0; x < imgTerrain->getWidth(); ++x)
            {
                indices.push_back(x + imgTerrain->getWidth() * (z + 1));
                indices.push_back(x + imgTerrain->getWidth() * z);
            }

            indices.push_back(RESTART_INDEX);
        }
        return indices;
    }

    void Terrain::display(const Matrix4<float> &viewMatrix) const
    {
        unsigned int shaderSaved = ShaderManager::instance()->getCurrentProgram();
        ShaderManager::instance()->bind(shader);

        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)viewMatrix);

        glBindVertexArray(vertexArrayObject);

        glEnable(GL_PRIMITIVE_RESTART); //TODO general to application ?
        glPrimitiveRestartIndex(RESTART_INDEX); //TODO general to application ?
        glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, nullptr);

        ShaderManager::instance()->bind(shaderSaved);
    }
}
