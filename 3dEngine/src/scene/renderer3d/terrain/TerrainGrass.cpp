#include <stdexcept>
#include <random>
#include <chrono>

#include "TerrainGrass.h"
#include "resources/image/Image.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{

    TerrainGrass::TerrainGrass(const std::string &grassFilename) :
            sumTimeStep(0.0f)
    {
        glGenBuffers(1, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        terrainGrassShader = ShaderManager::instance()->createProgram("terrainGrass.vert", "terrainGrass.frag");
        ShaderManager::instance()->setGeometryShader(terrainGrassShader, "terrainGrass.geo");
        ShaderManager::instance()->bind(terrainGrassShader);

        mProjectionLoc = glGetUniformLocation(terrainGrassShader, "mProjection");
        mViewLoc = glGetUniformLocation(terrainGrassShader, "mView");
        sumTimeStepLoc = glGetUniformLocation(terrainGrassShader, "sumTimeStep");

        grassTexture = MediaManager::instance()->getMedia<Image>(grassFilename, nullptr);
        grassTexture->toTexture(true, true, false);
        int terrainGrassTexLoc = glGetUniformLocation(terrainGrassShader, "grassTex");
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(terrainGrassTexLoc, 0);
    }

    TerrainGrass::~TerrainGrass()
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(1, bufferIDs);

        ShaderManager::instance()->removeProgram(terrainGrassShader);

        grassTexture->release();
    }

    void TerrainGrass::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;

        ShaderManager::instance()->bind(terrainGrassShader);
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
    }

    void TerrainGrass::initialize(const std::unique_ptr<TerrainMesh> &mesh, const Point3<float> &position)
    {
        generateGrassVertices(mesh, position);

        glBindVertexArray(vertexArrayObject);

//        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
//        glBufferData(GL_ARRAY_BUFFER, grassCenterVertices.size()*sizeof(float)*3, &grassCenterVertices[0], GL_STATIC_DRAW); //TODO GL_?_DRAW
//        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
//        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        isInitialized = true;
    }

    void TerrainGrass::generateGrassVertices(const std::unique_ptr<TerrainMesh> &mesh, const Point3<float> &terrainPosition)
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(-0.2f, 0.2f); //TODO configurable ?

        float grassOffset = 0.4f; //TODO make it configurable
        unsigned int grassXQuantity = mesh->getXZScale() * mesh->getXSize() / grassOffset;
        unsigned int grassZQuantity = mesh->getXZScale() * mesh->getZSize() / grassOffset;
        grassCenterVertices.reserve(grassXQuantity * grassZQuantity);

        float startX = mesh->getVertices()[0].X;
        float startZ = mesh->getVertices()[0].Z;

        for(unsigned int xIndex=0; xIndex<grassXQuantity; ++xIndex)
        {
            const float xFixedValue = startX + xIndex * grassOffset;

            for(unsigned int zIndex=0; zIndex<grassZQuantity; ++zIndex)
            {
                float xValue = xFixedValue + distribution(generator);
                float zValue = (startZ + zIndex * grassOffset) + distribution(generator);
                float yValue = retrieveGlobalVertex(Point2<float>(xValue, zValue), mesh, terrainPosition).Y;

                grassCenterVertices.emplace_back(Point3<float>(xValue, yValue, zValue));
            }
        }
    }

    Point3<float> TerrainGrass::retrieveGlobalVertex(const Point2<float> &localXzCoordinate, const std::unique_ptr<TerrainMesh> &mesh, const Point3<float> &terrainPosition) const
    { //TODO avoid duplicate with PolytopeTerrainSurface::retrieveGlobalVertex
        Point3<float> localCoordinate = Point3<float>(localXzCoordinate.X, 0.0f, localXzCoordinate.Y);
        Point3<float> farLeftCoordinate = localCoordinate - mesh->getVertices()[0];

        float xInterval = mesh->getVertices()[1].X - mesh->getVertices()[0].X;
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(mesh->getXSize() - 1));

        float zInterval = mesh->getVertices()[mesh->getXSize()].Z - mesh->getVertices()[0].Z;
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Z / zInterval)), 0, static_cast<int>(mesh->getZSize() - 1));

        return mesh->getVertices()[xIndex + zIndex*mesh->getXSize()] + terrainPosition;
    }

    void TerrainGrass::display(const Camera *camera, float invFrameRate)
    {
        if(!isInitialized)
        {
            throw std::runtime_error("Terrain grass must be initialized before load textures.");
        }

        ShaderManager::instance()->bind(terrainGrassShader);

        glDisable(GL_CULL_FACE);
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture->getTextureID());

        sumTimeStep += invFrameRate;
        glUniform1f(sumTimeStepLoc, sumTimeStep);
        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)camera->getViewMatrix());

        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, grassCenterVertices.size() * sizeof(float) * 3, &grassCenterVertices[0], GL_STATIC_DRAW); //TODO GL_?_DRAW
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(GL_POINTS, 0, grassCenterVertices.size());

        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable(GL_CULL_FACE);
    }

}
