#include <GL/glew.h>

#include "Water.h"
#include "resources/MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/shader/ShaderManager.h"

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
        waterShader = ShaderManager::instance()->createProgram("water.vert", "", "water.frag");
        ShaderManager::instance()->bind(waterShader);

        mProjectionLoc = glGetUniformLocation(waterShader, "mProjection");
        mViewLoc = glGetUniformLocation(waterShader, "mView");
        sumTimeStepLoc = glGetUniformLocation(waterShader, "sumTimeStep");

        waterColorLoc = glGetUniformLocation(waterShader, "waterColor");
        waveSpeedLoc = glGetUniformLocation(waterShader, "waveSpeed");
        waveStrengthLoc = glGetUniformLocation(waterShader, "waveStrength");

        int normalTexLoc = glGetUniformLocation(waterShader, "normalTex");
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(normalTexLoc, 0);

        int dudvTexLoc = glGetUniformLocation(waterShader, "dudvMap");
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(dudvTexLoc, 1);

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

        ShaderManager::instance()->removeProgram(waterShader);
    }

    void Water::generateVertex() {
        std::vector<Point3<float>> vertexCoord = {
                Point3<float>(-xSize/2.0f, 0.0f, -zSize/2.0f) + centerPosition,
                Point3<float>(xSize/2.0f, 0.0f, -zSize/2.0f) + centerPosition,
                Point3<float>(xSize/2.0f, 0.0f, zSize/2.0f) + centerPosition,
                Point3<float>(-xSize/2.0f, 0.0f, zSize/2.0f) + centerPosition};

        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f*sRepeat, 0.0f*tRepeat),
                Point2<float>(1.0f*sRepeat, 0.0f*tRepeat),
                Point2<float>(1.0f*sRepeat, 1.0f*tRepeat),
                Point2<float>(0.0f*sRepeat, 1.0f*tRepeat)};

        waterRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::RECTANGLE)
                ->enableDepthTest()
                ->vertexData(CoordType::FLOAT, CoordDimension::_3D, &vertexCoord[0])
                ->textureData(CoordType::FLOAT, CoordDimension::_2D, &textureCoord[0])
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

        ShaderManager::instance()->bind(waterShader);
        glUniform3fv(waterColorLoc, 1, (const float*) waterColor);
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

        normalTexture->toTexture(true, true, true);
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

        dudvMap->toTexture(true, true, true);
        updateWaterTextures();
    }

    const Image *Water::getDudvMap() const {
        return dudvMap;
    }

    void Water::setWaveSpeed(float waveSpeed) {
        this->waveSpeed = waveSpeed;

        ShaderManager::instance()->bind(waterShader);
        glUniform1f(waveSpeedLoc, waveSpeed);
    }

    float Water::getWaveSpeed() const {
        return waveSpeed;
    }

    void Water::setWaveStrength(float waveStrength) {
        this->waveStrength = waveStrength;

        ShaderManager::instance()->bind(waterShader);
        glUniform1f(waveStrengthLoc, waveStrength);
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

        ShaderManager::instance()->bind(waterShader);
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
    }

    void Water::display(const Camera *camera, FogManager *fogManager, float dt) {
        if (camera->getPosition().Y < centerPosition.Y && waterRectangle->collideWithPoint(Point2<float>(camera->getPosition().X, camera->getPosition().Z))) {
            if (fogManager->getCurrentFog() != underwaterFog) {
                fogManager->pushFog(underwaterFog);
                notifyObservers(this, NotificationType::MOVE_UNDER_WATER);
            }
        } else {
            if (fogManager->getCurrentFog() == underwaterFog) {
                fogManager->popFog();
                notifyObservers(this, NotificationType::MOVE_ABOVE_WATER);
            }

            ShaderManager::instance()->bind(waterShader);
            glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float *) camera->getViewMatrix());

            sumTimeStep += dt;
            glUniform1f(sumTimeStepLoc, sumTimeStep);

            waterRenderer->draw();
        }
    }
}
