#include "Water.h"
#include "resources/MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/render/shader/builder/ShaderBuilder.h"

#define DEFAULT_CENTER_POSITION Point3<float>(0.0f, 0.0f, 0.0f)
#define DEFAULT_SIZE 1000.0f
#define DEFAULT_WATER_COLOR Vector3<float>(0.08f, 0.22f, 0.29f)
#define DEFAULT_NORMAL_TEXTURE ""
#define DEFAULT_DUDV_MAP ""
#define DEFAULT_WAVE_SPEED 0.08f
#define DEFAULT_WAVE_STRENGTH 0.04f
#define DEFAULT_REPEAT 1.0f
#define DEFAULT_DENSITY 2.0f
#define DEFAULT_GRADIENT 0.5f

namespace urchin {

    Water::Water() :
            isInitialized(false),
            positioningData({}),
            waterProperties({}),
            xSize(0.0f),
            zSize(0.0f),
            sRepeat(0.0f),
            tRepeat(0.0f),
            density(0.0f),
            gradient(0.0f) {
        waterShader = ShaderBuilder::createShader("water.vert", "", "water.frag");

        normalTexture = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 255, 0, 255})).createTexture(false);
        dudvMap = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 255, 0, 255})).createTexture(false);

        //general properties
        setCenterPosition(DEFAULT_CENTER_POSITION);
        setXSize(DEFAULT_SIZE);
        setZSize(DEFAULT_SIZE);

        //surface properties
        setWaterColor(DEFAULT_WATER_COLOR);
        setNormalTexture(DEFAULT_NORMAL_TEXTURE);
        setDudvMap(DEFAULT_DUDV_MAP);
        setWaveSpeed(DEFAULT_WAVE_SPEED);
        setWaveStrength(DEFAULT_WAVE_STRENGTH);
        setSRepeat(DEFAULT_REPEAT);
        setTRepeat(DEFAULT_REPEAT);

        //under water properties
        setDensity(DEFAULT_DENSITY);
        setGradient(DEFAULT_GRADIENT);
    }

    void Water::initialize(std::shared_ptr<RenderTarget> renderTarget) {
        this->renderTarget = std::move(renderTarget);

        updateRenderer();

        isInitialized = true;
    }

    void Water::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        this->projectionMatrix = projectionMatrix;

        waterRenderer->updateShaderData(2, &projectionMatrix);
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
            waterRenderer = std::make_unique<GenericRendererBuilder>(renderTarget, waterShader, ShapeType::TRIANGLE)
                    ->enableDepthOperations()
                    ->addData(vertexCoord)
                    ->addData(textureCoord)
                    ->addShaderData(sizeof(positioningData), &positioningData) //binding 0
                    ->addShaderData(sizeof(waterProperties), &waterProperties) //binding 1
                    ->addShaderData(sizeof(projectionMatrix), &projectionMatrix) //binding 2
                    ->addTextureReader(TextureReader::build(normalTexture, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY)))
                    ->addTextureReader(TextureReader::build(dudvMap, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY)))
                    ->build();
        }

        Point2<float> leftFarPoint(Point2<float>(-xSize / 2.0f + centerPosition.X, -zSize / 2.0f + centerPosition.Z));
        Point2<float> rightNearPoint(Point2<float>(xSize / 2.0f + centerPosition.X, zSize / 2.0f + centerPosition.Z));
        waterRectangle = std::make_unique<Rectangle<float>>(leftFarPoint, rightNearPoint);
    }

    void Water::buildUnderwaterFog() {
        underwaterFog = std::make_shared<Fog>(density, gradient, waterProperties.color, centerPosition.Y);
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

        if(waterRenderer) {
            waterRenderer->updateShaderData(1, &waterProperties);
        }
    }

    const Vector3<float>& Water::getWaterColor() const {
        return waterProperties.color;
    }

    void Water::setNormalTexture(const std::string& normalFilename) {
        this->normalFilename = normalFilename;

        if (normalFilename.empty()) {
            normalTexture = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 255, 0, 255})).createTexture(false);
        } else {
            auto* normalImage = MediaManager::instance()->getMedia<Image>(normalFilename);
            if (normalImage->getImageFormat() != Image::IMAGE_RGBA) {
                normalImage->release();
                throw std::runtime_error("Water normal texture must have 4 components (RGBA). Components: " + std::to_string(normalImage->retrieveComponentsCount()));
            }
            normalTexture = normalImage->createTexture(true);
            normalImage->release();
        }

        updateRenderer();
    }

    const std::string& Water::getNormalFilename() const {
        return normalFilename;
    }

    void Water::setDudvMap(const std::string& dudvFilename) {
        this->dudvFilename = dudvFilename;

        if (dudvFilename.empty()) {
            dudvMap = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({255, 0, 255, 255})).createTexture(false);
        } else {
            auto* dudvImage = MediaManager::instance()->getMedia<Image>(dudvFilename);
            if (dudvImage->getImageFormat() != Image::IMAGE_RGBA) {
                dudvImage->release();
                throw std::runtime_error("Water dudv map must have 4 components (RGBA). Components: " + std::to_string(dudvImage->retrieveComponentsCount()));
            }
            dudvMap = dudvImage->createTexture(true);
            dudvImage->release();
        }

        updateRenderer();
    }

    const std::string& Water::getDudvFilename() const {
        return dudvFilename;
    }

    void Water::setWaveSpeed(float waveSpeed) {
        waterProperties.waveSpeed = waveSpeed;

        if(waterRenderer) {
            waterRenderer->updateShaderData(1, &waterProperties);
        }
    }

    float Water::getWaveSpeed() const {
        return waterProperties.waveSpeed;
    }

    void Water::setWaveStrength(float waveStrength) {
        waterProperties.waveStrength = waveStrength;

        if(waterRenderer) {
            waterRenderer->updateShaderData(1, &waterProperties);
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

    void Water::prepareRendering(const Camera* camera, FogManager* fogManager, float dt) {
        assert(isInitialized);
        if (camera->getPosition().Y < centerPosition.Y && waterRectangle->collideWithPoint(Point2<float>(camera->getPosition().X, camera->getPosition().Z))) {
            if (fogManager->getActiveFog() != underwaterFog) {
                fogManager->pushFog(underwaterFog);
                notifyObservers(this, NotificationType::MOVE_UNDER_WATER);
            }
        } else {
            if (fogManager->getActiveFog() == underwaterFog) {
                fogManager->popFog();
                notifyObservers(this, NotificationType::MOVE_ABOVE_WATER);
            }

            positioningData.viewMatrix = camera->getViewMatrix();
            positioningData.sumTimeStep += dt;

            waterRenderer->updateShaderData(0, &positioningData);

            //TODO renderTarget->display(waterRenderer);
        }
    }
}
