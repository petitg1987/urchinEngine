#include <random>
#include <stack>
#include <cassert>
#include <thread>
#include <functional>

#include <scene/renderer3d/landscape/terrain/grass/TerrainGrass.h>
#include <resources/ResourceRetriever.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    TerrainGrass::TerrainGrass(const std::string& grassTextureFilename) :
            grassParcelSize(ConfigService::instance().getFloatValue("terrain.grassParcelSize")),
            grassQuadtreeDepth(ConfigService::instance().getUnsignedIntValue("terrain.grassQuadtreeDepth")),
            bIsInitialized(false),
            renderTarget(nullptr),
            positioningData({}),
            grassProperties({}),
            ambient(0.5f),
            mesh(nullptr),
            grassTextureParam(TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, TextureParam::ANISOTROPY)),
            grassQuantity(0.0f) {
        float grassAlphaTest = ConfigService::instance().getFloatValue("terrain.grassAlphaTest");
        std::vector<std::size_t> variablesSize = {sizeof(grassAlphaTest)};
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &grassAlphaTest);
        terrainGrassShader = ShaderBuilder::createShader("terrainGrass.vert.spv", "terrainGrass.frag.spv", std::move(shaderConstants));

        setGrassTexture(grassTextureFilename);
        setMaskTexture("");
        setNumGrassInTexture(1);
        setGrassDisplayDistance(100.0f);
        setGrassHeight(1.0f);
        setGrassLength(1.0f);
        setGrassQuantity(0.1f);
        setWindDirection(Vector3<float>(1.0f, 0.0f, 0.0f));
        setWindStrength(1.0f);
    }

    void TerrainGrass::initialize(RenderTarget& renderTarget) {
        assert(!bIsInitialized);

        this->renderTarget = &renderTarget;

        bIsInitialized = true;
    }

    bool TerrainGrass::isInitialized() const {
        return bIsInitialized;
    }

    void TerrainGrass::refreshWith(const TerrainMesh* mesh, const Point3<float>& terrainPosition) {
        assert(bIsInitialized);
        assert(mesh);
        generateGrass(mesh, terrainPosition);
    }

    void TerrainGrass::refreshWith(float ambient) {
        assert(bIsInitialized);
        this->ambient = ambient;

        for (auto* renderer: getAllRenderers()) {
            renderer->updateUniformData(AMBIENT_UNIFORM_BINDING, &ambient);
        }
    }

    void TerrainGrass::generateGrass(const TerrainMesh* mesh, const Point3<float>& terrainPosition) {
        if (!mesh) {
            return;
        }

        this->mesh = mesh;
        this->terrainPosition = terrainPosition;

        const unsigned int NUM_THREADS = std::max(2u, std::thread::hardware_concurrency());
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::default_random_engine generator(seed);
        std::uniform_real_distribution<float> distribution(-GRASS_POSITION_RANDOM_PERCENTAGE / grassQuantity, GRASS_POSITION_RANDOM_PERCENTAGE / grassQuantity);

        float terrainSizeX = mesh->getXZScale() * (float)mesh->getXSize();
        float terrainSizeZ = mesh->getXZScale() * (float)mesh->getZSize();
        unsigned int parcelQuantityX = MathFunction::roundToUInt(terrainSizeX / grassParcelSize);
        unsigned int parcelQuantityZ = MathFunction::roundToUInt(terrainSizeZ / grassParcelSize);
        float parcelSizeX = terrainSizeX / (float)parcelQuantityX;
        float parcelSizeZ = terrainSizeZ / (float)parcelQuantityZ;
        unsigned int grassXQuantity = MathFunction::roundToUInt(terrainSizeX * grassQuantity);
        unsigned int grassZQuantity = MathFunction::roundToUInt(terrainSizeZ * grassQuantity);

        std::shared_ptr<Image> grassMaskImage;
        if (!grassMaskFilename.empty()) {
            grassMaskImage = ResourceRetriever::instance().getResource<Image>(this->grassMaskFilename);
        }

        std::vector<std::unique_ptr<TerrainGrassQuadtree>> leafGrassParcels;
        leafGrassParcels.reserve((std::size_t)parcelQuantityX * parcelQuantityZ);
        for (unsigned int i = 0; i < parcelQuantityX * parcelQuantityZ; ++i) {
            leafGrassParcels.push_back(std::make_unique<TerrainGrassQuadtree>());
        }

        std::vector<std::jthread> threads(NUM_THREADS);
        for (unsigned int threadI = 0; threadI < NUM_THREADS; threadI++) {
            unsigned int beginX = threadI * grassXQuantity / NUM_THREADS;
            unsigned int endX = (threadI + 1) == NUM_THREADS ? grassXQuantity : (threadI + 1) * grassXQuantity / NUM_THREADS;

            threads[threadI] = std::jthread([&, beginX, endX]() {
                for (unsigned int xIndex = beginX; xIndex < endX; ++xIndex) {
                    for (unsigned int zIndex = 0; zIndex < grassZQuantity; ++zIndex) {
                        float xValue = ((float)xIndex / grassQuantity) + distribution(generator);
                        float zValue = ((float)zIndex / grassQuantity) + distribution(generator);
                        if (xValue <= 0.0f || xValue >= terrainSizeX || zValue <= 0.0f || zValue >= terrainSizeZ) {
                            continue;
                        }

                        if (discardGrass(grassMaskImage.get(), xValue, zValue, terrainSizeX, terrainSizeZ)) {
                            continue;
                        }

                        //Use the same normal as terrain to have identical lighting. Convert normal range from (-1.0, 1.0) to (0.0, 1.0).
                        unsigned int vertexIndex = retrieveVertexIndex(Point2<float>(xValue, zValue));
                        Vector3<float> grassNormal = (mesh->getNormals()[vertexIndex] / 2.0f) + Vector3<float>(0.5f, 0.5f, 0.5f);

                        float globalXValue = terrainPosition.X + mesh->getVertices()[0].X + xValue;
                        float globalZValue = terrainPosition.Z + mesh->getVertices()[0].Z + zValue;
                        float globalYValue = terrainPosition.Y + mesh->getVertices()[vertexIndex].Y;
                        Point3 globalGrassPosition(globalXValue, globalYValue, globalZValue);

                        unsigned int parcelXIndex = std::min((unsigned int)(xValue / parcelSizeX), parcelQuantityX);
                        unsigned int parcelZIndex = std::min((unsigned int)(zValue / parcelSizeZ), parcelQuantityZ);
                        unsigned int parcelIndex = (parcelZIndex * parcelQuantityX) + parcelXIndex;

                        leafGrassParcels[parcelIndex]->addGrassInstanceData(globalGrassPosition, grassNormal);
                    }
                }
            });
        }
        std::ranges::for_each(threads, [](std::jthread& x){x.join();});

        createRenderers(leafGrassParcels);
        buildGrassQuadtree(std::move(leafGrassParcels), parcelQuantityX, parcelQuantityZ);
    }

    bool TerrainGrass::discardGrass(const Image* grassMaskImage, float xValue, float zValue, float terrainSizeX, float terrainSizeZ) const {
        if (grassMaskImage != nullptr) {
            unsigned int texelX = MathFunction::roundToUInt((xValue / terrainSizeX) * (float)grassMaskImage->getWidth());
            unsigned int texelY = MathFunction::roundToUInt((zValue / terrainSizeZ) * (float)grassMaskImage->getHeight());
            unsigned int texelIndex = (texelY * grassMaskImage->getWidth() + texelX) * grassMaskImage->getComponentCount();
            unsigned int adjustedTexelIndex = std::clamp(texelIndex, 0u, (unsigned int)grassMaskImage->getTexels().size());

            unsigned char xImg = grassMaskImage->getTexels()[adjustedTexelIndex];
            if (xImg > 127) {
                return true;
            }
        }
        return false;
    }

    unsigned int TerrainGrass::retrieveVertexIndex(const Point2<float>& localXzCoordinate) const {
        float xInterval = mesh->getVertices()[1].X - mesh->getVertices()[0].X;
        unsigned int xIndex = std::clamp(MathFunction::roundToUInt(localXzCoordinate.X / xInterval), 0u, mesh->getXSize() - 1);

        float zInterval = mesh->getVertices()[mesh->getXSize()].Z - mesh->getVertices()[0].Z;
        unsigned int zIndex = std::clamp(MathFunction::roundToUInt(localXzCoordinate.Y / zInterval), 0u, mesh->getZSize() - 1);

        return xIndex + zIndex * mesh->getXSize();
    }

    void TerrainGrass::buildGrassQuadtree(std::vector<std::unique_ptr<TerrainGrassQuadtree>> leafGrassParcels, unsigned int leafQuantityX, unsigned int leafQuantityZ) {
        std::vector<std::unique_ptr<TerrainGrassQuadtree>> childrenGrassQuadtree = std::move(leafGrassParcels);
        unsigned int childrenNbQuadtreeX = leafQuantityX;
        unsigned int childrenNbQuadtreeZ = leafQuantityZ;

        unsigned int depth = grassQuadtreeDepth;
        while (depth >= 1) {
            auto depthNbQuadtreeX = (unsigned int)(MathFunction::pow(2, depth));
            auto depthNbQuadtreeZ = depthNbQuadtreeX;
            unsigned int depthNbQuadtree = depthNbQuadtreeX * depthNbQuadtreeZ;
            if (std::sqrt(childrenGrassQuadtree.size()) >= std::sqrt(depthNbQuadtree) * 2) {
                std::vector<std::unique_ptr<TerrainGrassQuadtree>> depthGrassQuadtree;
                depthGrassQuadtree.reserve(depthNbQuadtree);
                for (unsigned int i = 0; i < depthNbQuadtree; ++i) {
                    depthGrassQuadtree.push_back(std::make_unique<TerrainGrassQuadtree>());
                }

                for (unsigned int childZ = 0; childZ < childrenNbQuadtreeZ; ++childZ) {
                    for (unsigned int childX = 0; childX < childrenNbQuadtreeX; ++childX) {
                        auto xQuadtreeIndex = (unsigned int)(((float)depthNbQuadtreeX / (float)childrenNbQuadtreeX) * ((float)childX + 0.5f));
                        auto zQuadtreeIndex = (unsigned int)(((float)depthNbQuadtreeZ / (float)childrenNbQuadtreeZ) * ((float)childZ + 0.5f));

                        unsigned int quadtreeIndex = (zQuadtreeIndex * depthNbQuadtreeX) + xQuadtreeIndex;
                        unsigned int childQuadtreeIndex = (childZ * childrenNbQuadtreeZ) + childX;

                        assert(childrenGrassQuadtree[childQuadtreeIndex]);
                        depthGrassQuadtree[quadtreeIndex]->addChild(std::move(childrenGrassQuadtree[childQuadtreeIndex]));
                    }
                }

                childrenGrassQuadtree = std::move(depthGrassQuadtree);
                childrenNbQuadtreeX = depthNbQuadtreeX;
                childrenNbQuadtreeZ = depthNbQuadtreeZ;
            }

            depth--;
        }

        mainGrassQuadtree = std::make_unique<TerrainGrassQuadtree>(std::move(childrenGrassQuadtree));
    }

    void TerrainGrass::createRenderers(const std::vector<std::unique_ptr<TerrainGrassQuadtree>>& leafGrassParcels) {
        if (grassTexture && renderTarget) {

//TODO extract in method
            float grassHalfLength = grassProperties.length / 2.0f;
            float degree45 = 45.0f * MathValue::PI_FLOAT / 180.0f;
            std::array<Vector3<float>, 3> directions = {
                Vector3<float>(1.0, 0.0, 0.0),
                Vector3<float>(std::cos(degree45), 0.0f, std::sin(degree45)),
                Vector3<float>(std::cos(-degree45), 0.0f, std::sin(-degree45))
            };

            unsigned int seed = 0; //no need to generate different random numbers at each start
            std::uniform_int_distribution<std::mt19937::result_type> randomInts(0, (unsigned long)grassProperties.numGrassInTex - 1);
            std::default_random_engine generator(seed);

            std::vector<Point3<float>> grassVertex;
            grassVertex.reserve(directions.size() * 6);
            std::vector<Point2<float>> grassUv;
            grassUv.reserve(directions.size() * 6);

            for (const Vector3<float>& direction : directions) { //TODO slitghly randomly rotate grass ?
                Point3<float> topLeft = Point3<float>(0.0f, grassProperties.height, 0.0f).translate(-direction * grassHalfLength);
                Point3<float> bottomLeft = Point3<float>(0.0f, 0.0f, 0.0f).translate(-direction * grassHalfLength);
                Point3<float> topRight = Point3<float>(0.0f, grassProperties.height, 0.0f).translate(direction * grassHalfLength);
                Point3<float> bottomRight = Point3<float>(0.0f, 0.0f, 0.0f).translate(direction * grassHalfLength);
                grassVertex.push_back(topLeft);
                grassVertex.push_back(topRight);
                grassVertex.push_back(bottomLeft);
                grassVertex.push_back(bottomLeft);
                grassVertex.push_back(topRight);
                grassVertex.push_back(bottomRight);

                float startTexX = (float)randomInts(generator) * (1.0f / (float)grassProperties.numGrassInTex);
                float endTexX = startTexX + (1.0f / (float)grassProperties.numGrassInTex);
                grassUv.emplace_back(startTexX, 0.0f);
                grassUv.emplace_back(endTexX, 0.0f);
                grassUv.emplace_back(startTexX, 1.0f);
                grassUv.emplace_back(startTexX, 1.0f);
                grassUv.emplace_back(endTexX, 0.0f);
                grassUv.emplace_back(endTexX, 1.0f);
            }

            for (auto& grassQuadtree : leafGrassParcels) {
                if (!grassQuadtree->getGrassInstanceData().empty()) {
                    auto renderer = GenericRendererBuilder::create("grass", *renderTarget, *terrainGrassShader, ShapeType::TRIANGLE)
                            ->enableDepthTest()
                            ->enableDepthWrite()
                            ->disableCullFace()
                            ->addData(grassVertex)
                            ->addData(grassUv)
                            ->instanceData(grassQuadtree->getGrassInstanceData().size(), {VariableType::VEC3_FLOAT, VariableType::VEC3_FLOAT}, (const float *)grassQuadtree->getGrassInstanceData().data())
                            ->addUniformData(POSITIONING_DATA_UNIFORM_BINDING, sizeof(positioningData), &positioningData)
                            ->addUniformData(GRASS_PROPS_UNIFORM_BINDING, sizeof(grassProperties), &grassProperties) //TODO remove unused data
                            ->addUniformData(AMBIENT_UNIFORM_BINDING, sizeof(ambient), &ambient)
                            ->addUniformTextureReader(GRASS_TEX_UNIFORM_BINDING, TextureReader::build(grassTexture, grassTextureParam))
                            ->build();

                    grassQuadtree->setRenderer(std::move(renderer));
                }
            }
        }
    }

    std::vector<GenericRenderer*> TerrainGrass::getAllRenderers() const {
        std::vector<GenericRenderer*> renderers;

        if (mainGrassQuadtree != nullptr) {
            grassQuadtrees.clear();
            grassQuadtrees.push_back(mainGrassQuadtree.get());
            for (std::size_t i = 0; i < grassQuadtrees.size(); ++i) {
                const TerrainGrassQuadtree* grassQuadtree = grassQuadtrees[i];
                if (grassQuadtree->isLeaf() && grassQuadtree->getRenderer()) {
                    renderers.emplace_back(grassQuadtree->getRenderer());
                } else {
                    for (const auto& child : grassQuadtree->getChildren()) {
                        grassQuadtrees.push_back(child.get());
                    }
                }
            }
        }

        return renderers;
    }

    const std::string& TerrainGrass::getGrassTexture() const {
        return grassTextureFilename;
    }

    void TerrainGrass::setGrassTexture(std::string grassTextureFilename) {
        if (grassTextureFilename.empty() || FileUtil::isAbsolutePath(grassTextureFilename)) {
            this->grassTextureFilename = std::move(grassTextureFilename);
        } else {
            this->grassTextureFilename = FileSystem::instance().getResourcesDirectory() + std::move(grassTextureFilename);
        }

        if (this->grassTextureFilename.empty()) {
            grassTexture = nullptr;
        } else {
            grassTexture = ResourceRetriever::instance().getResource<Texture>(this->grassTextureFilename, {{"mipMap", "1"}});

            for (auto* renderer: getAllRenderers()) {
                renderer->updateUniformTextureReader(GRASS_TEX_UNIFORM_BINDING, TextureReader::build(grassTexture, grassTextureParam));
            }
        }
    }

    const std::string& TerrainGrass::getMaskTexture() const {
        return grassMaskFilename;
    }

    void TerrainGrass::setMaskTexture(std::string grassMaskFilename) {
        if (grassMaskFilename.empty() || FileUtil::isAbsolutePath(grassMaskFilename)) {
            this->grassMaskFilename = std::move(grassMaskFilename);
        } else {
            this->grassMaskFilename = FileSystem::instance().getResourcesDirectory() + std::move(grassMaskFilename);
        }

        generateGrass(mesh, terrainPosition);
    }

    float TerrainGrass::getGrassDisplayDistance() const {
        return grassProperties.displayDistance;
    }

    void TerrainGrass::setGrassDisplayDistance(float grassDisplayDistance) {
        assert(grassDisplayDistance != 0.0f);
        grassProperties.displayDistance = grassDisplayDistance;

        for (auto* renderer: getAllRenderers()) {
            renderer->updateUniformData(GRASS_PROPS_UNIFORM_BINDING, &grassProperties);
        }
    }

    float TerrainGrass::getGrassHeight() const {
        return grassProperties.height;
    }

    void TerrainGrass::setGrassHeight(float grassHeight) {
        grassProperties.height = grassHeight;

        for (auto* renderer: getAllRenderers()) {
            renderer->updateUniformData(GRASS_PROPS_UNIFORM_BINDING, &grassProperties);
        }
    }

    float TerrainGrass::getGrassLength() const {
        return grassProperties.length;
    }

    void TerrainGrass::setGrassLength(float grassLength) {
        grassProperties.length = grassLength;

        for (auto* renderer: getAllRenderers()) {
            renderer->updateUniformData(GRASS_PROPS_UNIFORM_BINDING, &grassProperties);
        }
    }

    unsigned int TerrainGrass::getNumGrassInTexture() const {
        return (unsigned int)grassProperties.numGrassInTex;
    }

    void TerrainGrass::setNumGrassInTexture(unsigned int numGrassInTex) { //TODO refresh rendered (+check other props)
        grassProperties.numGrassInTex = (int)numGrassInTex;

        for (auto* renderer: getAllRenderers()) {
            renderer->updateUniformData(GRASS_PROPS_UNIFORM_BINDING, &grassProperties);
        }
    }

    float TerrainGrass::getGrassQuantity() const {
        return grassQuantity;
    }

    void TerrainGrass::setGrassQuantity(float grassQuantity) {
        this->grassQuantity = grassQuantity;

        generateGrass(mesh, terrainPosition);
    }

    Vector3<float> TerrainGrass::getWindDirection() const {
        return grassProperties.windDirection;
    }

    void TerrainGrass::setWindDirection(const Vector3<float>& windDirection) {
        grassProperties.windDirection = windDirection.normalize();

        for (auto* renderer: getAllRenderers()) {
            renderer->updateUniformData(GRASS_PROPS_UNIFORM_BINDING, &grassProperties);
        }
    }

    float TerrainGrass::getWindStrength() const {
        return grassProperties.windStrength;
    }

    void TerrainGrass::setWindStrength(float windStrength) {
        grassProperties.windStrength = windStrength;

        for (auto* renderer: getAllRenderers()) {
            renderer->updateUniformData(GRASS_PROPS_UNIFORM_BINDING, &grassProperties);
        }
    }

    void TerrainGrass::prepareRendering(unsigned int renderingOrder, const Camera& camera, float dt) {
        assert(bIsInitialized);

        if (grassTexture) {
            ScopeProfiler sp(Profiler::graphic(), "grassPreRender");

            positioningData.projectionViewMatrix = camera.getProjectionViewMatrix();
            positioningData.cameraPosition = camera.getPosition();
            positioningData.sumTimeStep += dt;

            grassQuadtrees.clear();
            grassQuadtrees.push_back(mainGrassQuadtree.get());

            for (std::size_t i = 0; i < grassQuadtrees.size(); ++i) {
                const TerrainGrassQuadtree* grassQuadtree = grassQuadtrees[i];
                const auto* grassQuadtreeBox = grassQuadtree->getBox();

                if (grassQuadtreeBox && camera.getFrustum().cutFrustum(grassProperties.displayDistance).collideWithAABBox(*grassQuadtreeBox)) {
                    if (grassQuadtree->isLeaf()) {
                        if (grassQuadtree->getRenderer()) {
                            grassQuadtree->getRenderer()->updateUniformData(POSITIONING_DATA_UNIFORM_BINDING, &positioningData);
                            grassQuadtree->getRenderer()->enableRenderer(renderingOrder);
                        }
                    } else {
                        for (const auto& child : grassQuadtree->getChildren()) {
                            grassQuadtrees.push_back(child.get());
                        }
                    }
                }
            }
        }
    }

}
