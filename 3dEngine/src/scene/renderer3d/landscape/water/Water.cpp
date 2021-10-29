#include <scene/renderer3d/landscape/water/Water.h>
#include <resources/ResourceRetriever.h>
#include <api/render/GenericRendererBuilder.h>
#include <api/render/shader/builder/ShaderBuilder.h>

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

        normalTexture = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 255, 0, 255}), false).createTexture(false);
        dudvMap = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 255, 0, 255}), false).createTexture(false);

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

    void Water::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        this->projectionMatrix = projectionMatrix;

        waterRenderer->updateUniformData(2, &projectionMatrix);
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
                    ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 2
                    ->addUniformTextureReader(TextureReader::build(normalTexture, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY))) //binding 3
                    ->addUniformTextureReader(TextureReader::build(dudvMap, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY))) //binding 4
                    ->build();
        }

        Point2<float> leftFarPoint(Point2<float>(-xSize / 2.0f + centerPosition.X, -zSize / 2.0f + centerPosition.Z));
        Point2<float> rightNearPoint(Point2<float>(xSize / 2.0f + centerPosition.X, zSize / 2.0f + centerPosition.Z));
        waterRectangle = std::make_unique<Rectangle<float>>(leftFarPoint, rightNearPoint);
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

    void Water::setNormalTexture(const std::string& normalFilename) {
        this->normalFilename = normalFilename;

        if (normalFilename.empty()) {
            normalTexture = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 255, 0, 255}), false).createTexture(false);
        } else {
            auto normalImage = ResourceRetriever::instance().getResource<Image>(normalFilename);
            if (normalImage->getImageFormat() != Image::IMAGE_RGBA) {
                throw std::runtime_error("Water normal texture must have 4 components (RGBA). Components: " + std::to_string(normalImage->retrieveComponentsCount()));
            }
            normalTexture = normalImage->createTexture(true);
        }

        updateRenderer();
    }

    const std::string& Water::getNormalFilename() const {
        return normalFilename;
    }

    void Water::setDudvMap(const std::string& dudvFilename) {
        this->dudvFilename = dudvFilename;

        if (dudvFilename.empty()) {
            dudvMap = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({255, 0, 255, 255}), false).createTexture(false);
        } else {
            auto dudvImage = ResourceRetriever::instance().getResource<Image>(dudvFilename);
            if (dudvImage->getImageFormat() != Image::IMAGE_RGBA) {
                throw std::runtime_error("Water dudv map must have 4 components (RGBA). Components: " + std::to_string(dudvImage->retrieveComponentsCount()));
            }
            dudvMap = dudvImage->createTexture(true);
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

    void Water::prepareRendering(unsigned int& renderingOrder, const Camera& camera, FogContainer& fogContainer, float dt) {
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

            positioningData.viewMatrix = camera.getViewMatrix();
            positioningData.sumTimeStep += dt;

            waterRenderer->updateUniformData(0, &positioningData);
            waterRenderer->enableRenderer(renderingOrder);
        }
    }
}
