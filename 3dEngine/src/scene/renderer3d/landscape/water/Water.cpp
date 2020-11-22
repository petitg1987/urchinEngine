#include "Water.h"
#include "resources/MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

#define DEFAULT_CENTER_POSITION Point3<float>(0.0f, 0.0f, 0.0f)
#define DEFAULT_SIZE 1000.0
#define DEFAULT_WATER_COLOR Vector3<float>(0.08f, 0.22f, 0.29f)
#define DEFAULT_NORMAL_TEXTURE ""
#define DEFAULT_DUDV_MAP ""
#define DEFAULT_WAVE_SPEED 0.08
#define DEFAULT_WAVE_STRENGTH 0.04
#define DEFAULT_REPEAT 1.0
#define DEFAULT_DENSITY 2.0
#define DEFAULT_GRADIENT 0.5

namespace urchin {

    Water::Water() :
            sumTimeStep(0.0f),
            xSize(0.0f),
            zSize(0.0f),
            normalTexture(nullptr),
            dudvMap(nullptr),
            waveSpeed(0.0f),
            waveStrength(0.0f),
            sRepeat(0.0f),
            tRepeat(0.0f),
            density(0.0f),
            gradient(0.0f) {
        waterShader = ShaderBuilder().createShader("water.vert", "", "water.frag");

        mProjectionShaderVar = ShaderVar(waterShader, "mProjection");
        mViewShaderVar = ShaderVar(waterShader, "mView");
        sumTimeStepShaderVar = ShaderVar(waterShader, "sumTimeStep");

        waterColorShaderVar = ShaderVar(waterShader, "waterColor");
        waveSpeedShaderVar = ShaderVar(waterShader, "waveSpeed");
        waveStrengthShaderVar = ShaderVar(waterShader, "waveStrength");

        int normalTexUnit = 0;
        int dudvMapUnit = 1;
        ShaderDataSender()
            .sendData(ShaderVar(waterShader, "normalTex"), normalTexUnit)
            .sendData(ShaderVar(waterShader, "dudvMap"), dudvMapUnit);

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

    Water::~Water() {
        if(normalTexture) {
            normalTexture->release();
        }
        if(dudvMap) {
            dudvMap->release();
        }
    }

    void Water::generateVertex() {
        float minX = -xSize/2.0f + centerPosition.X;
        float minZ = -zSize/2.0f + centerPosition.Z;
        float maxX = xSize/2.0f + centerPosition.X;
        float maxZ = zSize/2.0f + centerPosition.Z;

        std::vector<Point3<float>> vertexCoord = {
                Point3<float>(minX, centerPosition.Y, minZ), Point3<float>(maxX, centerPosition.Y, minZ), Point3<float>(maxX, centerPosition.Y, maxZ),
                Point3<float>(minX, centerPosition.Y, minZ), Point3<float>(maxX, centerPosition.Y, maxZ), Point3<float>(minX, centerPosition.Y, maxZ)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(sRepeat, 0.0f), Point2<float>(sRepeat, tRepeat),
                Point2<float>(0.0f, 0.0f), Point2<float>(sRepeat, tRepeat), Point2<float>(0.0f, tRepeat)
        };
        waterRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->enableDepthTest()
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(Texture::build(0)) //normal texture
                ->addTexture(Texture::build(0)) //dudv map
                ->build();
        updateWaterTextures();

        Point2<float> leftFarPoint(Point2<float>(-xSize/2.0f + centerPosition.X, -zSize/2.0f + centerPosition.Z));
        Point2<float> rightNearPoint(Point2<float>(xSize/2.0f + centerPosition.X, zSize/2.0f + centerPosition.Z));
        waterRectangle = std::make_unique<Rectangle<float>>(leftFarPoint, rightNearPoint);
    }

    void Water::updateWaterTextures() {
        if(normalTexture) {
            TextureParam textureParam = TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR_MIPMAP, TextureParam::ANISOTROPY);
            waterRenderer->updateTexture(0, Texture::build(normalTexture->getTextureID(), Texture::DEFAULT, textureParam));
        }

        if(dudvMap) {
            TextureParam textureParam = TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR_MIPMAP, TextureParam::ANISOTROPY);
            waterRenderer->updateTexture(1, Texture::build(dudvMap->getTextureID(), Texture::DEFAULT, textureParam));
        }
    }

    void Water::buildUnderwaterFog() {
        underwaterFog = std::make_shared<Fog>(density, gradient, waterColor, centerPosition.Y);
    }

    void Water::setCenterPosition(const Point3<float> &centerPosition) {
        this->centerPosition = centerPosition;

        generateVertex();
    }

    const Point3<float> &Water::getCenterPosition() const {
        return centerPosition;
    }

    void Water::setXSize(float xSize) {
        this->xSize = xSize;

        generateVertex();
    }

    float Water::getXSize() const {
        return xSize;
    }

    void Water::setZSize(float zSize) {
        this->zSize = zSize;

        generateVertex();
    }

    float Water::getZSize() const {
        return zSize;
    }

    void Water::setWaterColor(const Vector3<float> &waterColor) {
        this->waterColor = waterColor;

        ShaderDataSender().sendData(waterColorShaderVar, waterColor);
    }

    const Vector3<float> &Water::getWaterColor() const {
        return waterColor;
    }

    void Water::setNormalTexture(const std::string &normalFilename) {
        if (normalTexture) {
            normalTexture->release();
        }

        if (normalFilename.empty()) {
            normalTexture = new Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({0, 255, 0}));
        } else {
            normalTexture = MediaManager::instance()->getMedia<Image>(normalFilename);
            if (normalTexture->getImageFormat() != Image::IMAGE_RGB) {
                normalTexture->release();
                throw std::runtime_error("Water normal texture must have 3 components (RGB). Components: " + std::to_string(normalTexture->retrieveComponentsCount()));
            }
        }

        normalTexture->toTexture(true);
        updateWaterTextures();
    }

    const Image *Water::getNormalTexture() const {
        return normalTexture;
    }

    void Water::setDudvMap(const std::string &dudvFilename) {
        if (dudvMap) {
            dudvMap->release();
        }

        if (dudvFilename.empty()) {
            dudvMap = new Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({255, 0, 255}));
        } else {
            dudvMap = MediaManager::instance()->getMedia<Image>(dudvFilename);
            if (dudvMap->getImageFormat() != Image::IMAGE_RGB) {
                dudvMap->release();
                throw std::runtime_error("Water dudv map must have 3 components (RGB). Components: " + std::to_string(dudvMap->retrieveComponentsCount()));
            }
        }

        dudvMap->toTexture(true);
        updateWaterTextures();
    }

    const Image *Water::getDudvMap() const {
        return dudvMap;
    }

    void Water::setWaveSpeed(float waveSpeed) {
        this->waveSpeed = waveSpeed;

        ShaderDataSender().sendData(waveSpeedShaderVar, waveSpeed);
    }

    float Water::getWaveSpeed() const {
        return waveSpeed;
    }

    void Water::setWaveStrength(float waveStrength) {
        this->waveStrength = waveStrength;

        ShaderDataSender().sendData(waveStrengthShaderVar, waveStrength);
    }

    float Water::getWaveStrength() const {
        return waveStrength;
    }

    void Water::setSRepeat(float sRepeat) {
        this->sRepeat = sRepeat;

        generateVertex();
    }

    float Water::getSRepeat() const {
        return sRepeat;
    }

    void Water::setTRepeat(float tRepeat) {
        this->tRepeat  = tRepeat;

        generateVertex();
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

    void Water::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix) {
        this->projectionMatrix = projectionMatrix;

        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);
    }

    void Water::display(const Camera *camera, FogManager *fogManager, float dt) {
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

            sumTimeStep += dt;
            ShaderDataSender()
                    .sendData(mViewShaderVar, camera->getViewMatrix())
                    .sendData(sumTimeStepShaderVar, sumTimeStep);

            waterShader->bind();
            waterRenderer->draw();
        }
    }
}
