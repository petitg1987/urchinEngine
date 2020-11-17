#include <stdexcept>
#include <random>
#include <stack>
#include <cassert>
#include <thread>
#include <functional>

#include "TerrainGrass.h"
#include "resources/MediaManager.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

#define DEFAULT_NUM_GRASS_IN_TEX 1
#define DEFAULT_GRASS_DISPLAY_DISTANCE 100.0
#define DEFAULT_GRASS_HEIGHT 1.0
#define DEFAULT_GRASS_LENGTH 1.0
#define DEFAULT_GRASS_QUANTITY 0.1
#define DEFAULT_WIND_DIRECTION Vector3<float>(1.0f, 0.0f, 0.0f)
#define DEFAULT_WIND_STRENGTH 1.0

namespace urchin {

    TerrainGrass::TerrainGrass(const std::string &grassTextureFilename) :
            grassPositionRandomPercentage(ConfigService::instance()->getFloatValue("terrain.grassPositionRandomPercentage")),
            grassPatchSize(ConfigService::instance()->getFloatValue("terrain.grassPatchSize")),
            grassQuadtreeDepth(ConfigService::instance()->getUnsignedIntValue("terrain.grassQuadtreeDepth")),
            sumTimeStep(0.0f),
            grassTexture(nullptr),
            grassMaskTexture(nullptr),
            mainGrassQuadtree(nullptr),
            numGrassInTex(0),
            grassDisplayDistance(0.0f),
            grassHeight(0.0f),
            grassLength(0.0f),
            grassQuantity(0.0f),
            windStrength(0.0f) {
        std::map<std::string, std::string> tokens;
        tokens["GRASS_ALPHA_TEST"] = ConfigService::instance()->getStringValue("terrain.grassAlphaTest");
        terrainGrassShader = ShaderBuilder().createShader("terrainGrass.vert", "terrainGrass.geom", "terrainGrass.frag", tokens);

        mProjectionShaderVar = ShaderVar(terrainGrassShader, "mProjection");
        mViewShaderVar = ShaderVar(terrainGrassShader, "mView");
        cameraPositionShaderVar = ShaderVar(terrainGrassShader, "cameraPosition");
        sumTimeStepShaderVar = ShaderVar(terrainGrassShader, "sumTimeStep");

        terrainMinPointShaderVar = ShaderVar(terrainGrassShader, "terrainMinPoint");
        terrainMaxPointShaderVar = ShaderVar(terrainGrassShader, "terrainMaxPoint");
        terrainAmbientShaderVar = ShaderVar(terrainGrassShader, "ambient");

        grassDisplayDistanceShaderVar = ShaderVar(terrainGrassShader, "grassDisplayDistance");
        grassHeightShaderVar = ShaderVar(terrainGrassShader, "grassHeight");
        grassHalfLengthShaderVar = ShaderVar(terrainGrassShader, "grassHalfLength");
        numGrassInTexShaderVar = ShaderVar(terrainGrassShader, "numGrassInTex");

        windDirectionShaderVar = ShaderVar(terrainGrassShader, "windDirection");
        windStrengthShaderVar = ShaderVar(terrainGrassShader, "windStrength");

        int grassTexUnit = 0;
        int grassMaskTexUnit = 1;
        ShaderDataSender()
            .sendData(ShaderVar(terrainGrassShader, "grassTex"), grassTexUnit)
            .sendData(ShaderVar(terrainGrassShader, "grassMaskTex"), grassMaskTexUnit);

        setGrassTexture(grassTextureFilename);
        setMaskTexture("");
        setNumGrassInTexture(DEFAULT_NUM_GRASS_IN_TEX);
        setGrassDisplayDistance(DEFAULT_GRASS_DISPLAY_DISTANCE);
        setGrassHeight(DEFAULT_GRASS_HEIGHT);
        setGrassLength(DEFAULT_GRASS_LENGTH);
        setGrassQuantity(DEFAULT_GRASS_QUANTITY);
        setWindDirection(DEFAULT_WIND_DIRECTION);
        setWindStrength(DEFAULT_WIND_STRENGTH);
    }

    TerrainGrass::~TerrainGrass() {
        if (grassTexture) {
            grassTexture->release();
        }
        if (grassMaskTexture) {
            grassMaskTexture->release();
        }
        delete mainGrassQuadtree;
    }

    void TerrainGrass::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix) {
        this->projectionMatrix = projectionMatrix;

        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);
    }

    void TerrainGrass::refreshWith(const std::shared_ptr<TerrainMesh> &mesh, const Point3<float> &terrainPosition) {
        generateGrass(mesh, terrainPosition);

        ShaderDataSender()
            .sendData(terrainMinPointShaderVar, mesh->getVertices()[0])
            .sendData(terrainMaxPointShaderVar, mesh->getVertices()[mesh->getXSize()*mesh->getZSize()-1]);
    }

    void TerrainGrass::refreshWith(float ambient) {
        ShaderDataSender().sendData(terrainAmbientShaderVar, ambient);
    }

    void TerrainGrass::generateGrass(const std::shared_ptr<TerrainMesh> &mesh, const Point3<float> &terrainPosition) {
        const unsigned int NUM_THREADS = std::max(2u, std::thread::hardware_concurrency());

        if (mesh) {
            this->mesh = mesh;
            this->terrainPosition = terrainPosition;

            unsigned int seed = 0; //no need to generate different random numbers at each start
            std::default_random_engine generator(seed);
            std::uniform_real_distribution<float> distribution(-grassPositionRandomPercentage / grassQuantity, grassPositionRandomPercentage/grassQuantity);

            auto grassXQuantity = std::lround(mesh->getXZScale() * (float)mesh->getXSize() * grassQuantity);
            auto grassZQuantity = std::lround(mesh->getXZScale() * (float)mesh->getZSize() * grassQuantity);

            auto patchQuantityX = std::lround(mesh->getXZScale() * (float)mesh->getXSize() / grassPatchSize);
            auto patchQuantityZ = std::lround(mesh->getXZScale() * (float)mesh->getZSize() / grassPatchSize);
            float adjustedPatchSizeX = mesh->getXZScale() * (float)mesh->getXSize() / patchQuantityX;
            float adjustedPatchSizeZ = mesh->getXZScale() * (float)mesh->getZSize() / patchQuantityZ;

            std::vector<TerrainGrassQuadtree *> leafGrassPatches;
            leafGrassPatches.reserve(patchQuantityX * patchQuantityZ);
            for (unsigned int i = 0; i < patchQuantityX * patchQuantityZ; ++i) {
                leafGrassPatches.push_back(new TerrainGrassQuadtree());
            }

            float startX = mesh->getVertices()[0].X;
            float startZ = mesh->getVertices()[0].Z;

            std::vector<std::thread> threads(NUM_THREADS);
            for (unsigned int threadI=0; threadI<NUM_THREADS; threadI++) {
                unsigned int beginX = threadI * grassXQuantity / NUM_THREADS;
                unsigned int endX = (threadI + 1) == NUM_THREADS ? grassXQuantity : (threadI + 1) * grassXQuantity / NUM_THREADS;

                threads[threadI] = std::thread([&, beginX, endX]() {
                    for (unsigned int xIndex = beginX; xIndex < endX; ++xIndex) {
                        const float xFixedValue = startX + (float)xIndex / grassQuantity;

                        for (unsigned int zIndex = 0; zIndex < grassZQuantity; ++zIndex) {
                            float xValue = xFixedValue + distribution(generator);
                            float zValue = (startZ + (float)zIndex / grassQuantity) + distribution(generator);
                            unsigned int vertexIndex = retrieveVertexIndex(Point2<float>(xValue, zValue));
                            float yValue = (mesh->getVertices()[vertexIndex] + terrainPosition).Y;

                            Point3<float> globalGrassVertex(xValue + terrainPosition.X, yValue, zValue + terrainPosition.Z);
                            Vector3<float> grassNormal = (mesh->getNormals()[vertexIndex] / 2.0f) + Vector3<float>(0.5f, 0.5f, 0.5f);

                            unsigned int patchXIndex = std::min(static_cast<long>((xValue - startX) / adjustedPatchSizeX), patchQuantityX);
                            unsigned int patchZIndex = std::min(static_cast<long>((zValue - startZ) / adjustedPatchSizeZ), patchQuantityZ);
                            unsigned int patchIndex = (patchZIndex * patchQuantityX) + patchXIndex;

                            leafGrassPatches[patchIndex]->addVertex(globalGrassVertex, grassNormal);
                        }
                    }
                });
            }
            std::for_each(threads.begin(), threads.end(), [](std::thread& x){x.join();});

            buildGrassQuadtree(leafGrassPatches, patchQuantityX, patchQuantityZ);
            createRenderers(leafGrassPatches);
        }
    }

    unsigned int TerrainGrass::retrieveVertexIndex(const Point2<float> &localXzCoordinate) const {
        Point3<float> localCoordinate = Point3<float>(localXzCoordinate.X, 0.0f, localXzCoordinate.Y);
        Point3<float> farLeftCoordinate = localCoordinate - mesh->getVertices()[0];

        float xInterval = mesh->getVertices()[1].X - mesh->getVertices()[0].X;
        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.X / xInterval)), 0, static_cast<int>(mesh->getXSize() - 1));

        float zInterval = mesh->getVertices()[mesh->getXSize()].Z - mesh->getVertices()[0].Z;
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farLeftCoordinate.Z / zInterval)), 0, static_cast<int>(mesh->getZSize() - 1));

        return xIndex + zIndex*mesh->getXSize();
    }

    void TerrainGrass::buildGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &leafGrassPatches, unsigned int leafQuantityX, unsigned int leafQuantityZ) {
        std::vector<TerrainGrassQuadtree *> childrenGrassQuadtree = leafGrassPatches;
        unsigned int childrenNbQuadtreeX = leafQuantityX;
        unsigned int childrenNbQuadtreeZ = leafQuantityZ;

        unsigned int depth = grassQuadtreeDepth;
        while (depth >= 1) {
            auto depthNbQuadtreeX = static_cast<unsigned int>(MathAlgorithm::pow(2, depth));
            auto depthNbQuadtreeZ = depthNbQuadtreeX;
            unsigned int depthNbQuadtree = depthNbQuadtreeX * depthNbQuadtreeZ;
            if (std::sqrt(childrenGrassQuadtree.size()) >= std::sqrt(depthNbQuadtree)*2) {
                std::vector<TerrainGrassQuadtree *> depthGrassQuadtree;
                depthGrassQuadtree.reserve(depthNbQuadtree);
                for (unsigned int i=0; i<depthNbQuadtree; ++i) {
                    depthGrassQuadtree.push_back(new TerrainGrassQuadtree());
                }

                for (unsigned int childZ = 0; childZ < childrenNbQuadtreeZ; ++childZ) {
                    for (unsigned int childX = 0; childX < childrenNbQuadtreeX; ++childX) {
                        auto xQuadtreeIndex = static_cast<int>((depthNbQuadtreeX / (float)childrenNbQuadtreeX) * ((float)childX + 0.5f));
                        auto zQuadtreeIndex = static_cast<int>((depthNbQuadtreeZ / (float)childrenNbQuadtreeZ) * ((float)childZ + 0.5f));

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

    void TerrainGrass::createRenderers(const std::vector<TerrainGrassQuadtree *> &leafGrassPatches) {
        if(grassTexture) {
            TextureParam grassTextureParam = TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR_MIPMAP, TextureParam::ANISOTROPY);
            TextureParam grassMaskTextureParam = TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, TextureParam::NO_ANISOTROPY);

            for (auto *grassQuadtree : leafGrassPatches) {
                std::unique_ptr<GenericRenderer> renderer = std::make_unique<GenericRendererBuilder>(ShapeType::POINT)
                        ->enableDepthTest()
                        ->disableCullFace()
                        ->addData(&grassQuadtree->getGrassVertices())
                        ->addData(&grassQuadtree->getGrassNormals())
                        ->addTexture(Texture::build(grassTexture->getTextureID(), Texture::DEFAULT, grassTextureParam))
                        ->addTexture(Texture::build(grassMaskTexture->getTextureID(), Texture::DEFAULT, grassMaskTextureParam))
                        ->build();

                grassQuadtree->setRenderer(std::move(renderer));
            }
        }
    }

    const std::string &TerrainGrass::getGrassTexture() const {
        return grassTextureFilename;
    }

    void TerrainGrass::setGrassTexture(const std::string &grassTextureFilename) {
        this->grassTextureFilename = grassTextureFilename;

        if (grassTexture) {
            grassTexture->release();
        }

        if (grassTextureFilename.empty()) {
            grassTexture = nullptr;
        } else {
            grassTexture = MediaManager::instance()->getMedia<Image>(grassTextureFilename);
            grassTexture->toTexture(true);
        }
    }

    const std::string &TerrainGrass::getMaskTexture() const {
        return grassMaskFilename;
    }

    void TerrainGrass::setMaskTexture(const std::string &grassMaskFilename) {
        this->grassMaskFilename = grassMaskFilename;

        if (grassMaskTexture) {
            grassMaskTexture->release();
        }

        if (grassMaskFilename.empty()) {
            grassMaskTexture = new Image(1, 1, Image::IMAGE_GRAYSCALE, std::vector<unsigned char>({0}));
        } else {
            grassMaskTexture = MediaManager::instance()->getMedia<Image>(grassMaskFilename);
        }
        grassMaskTexture->toTexture(false);
    }

    float TerrainGrass::getGrassDisplayDistance() const {
        return grassDisplayDistance;
    }

    void TerrainGrass::setGrassDisplayDistance(float grassDisplayDistance) {
        assert(grassDisplayDistance != 0.0f);
        this->grassDisplayDistance = grassDisplayDistance;

        ShaderDataSender().sendData(grassDisplayDistanceShaderVar, grassDisplayDistance);
    }

    float TerrainGrass::getGrassHeight() const {
        return grassHeight;
    }

    void TerrainGrass::setGrassHeight(float grassHeight) {
        this->grassHeight = grassHeight;

        ShaderDataSender().sendData(grassHeightShaderVar, grassHeight);
    }

    float TerrainGrass::getGrassLength() const {
        return grassLength;
    }

    void TerrainGrass::setGrassLength(float grassLength) {
        this->grassLength = grassLength;

        ShaderDataSender().sendData(grassHalfLengthShaderVar, grassLength / 2.0f);
    }

    unsigned int TerrainGrass::getNumGrassInTexture() const {
        return numGrassInTex;
    }

    void TerrainGrass::setNumGrassInTexture(unsigned int numGrassInTex) {
        this->numGrassInTex = numGrassInTex;

        ShaderDataSender().sendData(numGrassInTexShaderVar, static_cast<int>(numGrassInTex));
    }

    float TerrainGrass::getGrassQuantity() const {
        return grassQuantity;
    }

    void TerrainGrass::setGrassQuantity(float grassQuantity) {
        this->grassQuantity = grassQuantity;

        generateGrass(mesh, terrainPosition);
    }

    Vector3<float> TerrainGrass::getWindDirection() const {
        return windDirection;
    }

    void TerrainGrass::setWindDirection(const Vector3<float> &windDirection) {
        this->windDirection = windDirection.normalize();

        ShaderDataSender().sendData(windDirectionShaderVar, this->windDirection);
    }

    float TerrainGrass::getWindStrength() const {
        return windStrength;
    }

    void TerrainGrass::setWindStrength(float windStrength) {
        this->windStrength = windStrength;

        ShaderDataSender().sendData(windStrengthShaderVar, windStrength);
    }

    void TerrainGrass::display(const Camera *camera, float dt) {
        if (grassTexture) {
            ScopeProfiler profiler("3d", "grassDisplay");

            sumTimeStep += dt;
            ShaderDataSender()
                .sendData(sumTimeStepShaderVar, sumTimeStep)
                .sendData(mViewShaderVar, camera->getViewMatrix())
                .sendData(cameraPositionShaderVar, camera->getPosition());

            grassQuadtrees.clear();
            grassQuadtrees.push_back(mainGrassQuadtree);

            terrainGrassShader->bind();
            for (std::size_t i=0; i<grassQuadtrees.size(); ++i) {
                const TerrainGrassQuadtree *grassQuadtree = grassQuadtrees[i];
                const std::unique_ptr<AABBox<float>> &grassQuadtreeBox = grassQuadtree->getBox();

                if (grassQuadtreeBox && camera->getFrustum().cutFrustum(grassDisplayDistance).collideWithAABBox(*grassQuadtreeBox)) {
                    if (grassQuadtree->isLeaf()) {
                        grassQuadtree->getRenderer()->draw();
                    } else {
                        for (const auto *child : grassQuadtree->getChildren()) {
                            grassQuadtrees.push_back(child);
                        }
                    }
                }
            }
        }
    }

}
