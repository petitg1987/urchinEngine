#include <scene/renderer3d/landscape/water/Water.h>
#include <resources/ResourceRetriever.h>
#include <graphics/api/vulkan/render/GenericRendererBuilder.h>
#include <graphics/shader/ShaderBuilder.h>

namespace urchin {

    Water::Water() :
            isInitialized(false),
            renderTarget(nullptr),
            positioningData({}),
            waterProperties({}),
            xSize(0.0f),
            zSize(0.0f),
            sRepeat(0.0f),
            tRepeat(0.0f),
            density(0.0f),
            gradient(0.0f) {
        waterShader = ShaderBuilder::createShader("water.vert.spv", "", "water.frag.spv");

        //general properties
        setCenterPosition(Point3<float>(0.0f, 0.0f, 0.0f));
        setXSize(1000.0f);
        setZSize(1000.0f);

        //surface properties
        setWaterColor(Vector3<float>(0.08f, 0.22f, 0.29f));
        setNormalTexture("");
        setDudvMap("");
        setWaveSpeed(0.08f);
        setWaveStrength(0.04f);
        setSRepeat(1.0f);
        setTRepeat(1.0f);

        //under water properties
        setDensity(2.0f);
        setGradient(0.5f);
    }

    void Water::initialize(RenderTarget& renderTarget) {
        this->renderTarget = &renderTarget;

        updateRenderer();

        isInitialized = true;
    }

    void Water::updateRenderer() {
        if (renderTarget) {
            float minX = -xSize / 2.0f + centerPosition.X;
            float minZ = -zSize / 2.0f + centerPosition.Z;
            float maxX = xSize / 2.0f + centerPosition.X;
            float maxZ = zSize / 2.0f + centerPosition.Z;

            std::vector<Point3<float>> vertexCoord = {
                    Point3<float>(minX, centerPosition.Y, minZ), Point3<float>(maxX, centerPosition.Y, minZ), Point3<float>(maxX, centerPosition.Y, maxZ),
                    Point3<float>(minX, centerPosition.Y, minZ), Point3<float>(maxX, centerPosition.Y, maxZ), Point3<float>(minX, centerPosition.Y, maxZ)
            };
            std::vector<Point2<float>> textureCoord = {
                    Point2<float>(0.0f, 0.0f), Point2<float>(sRepeat, 0.0f), Point2<float>(sRepeat, tRepeat),
                    Point2<float>(0.0f, 0.0f), Point2<float>(sRepeat, tRepeat), Point2<float>(0.0f, tRepeat)
            };
            waterRenderer = GenericRendererBuilder::create("water", *renderTarget, *waterShader, ShapeType::TRIANGLE)
                    ->enableDepthTest()
                    ->enableDepthWrite()
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addUniformData(sizeof(positioningData), &positioningData) //binding 0
                    ->addUniformData(sizeof(waterProperties), &waterProperties) //binding 1
                    ->addUniformTextureReader(TextureReader::build(normalTexture, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY))) //binding 2
                    ->addUniformTextureReader(TextureReader::build(dudvMap, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY))) //binding 3
                    ->build();
        }

        Point2 leftFarPoint(-xSize / 2.0f + centerPosition.X, -zSize / 2.0f + centerPosition.Z);
        Point2 rightNearPoint(xSize / 2.0f + centerPosition.X, zSize / 2.0f + centerPosition.Z);
        waterRectangle = std::make_unique<Rectangle2D<float>>(leftFarPoint, rightNearPoint);
    }

    void Water::buildUnderwaterFog() {
        underwaterFog = std::make_unique<Fog>(density, gradient, waterProperties.color, centerPosition.Y);
    }

    void Water::setCenterPosition(const Point3<float>& centerPosition) {
        this->centerPosition = centerPosition;

        updateRenderer();
    }

    const Point3<float>& Water::getCenterPosition() const {
        return centerPosition;
    }

    void Water::setXSize(float xSize) {
        this->xSize = xSize;

        updateRenderer();
    }

    float Water::getXSize() const {
        return xSize;
    }

    void Water::setZSize(float zSize) {
        this->zSize = zSize;

        updateRenderer();
    }

    float Water::getZSize() const {
        return zSize;
    }

    void Water::setWaterColor(const Vector3<float>& waterColor) {
        waterProperties.color = waterColor;

        if (waterRenderer) {
            waterRenderer->updateUniformData(1, &waterProperties);
        }
    }

    const Vector3<float>& Water::getWaterColor() const {
        return waterProperties.color;
    }

    void Water::setNormalTexture(std::string normalFilename) {
        if (normalFilename.empty() || FileUtil::isAbsolutePath(normalFilename)) {
            this->normalFilename = std::move(normalFilename);
        } else {
            this->normalFilename = FileSystem::instance().getResourcesDirectory() + std::move(normalFilename);
        }

        if (this->normalFilename.empty()) {
            std::vector<unsigned char> waterNormalColor({0, 255, 0, 255});
            normalTexture = Texture::build("default water normal", 1, 1, TextureFormat::RGBA_8_INT, waterNormalColor.data());
        } else {
            normalTexture = ResourceRetriever::instance().getResource<Texture>(this->normalFilename, {{"mipMap", "1"}});
            if (normalTexture->getFormat() != TextureFormat::RGBA_8_INT) {
                throw std::runtime_error("Mask texture must have 4 components (RGBA). Texture format: " + std::to_string((int)normalTexture->getFormat()));
            }
        }

        updateRenderer();
    }

    const std::string& Water::getNormalFilename() const {
        return normalFilename;
    }

    void Water::setDudvMap(std::string dudvFilename) {
        if (dudvFilename.empty() || FileUtil::isAbsolutePath(dudvFilename)) {
            this->dudvFilename = std::move(dudvFilename);
        } else {
            this->dudvFilename = FileSystem::instance().getResourcesDirectory() + std::move(dudvFilename);
        }

        if (this->dudvFilename.empty()) {
            std::vector<unsigned char> waterDudvColor({255, 0, 255, 255});
            dudvMap = Texture::build("default water dudv", 1, 1, TextureFormat::RGBA_8_INT, waterDudvColor.data());
        } else {
            dudvMap = ResourceRetriever::instance().getResource<Texture>(this->dudvFilename, {{"mipMap", "1"}});
            if (dudvMap->getFormat() != TextureFormat::RGBA_8_INT) {
                throw std::runtime_error("Water dudv map must have 4 components (RGBA). Texture format: " + std::to_string((int)dudvMap->getFormat()));
            }
        }

        updateRenderer();
    }

    const std::string& Water::getDudvFilename() const {
        return dudvFilename;
    }

    void Water::setWaveSpeed(float waveSpeed) {
        waterProperties.waveSpeed = waveSpeed;

        if (waterRenderer) {
            waterRenderer->updateUniformData(1, &waterProperties);
        }
    }

    float Water::getWaveSpeed() const {
        return waterProperties.waveSpeed;
    }

    void Water::setWaveStrength(float waveStrength) {
        waterProperties.waveStrength = waveStrength;

        if (waterRenderer) {
            waterRenderer->updateUniformData(1, &waterProperties);
        }
    }

    float Water::getWaveStrength() const {
        return waterProperties.waveStrength;
    }

    void Water::setSRepeat(float sRepeat) {
        this->sRepeat = sRepeat;

        updateRenderer();
    }

    float Water::getSRepeat() const {
        return sRepeat;
    }

    void Water::setTRepeat(float tRepeat) {
        this->tRepeat  = tRepeat;

        updateRenderer();
    }

    float Water::getTRepeat() const {
        return tRepeat;
    }

    void Water::setDensity(float density) {
        this->density = density;

        buildUnderwaterFog();
    }

    float Water::getDensity() const {
        return density;
    }

    void Water::setGradient(float gradient) {
        this->gradient = gradient;

        buildUnderwaterFog();
    }

    float Water::getGradient() const {
        return gradient;
    }

    void Water::prepareRendering(unsigned int renderingOrder, const Camera& camera, FogContainer& fogContainer, float dt) {
        assert(isInitialized);
        if (camera.getPosition().Y < centerPosition.Y && waterRectangle->collideWithPoint(Point2<float>(camera.getPosition().X, camera.getPosition().Z))) {
            if (fogContainer.getActiveFog() != underwaterFog.get()) {
                fogContainer.pushFog(underwaterFog.get());
                notifyObservers(this, NotificationType::MOVE_UNDER_WATER);
            }
        } else {
            if (fogContainer.getActiveFog() == underwaterFog.get()) {
                fogContainer.popFog();
                notifyObservers(this, NotificationType::MOVE_ABOVE_WATER);
            }

            positioningData.projectionViewMatrix = camera.getProjectionViewMatrix();
            positioningData.sumTimeStep += dt;

            waterRenderer->updateUniformData(0, &positioningData);
            waterRenderer->enableRenderer(renderingOrder);
        }
    }
}
