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

        normalTexture = Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({0, 255, 0})).createTexture(false);
        dudvMap = Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({0, 255, 0})).createTexture(false);

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

    void Water::generateVertex() {
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
        waterRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->enableDepthTest()
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(normalTexture, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY)))
                ->addTexture(TextureReader::build(dudvMap, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY)))
                ->build();

        Point2<float> leftFarPoint(Point2<float>(-xSize / 2.0f + centerPosition.X, -zSize / 2.0f + centerPosition.Z));
        Point2<float> rightNearPoint(Point2<float>(xSize / 2.0f + centerPosition.X, zSize / 2.0f + centerPosition.Z));
        waterRectangle = std::make_unique<Rectangle<float>>(leftFarPoint, rightNearPoint);
    }

    void Water::updateWaterTextures() {
        assert(normalTexture && dudvMap);

        waterRenderer->updateTexture(0, TextureReader::build(normalTexture, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY)));
        waterRenderer->updateTexture(1, TextureReader::build(dudvMap, TextureParam::build(TextureParam::REPEAT, TextureParam::LINEAR, TextureParam::ANISOTROPY)));
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
        this->normalFilename = normalFilename;

        if (normalFilename.empty()) {
            normalTexture = Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({0, 255, 0})).createTexture(false);
        } else {
            auto *normalImage = MediaManager::instance()->getMedia<Image>(normalFilename);
            if (normalImage->getImageFormat() != Image::IMAGE_RGB) {
                normalImage->release();
                throw std::runtime_error("Water normal texture must have 3 components (RGB). Components: " + std::to_string(normalImage->retrieveComponentsCount()));
            }
            normalTexture = normalImage->createTexture(true);
            normalImage->release();
        }

        updateWaterTextures();
    }

    const std::string &Water::getNormalFilename() const {
        return normalFilename;
    }

    void Water::setDudvMap(const std::string &dudvFilename) {
        this->dudvFilename = dudvFilename;

        if (dudvFilename.empty()) {
            dudvMap = Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({255, 0, 255})).createTexture(false);
        } else {
            auto *dudvImage = MediaManager::instance()->getMedia<Image>(dudvFilename);
            if (dudvImage->getImageFormat() != Image::IMAGE_RGB) {
                dudvImage->release();
                throw std::runtime_error("Water dudv map must have 3 components (RGB). Components: " + std::to_string(dudvImage->retrieveComponentsCount()));
            }
            dudvMap = dudvImage->createTexture(true);
            dudvImage->release();
        }

        updateWaterTextures();
    }

    const std::string &Water::getDudvFilename() const {
        return dudvFilename;
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

    void Water::display(const TargetRenderer *renderTarget, const Camera *camera, FogManager *fogManager, float dt) {
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
            renderTarget->draw(waterRenderer);
        }
    }
}
