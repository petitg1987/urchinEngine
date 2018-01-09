#include <stdexcept>
#include <random>
#include <stack>

#include "TerrainGrass.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{

    TerrainGrass::TerrainGrass(const std::string &grassFilename) :
            grassPatchSize(ConfigService::instance()->getFloatValue("terrain.grassPatchSize")),
            grassQuadtreeDepth(ConfigService::instance()->getUnsignedIntValue("terrain.grassQuadtreeDepth")),
            sumTimeStep(0.0f),
            mainGrassQuadtree(nullptr)
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
        delete mainGrassQuadtree;
    }

    void TerrainGrass::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;

        ShaderManager::instance()->bind(terrainGrassShader);
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
    }

    void TerrainGrass::initialize(const std::unique_ptr<TerrainMesh> &mesh, const Point3<float> &position)
    {
        generateGrass(mesh, position);

        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        isInitialized = true;
    }

    void TerrainGrass::generateGrass(const std::unique_ptr<TerrainMesh> &mesh, const Point3<float> &terrainPosition)
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(-0.2f, 0.2f); //TODO configurable

        float grassOffset = 0.6f; //TODO configurable
        unsigned int grassXQuantity = mesh->getXZScale() * mesh->getXSize() / grassOffset;
        unsigned int grassZQuantity = mesh->getXZScale() * mesh->getZSize() / grassOffset;

        unsigned int patchQuantityX = mesh->getXZScale() * mesh->getXSize() / grassPatchSize;
        unsigned int patchQuantityZ = mesh->getXZScale() * mesh->getZSize() / grassPatchSize;
        float adjustedPatchSizeX = mesh->getXZScale() * mesh->getXSize() / patchQuantityX;
        float adjustedPatchSizeZ = mesh->getXZScale() * mesh->getZSize() / patchQuantityZ;

        std::vector<TerrainGrassQuadtree *> leafGrassPatches;
        leafGrassPatches.reserve(patchQuantityX * patchQuantityZ);
        for(unsigned int i=0; i<patchQuantityX * patchQuantityZ; ++i)
        {
            leafGrassPatches.push_back(new TerrainGrassQuadtree());
        }

        float startX = mesh->getVertices()[0].X;
        float startZ = mesh->getVertices()[0].Z;
        for(unsigned int xIndex=0; xIndex < grassXQuantity; ++xIndex)
        {
            const float xFixedValue = startX + xIndex * grassOffset;

            for(unsigned int zIndex=0; zIndex < grassZQuantity; ++zIndex)
            {
                float xValue = xFixedValue + distribution(generator);
                float zValue = (startZ + zIndex * grassOffset) + distribution(generator);
                float yValue = retrieveGlobalVertex(Point2<float>(xValue, zValue), mesh, terrainPosition).Y;

                unsigned int patchXIndex = std::min(static_cast<unsigned int>((xValue - startX) / adjustedPatchSizeX), patchQuantityX);
                unsigned int patchZIndex = std::min(static_cast<unsigned int>((zValue - startZ) / adjustedPatchSizeZ), patchQuantityZ);
                unsigned int patchIndex = (patchZIndex * patchQuantityX) + patchXIndex;

                leafGrassPatches[patchIndex]->addVertex(Point3<float>(xValue, yValue, zValue));
            }
        }

        buildGrassQuadtree(leafGrassPatches, patchQuantityX, patchQuantityZ);
    }

    Point3<float> TerrainGrass::retrieveGlobalVertex(const Point2<float> &localXzCoordinate, const std::unique_ptr<TerrainMesh> &mesh, const Point3<float> &terrainPosition) const
    {
        Point3<float> localCoordinate = Point3<float>(localXzCoordinate.X, 0.0f, localXzCoordinate.Y);
        Point3<float> farLeftCoordinate = localCoordinate - mesh->getVertices()[0];

        float xInterval = mesh->getVertices()[1].X - mesh->getVertices()[0].X;
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(mesh->getXSize() - 1));

        float zInterval = mesh->getVertices()[mesh->getXSize()].Z - mesh->getVertices()[0].Z;
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Z / zInterval)), 0, static_cast<int>(mesh->getZSize() - 1));

        return mesh->getVertices()[xIndex + zIndex*mesh->getXSize()] + terrainPosition;
    }

    void TerrainGrass::buildGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &leafGrassPatches, unsigned int leafQuantityX, unsigned int leafQuantityZ)
    {
        std::vector<TerrainGrassQuadtree *> childrenGrassQuadtree = leafGrassPatches;
        unsigned int childrenNbQuadtreeX = leafQuantityX;
        unsigned int childrenNbQuadtreeZ = leafQuantityZ;

        unsigned int depth = grassQuadtreeDepth;
        while(depth >= 1)
        {
            auto depthNbQuadtreeX = static_cast<unsigned int>(MathAlgorithm::pow(2, depth));
            unsigned int depthNbQuadtreeZ = depthNbQuadtreeX;
            unsigned int depthNbQuadtree = depthNbQuadtreeX * depthNbQuadtreeZ;
            if(std::sqrt(childrenGrassQuadtree.size()) >= std::sqrt(depthNbQuadtree)*2)
            {
                std::vector<TerrainGrassQuadtree *> depthGrassQuadtree;
                depthGrassQuadtree.reserve(depthNbQuadtree);
                for(unsigned int i=0; i<depthNbQuadtree; ++i)
                {
                    depthGrassQuadtree.push_back(new TerrainGrassQuadtree());
                }

                for (unsigned int childZ = 0; childZ < childrenNbQuadtreeZ; ++childZ)
                {
                    for (unsigned int childX = 0; childX < childrenNbQuadtreeX; ++childX)
                    {
                        int xQuadtreeIndex = (depthNbQuadtreeX/static_cast<float>(childrenNbQuadtreeX)) * (childX + 0.5f);
                        int zQuadtreeIndex = (depthNbQuadtreeZ/static_cast<float>(childrenNbQuadtreeZ)) * (childZ + 0.5f);

                        unsigned int quadtreeIndex = (zQuadtreeIndex * depthNbQuadtreeX) + xQuadtreeIndex;
                        unsigned int childQuadtreeIndex = (childZ * childrenNbQuadtreeZ) + childX;

                        depthGrassQuadtree[quadtreeIndex]->addChild(childrenGrassQuadtree[childQuadtreeIndex]);
                    }
                }

                childrenGrassQuadtree = depthGrassQuadtree;
                childrenNbQuadtreeX = depthNbQuadtreeX;
                childrenNbQuadtreeZ = depthNbQuadtreeZ;
            }

            depth--;
        }

        delete mainGrassQuadtree;
        mainGrassQuadtree = new TerrainGrassQuadtree(childrenGrassQuadtree);
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

        std::stack<const TerrainGrassQuadtree *> grassQuadtrees;
        grassQuadtrees.push(mainGrassQuadtree);
        while(!grassQuadtrees.empty())
        {
            const TerrainGrassQuadtree *grassQuadtree = grassQuadtrees.top();
            grassQuadtrees.pop();

            if(camera->getFrustum().collideWithAABBox(*grassQuadtree->getBox())) //TODO don't use whole frustum to cull
            {
                if(grassQuadtree->isLeaf())
                {
                    glBufferData(GL_ARRAY_BUFFER, grassQuadtree->getGrassVertices().size() * sizeof(float) * 3, &grassQuadtree->getGrassVertices()[0], GL_DYNAMIC_DRAW);
                    glDrawArrays(GL_POINTS, 0, grassQuadtree->getGrassVertices().size());
                }else
                {
                    for(const auto *child : grassQuadtree->getChildren())
                    {
                        grassQuadtrees.push(child);
                    }
                }
            }
        }

        glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable(GL_CULL_FACE);
    }

}
