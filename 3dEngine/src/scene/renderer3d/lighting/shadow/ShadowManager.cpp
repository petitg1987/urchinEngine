#include <GL/glew.h>
#include <cmath>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <limits>
#include <map>
#include <string>

#include "ShadowManager.h"
#include "scene/renderer3d/lighting/shadow/filter/ModelProduceShadowFilter.h"
#include "texturefilter/TextureFilter.h"
#include "texturefilter/gaussianblur/GaussianBlurFilterBuilder.h"
#include "texturefilter/downsample/DownSampleFilterBuilder.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "resources/geometry/obbox/OBBoxModel.h"

#define DEFAULT_NUMBER_SHADOW_MAPS 5
#define DEFAULT_SHADOW_MAP_RESOLUTION 1024
#define DEFAULT_VIEWING_SHADOW_DISTANCE 75.0
#define DEFAULT_BLUR_SHADOW BlurShadow::MEDIUM

namespace urchin {

    ShadowManager::ShadowManager(LightManager *lightManager, OctreeManager<Model> *modelOctreeManager) :
            shadowMapBias(ConfigService::instance()->getFloatValue("shadow.shadowMapBias")),
            percentageUniformSplit(ConfigService::instance()->getFloatValue("shadow.frustumUniformSplitAgainstLogSplit")),
            lightViewOverflowStepSize(ConfigService::instance()->getFloatValue("shadow.lightViewOverflowStepSize")),
            shadowMapResolution(DEFAULT_SHADOW_MAP_RESOLUTION),
            nbShadowMaps(DEFAULT_NUMBER_SHADOW_MAPS),
            viewingShadowDistance(DEFAULT_VIEWING_SHADOW_DISTANCE),
            blurShadow(DEFAULT_BLUR_SHADOW),
            sceneWidth(0),
            sceneHeight(0),
            shadowModelDisplayer(nullptr),
            lightManager(lightManager),
            modelOctreeManager(modelOctreeManager),
            shadowShaderVariable(nullptr),
            shadowModelShaderVariable(nullptr),
            frustumDistance(0.0),
            bForceUpdateAllShadowMaps(false),
            lightsLocation(nullptr) {
        switch(ConfigService::instance()->getUnsignedIntValue("shadow.depthComponent")) {
            case 16:
                depthComponent = GL_DEPTH_COMPONENT16;
                break;
            case 24:
                depthComponent = GL_DEPTH_COMPONENT24;
                break;
            case 32:
                depthComponent = GL_DEPTH_COMPONENT32;
                break;
            default:
                throw std::domain_error("Unsupported value for parameter 'shadow.depthComponent'.");
        }

        if (lightViewOverflowStepSize==0.0f) { //because fmod function doesn't accept zero value.
            lightViewOverflowStepSize=std::numeric_limits<float>::epsilon();
        }

        lightManager->addObserver(this, LightManager::ADD_LIGHT);
        lightManager->addObserver(this, LightManager::REMOVE_LIGHT);

        createOrUpdateShadowModelDisplayer();
    }

    ShadowManager::~ShadowManager() {
        for (auto &shadowData : shadowDatas) {
            removeShadowMaps(shadowData.first);

            delete shadowData.second;
        }

        deleteLightsLocation();

        delete shadowModelDisplayer;
        delete shadowShaderVariable;
        delete shadowModelShaderVariable;
    }

    void ShadowManager::initiateShaderVariables(const std::unique_ptr<Shader> &lightingShader) {
        //shadow information
        depthSplitDistanceShaderVar = ShaderVar(lightingShader, "depthSplitDistance");

        //light information
        deleteLightsLocation();
        lightsLocation = new LightLocation[lightManager->getMaxLights()];
        std::ostringstream shadowMapTextureLocName, mLightProjectionViewLocName;
        for (unsigned int i=0;i<lightManager->getMaxLights();++i) {
            //depth shadow texture
            shadowMapTextureLocName.str("");
            shadowMapTextureLocName << "lightsInfo[" << i << "].shadowMapTex";
            lightsLocation[i].shadowMapTexShaderVar = ShaderVar(lightingShader, shadowMapTextureLocName.str());

            //light projection matrices
            lightsLocation[i].mLightProjectionViewShaderVar = new ShaderVar[nbShadowMaps];
            for (unsigned int j=0; j<nbShadowMaps; ++j) {
                mLightProjectionViewLocName.str("");
                mLightProjectionViewLocName << "lightsInfo[" << i << "].mLightProjectionView[" << j << "]";
                lightsLocation[i].mLightProjectionViewShaderVar[j] = ShaderVar(lightingShader, mLightProjectionViewLocName.str());
            }
        }
    }

    void ShadowManager::createOrUpdateShadowModelDisplayer() {
        std::map<std::string, std::string> geometryTokens, fragmentTokens;
        geometryTokens["MAX_VERTICES"] = std::to_string(3*nbShadowMaps);
        geometryTokens["NUMBER_SHADOW_MAPS"] = std::to_string(nbShadowMaps);
        delete shadowModelDisplayer;
        shadowModelDisplayer = new ModelDisplayer(ModelDisplayer::DEPTH_ONLY_MODE);
        shadowModelDisplayer->setCustomGeometryShader("modelShadowMap.geom", geometryTokens);
        shadowModelDisplayer->setCustomFragmentShader("modelShadowMap.frag", fragmentTokens);
        shadowModelDisplayer->initialize();

        delete shadowShaderVariable;
        shadowShaderVariable = new ShadowShaderVariable();
        shadowShaderVariable->setProjectionMatricesShaderVar(shadowModelDisplayer->getShaderVar("projectionMatrix"));
        shadowModelDisplayer->setCustomShaderVariable(shadowShaderVariable);

        delete shadowModelShaderVariable;
        shadowModelShaderVariable = new ShadowModelShaderVariable();
        shadowModelShaderVariable->setLayersToUpdateShaderVar(shadowModelDisplayer->getShaderVar("layersToUpdate"));
        shadowModelDisplayer->setCustomModelShaderVariable(shadowModelShaderVariable);
    }

    void ShadowManager::deleteLightsLocation() {
        if (lightsLocation) {
            for (unsigned int i=0;i<lightManager->getMaxLights();++i) {
                delete [] lightsLocation[i].mLightProjectionViewShaderVar;
            }
            delete [] lightsLocation;
        }
    }

    void ShadowManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        for (auto &shadowData : shadowDatas) {
            updateViewMatrix(shadowData.first);
        }
    }

    void ShadowManager::onCameraProjectionUpdate(const Camera *camera) {
        this->projectionMatrix = camera->getProjectionMatrix();
        this->frustumDistance = camera->getFrustum().computeFarDistance() + camera->getFrustum().computeNearDistance();

        splitFrustum(camera->getFrustum());
        updateShadowLights();
    }

    void ShadowManager::notify(Observable *observable, int notificationType) {
        if (dynamic_cast<LightManager *>(observable)) {
            Light *light = lightManager->getLastUpdatedLight();
            if (notificationType==LightManager::ADD_LIGHT) {
                light->addObserver(this, Light::PRODUCE_SHADOW);
                if (light->isProduceShadow()) {
                    addShadowLight(light);
                }
            } else if (notificationType==LightManager::REMOVE_LIGHT) {
                light->removeObserver(this, Light::PRODUCE_SHADOW);
                if (light->isProduceShadow()) {
                    removeShadowLight(light);
                }
            }
        } else if (auto *light = dynamic_cast<Light *>(observable)) {
            if (notificationType==Light::LIGHT_MOVE) {
                updateViewMatrix(light);
            } else if (notificationType==Light::PRODUCE_SHADOW) {
                if (light->isProduceShadow()) {
                    addShadowLight(light);
                } else {
                    removeShadowLight(light);
                }
            }
        }
    }

    float ShadowManager::getShadowMapBias() const {
        return shadowMapBias;
    }

    void ShadowManager::setShadowMapResolution(unsigned int shadowMapResolution) {
        this->shadowMapResolution = shadowMapResolution;

        updateShadowLights();
    }

    unsigned int ShadowManager::getShadowMapResolution() const {
        return shadowMapResolution;
    }

    void ShadowManager::setNumberShadowMaps(unsigned int nbShadowMaps) {
        if (nbShadowMaps <= 1) { //note: shadow maps texture array with depth=1 generate error in GLSL texture2DArray function
            throw std::runtime_error("Number of shadow maps must be greater than one. Value: " + std::to_string(nbShadowMaps));
        }

        this->nbShadowMaps = nbShadowMaps;

        createOrUpdateShadowModelDisplayer();
        updateShadowLights();
        notifyObservers(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);
    }

    unsigned int ShadowManager::getNumberShadowMaps() const {
        return nbShadowMaps;
    }

    /**
     * @param viewingShadowDistance Viewing shadow distance. If negative, shadow will be displayed until the far plane.
     */
    void ShadowManager::setViewingShadowDistance(float viewingShadowDistance) {
        this->viewingShadowDistance = viewingShadowDistance;
    }

    /**
     * @return Viewing shadow distance. If negative, shadow will be displayed until the far plane.
     */
    float ShadowManager::getViewingShadowDistance() const {
        return viewingShadowDistance;
    }

    void ShadowManager::setBlurShadow(ShadowManager::BlurShadow blurShadow) {
        this->blurShadow = blurShadow;
        updateShadowLights();
    }

    ShadowManager::BlurShadow ShadowManager::getBlurShadow() const {
        return blurShadow;
    }

    const std::vector<Frustum<float>> &ShadowManager::getSplitFrustums() const {
        return splitFrustums;
    }

    const ShadowData &ShadowManager::getShadowData(const Light *light) const {
        auto it = shadowDatas.find(light);
        if (it==shadowDatas.end()) {
            throw std::runtime_error("No shadow data found for this light.");
        }

        const ShadowData *shadowData = it->second;
        return *shadowData;
    }

    /**
     * @return All visible models from all lights
     */
    const std::vector<Model *> &ShadowManager::computeVisibleModels() {
        ScopeProfiler profiler("3d", "coVisibleModel");

        visibleModels.clear();
        for (const auto &shadowData : shadowDatas) {
            for (std::size_t i=0; i<nbShadowMaps; ++i) {
                const std::vector<Model *> &visibleModelsForLightInFrustumSplit = shadowData.second->getFrustumShadowData(i)->getModels();
                OctreeableHelper<Model>::merge(visibleModels, visibleModelsForLightInFrustumSplit);
            }
        }

        return visibleModels;
    }

    void ShadowManager::addShadowLight(const Light *light) {
        light->addObserver(this, Light::LIGHT_MOVE);

        shadowDatas[light] = new ShadowData(light, nbShadowMaps);

        createShadowMaps(light);
        updateViewMatrix(light);
    }

    void ShadowManager::removeShadowLight(const Light *light) {
        light->removeObserver(this, Light::LIGHT_MOVE);

        removeShadowMaps(light);

        delete shadowDatas[light];
        shadowDatas.erase(light);
    }

    /**
     * Updates lights data which producing shadows
     */
    void ShadowManager::updateShadowLights() {
        std::vector<const Light *> allLights;
        allLights.reserve(shadowDatas.size());
        for (const auto &shadowData : shadowDatas) {
            allLights.emplace_back(shadowData.first);
        }

        for (const auto &allLight : allLights) {
            removeShadowLight(allLight);
            addShadowLight(allLight);
        }
    }

    void ShadowManager::updateViewMatrix(const Light *light) {
        ShadowData *shadowData = shadowDatas[light];

        if (light->hasParallelBeams()) { //sun light
            Vector3<float> lightDirection = light->getDirections()[0];

            const Vector3<float> &f = lightDirection.normalize();
            const Vector3<float> &s = f.crossProduct(Vector3<float>(0.0, 1.0, 0.0)).normalize();
            const Vector3<float> &u = s.crossProduct(f).normalize();
            Matrix4<float> M(
                s[0],    s[1],    s[2],    0,
                u[0],    u[1],    u[2],    0,
                -f[0],    -f[1],    -f[2],    0,
                0,        0,        0,        1);

            Matrix4<float> eye;
            eye.buildTranslation(lightDirection.X, lightDirection.Y, lightDirection.Z);
            Matrix4<float> mViewShadow = M * eye;

            shadowData->setLightViewMatrix(mViewShadow);
        } else {
            throw std::runtime_error("Shadow currently not supported on omnidirectional light.");
        }
    }

    /**
     * Updates frustum shadow data (models, shadow caster/receiver box, projection matrix)
     */
    void ShadowManager::updateFrustumShadowData(const Light *light, ShadowData *shadowData) {
        ScopeProfiler profiler("3d", "upFrustumShadow");

        if (light->hasParallelBeams()) { //sun light
            Matrix4<float> lightViewMatrixInverse = shadowData->getLightViewMatrix().inverse();
            for (std::size_t i=0; i<splitFrustums.size(); ++i) {
                AABBox<float> aabboxSceneIndependent = createSceneIndependentBox(splitFrustums[i], shadowData->getLightViewMatrix());
                OBBox<float> obboxSceneIndependentViewSpace = lightViewMatrixInverse * OBBox<float>(aabboxSceneIndependent);

                obboxModels.clear();
                modelOctreeManager->getOctreeablesIn(obboxSceneIndependentViewSpace, obboxModels, ModelProduceShadowFilter());
                shadowData->getFrustumShadowData(i)->updateModels(obboxModels);

                AABBox<float> aabboxSceneDependent = createSceneDependentBox(aabboxSceneIndependent, obboxSceneIndependentViewSpace,
                                                                             obboxModels, shadowData->getLightViewMatrix());
                shadowData->getFrustumShadowData(i)->updateShadowCasterReceiverBox(aabboxSceneDependent, bForceUpdateAllShadowMaps);
            }
        } else {
            throw std::runtime_error("Shadow not supported on omnidirectional light.");
        }

        bForceUpdateAllShadowMaps = false;
    }

    /**
     * @return Box in light space containing shadow caster and receiver (scene independent)
     */
    AABBox<float> ShadowManager::createSceneIndependentBox(const Frustum<float> &splitFrustum, const Matrix4<float> &lightViewMatrix) const {
        ScopeProfiler profiler("3d", "sceneIndepBox");

        const Frustum<float> &frustumLightSpace = lightViewMatrix * splitFrustum;

        //determine point belonging to shadow caster/receiver box
        Point3<float> shadowReceiverAndCasterVertex[16];
        float nearCapZ = computeNearZForSceneIndependentBox(frustumLightSpace);
        for (unsigned int i=0; i<8; ++i) {
            const Point3<float> &frustumPoint = frustumLightSpace.getFrustumPoints()[i];

            //add shadow receiver points
            shadowReceiverAndCasterVertex[i*2] = frustumPoint;

            //add shadow caster points
            shadowReceiverAndCasterVertex[i*2 + 1] = Point3<float>(frustumPoint.X, frustumPoint.Y, nearCapZ);
        }

        //build shadow receiver/caster bounding box from points
        return AABBox<float>(shadowReceiverAndCasterVertex, 16);
    }

    float ShadowManager::computeNearZForSceneIndependentBox(const Frustum<float> &splitFrustumLightSpace) const {
        float nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[0].Z;
        for (unsigned int i=1; i<8; ++i) {
            if (splitFrustumLightSpace.getFrustumPoints()[i].Z > nearestPointFromLight) {
                nearestPointFromLight = splitFrustumLightSpace.getFrustumPoints()[i].Z;
            }
        }
        return nearestPointFromLight + frustumDistance;
    }

    /**
     * @return Box in light space containing shadow caster and receiver (scene dependent)
     */
    AABBox<float> ShadowManager::createSceneDependentBox(const AABBox<float> &aabboxSceneIndependent, const OBBox<float> &obboxSceneIndependentViewSpace,
            const std::vector<Model *> &models, const Matrix4<float> &lightViewMatrix) const {
        ScopeProfiler profiler("3d", "sceneDepBox");

        AABBox<float> aabboxSceneDependent;
        if (!models.empty()) {
            aabboxSceneDependent = AABBox<float>::initMergeableAABBox();

            for (const auto &model : models) {
                if (model->getSplitAABBoxes().size() == 1) {
                    aabboxSceneDependent = aabboxSceneDependent.merge(lightViewMatrix * model->getSplitAABBoxes()[0]);
                } else {
                    for (const auto &splitAABBox : model->getSplitAABBoxes()) {
                        if (obboxSceneIndependentViewSpace.collideWithAABBox(splitAABBox)) {
                            aabboxSceneDependent = aabboxSceneDependent.merge(lightViewMatrix * splitAABBox);
                        }
                    }
                }
            }
        }

        Point3<float> cutMin(
                std::min(std::max(aabboxSceneDependent.getMin().X, aabboxSceneIndependent.getMin().X), aabboxSceneIndependent.getMax().X),
                std::min(std::max(aabboxSceneDependent.getMin().Y, aabboxSceneIndependent.getMin().Y), aabboxSceneIndependent.getMax().Y),
                aabboxSceneIndependent.getMin().Z); //shadow can be projected outside the box: value cannot be capped

        Point3<float> cutMax(
                std::max(std::min(aabboxSceneDependent.getMax().X, aabboxSceneIndependent.getMax().X), aabboxSceneIndependent.getMin().X),
                std::max(std::min(aabboxSceneDependent.getMax().Y, aabboxSceneIndependent.getMax().Y), aabboxSceneIndependent.getMin().Y),
                std::max(std::min(aabboxSceneDependent.getMax().Z, aabboxSceneIndependent.getMax().Z), aabboxSceneIndependent.getMin().Z));

        cutMin.X = (cutMin.X < 0.0f) ? cutMin.X - (lightViewOverflowStepSize + fmod(cutMin.X, lightViewOverflowStepSize)) : cutMin.X - fmod(cutMin.X, lightViewOverflowStepSize);
        cutMin.Y = (cutMin.Y < 0.0f) ? cutMin.Y - (lightViewOverflowStepSize + fmod(cutMin.Y, lightViewOverflowStepSize)) : cutMin.Y - fmod(cutMin.Y, lightViewOverflowStepSize);
        cutMin.Z = (cutMin.Z < 0.0f) ? cutMin.Z - (lightViewOverflowStepSize + fmod(cutMin.Z, lightViewOverflowStepSize)) : cutMin.Z - fmod(cutMin.Z, lightViewOverflowStepSize);
        cutMax.X = (cutMax.X < 0.0f) ? cutMax.X - fmod(cutMax.X, lightViewOverflowStepSize) : cutMax.X + (lightViewOverflowStepSize - fmod(cutMax.X, lightViewOverflowStepSize));
        cutMax.Y = (cutMax.Y < 0.0f) ? cutMax.Y - fmod(cutMax.Y, lightViewOverflowStepSize) : cutMax.Y + (lightViewOverflowStepSize - fmod(cutMax.Y, lightViewOverflowStepSize));
        cutMax.Z = (cutMax.Z < 0.0f) ? cutMax.Z - fmod(cutMax.Z, lightViewOverflowStepSize) : cutMax.Z + (lightViewOverflowStepSize - fmod(cutMax.Z, lightViewOverflowStepSize));

        return AABBox<float>(cutMin, cutMax);
    }

    void ShadowManager::splitFrustum(const Frustum<float> &frustum) {
        ScopeProfiler profiler("3d", "splitFrustum");

        splitDistances.clear();
        splitFrustums.clear();

        float near = frustum.computeNearDistance();
        float far = viewingShadowDistance;
        if (viewingShadowDistance < 0.0f) {
            far = frustum.computeFarDistance();
        }

        float previousSplitDistance = near;
        for (unsigned int i=1; i<=nbShadowMaps; ++i) {
            float uniformSplit = near + (far - near) * (static_cast<float>(i)/static_cast<float>(nbShadowMaps));
            float logarithmicSplit = near * std::pow(far/near, static_cast<float>(i)/static_cast<float>(nbShadowMaps));

            float splitDistance = (percentageUniformSplit * uniformSplit) + ((1.0f - percentageUniformSplit) * logarithmicSplit);

            splitDistances.push_back(splitDistance);
            splitFrustums.push_back(frustum.splitFrustum(previousSplitDistance, splitDistance));

            previousSplitDistance = splitDistance;
        }
    }

    void ShadowManager::createShadowMaps(const Light *light) {
        //frame buffer object
        unsigned int fboID = 0;
        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);

        shadowDatas[light]->setFboID(fboID);

        //textures for shadow map: depth texture && shadow map texture (variance shadow map)
        GLenum fboAttachments[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, fboAttachments);
        glReadBuffer(GL_NONE);

        unsigned int textureIDs[2];
        glGenTextures(2, &textureIDs[0]);

        glBindTexture(GL_TEXTURE_2D_ARRAY, textureIDs[0]);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, depthComponent, shadowMapResolution, shadowMapResolution, nbShadowMaps, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureIDs[0], 0);

        glBindTexture(GL_TEXTURE_2D_ARRAY, textureIDs[1]);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, shadowMapResolution, shadowMapResolution, nbShadowMaps, 0, GL_RG, GL_FLOAT, nullptr);
        glFramebufferTexture(GL_FRAMEBUFFER, fboAttachments[0], textureIDs[1], 0);

        shadowDatas[light]->setDepthTextureID(textureIDs[0]);
        shadowDatas[light]->setShadowMapTextureID(textureIDs[1]);

        //add shadow map filter
        if (blurShadow!=BlurShadow::NO_BLUR) {
            std::unique_ptr<TextureFilter> verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>()
                    ->textureSize(shadowMapResolution, shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(nbShadowMaps)
                    ->textureInternalFormat(GL_RG32F)
                    ->textureFormat(GL_RG)
                    ->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize(static_cast<unsigned int>(blurShadow))
                    ->build();

            std::unique_ptr<TextureFilter> horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>()
                    ->textureSize(shadowMapResolution, shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(nbShadowMaps)
                    ->textureInternalFormat(GL_RG32F)
                    ->textureFormat(GL_RG)
                    ->blurDirection(GaussianBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurSize(static_cast<unsigned int>(blurShadow))
                    ->build();

            shadowDatas[light]->addTextureFilter(std::move(verticalBlurFilter));
            shadowDatas[light]->addTextureFilter(std::move(horizontalBlurFilter));
        } else { //null filter necessary because it allows to store cached shadow map in a texture which is not cleared.
            std::unique_ptr<TextureFilter> nullFilter = std::make_unique<DownSampleFilterBuilder>()
                    ->textureSize(shadowMapResolution, shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(nbShadowMaps)
                    ->textureInternalFormat(GL_RG32F)
                    ->textureFormat(GL_RG)
                    ->build();

            shadowDatas[light]->addTextureFilter(std::move(nullFilter));
        }

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowManager::removeShadowMaps(const Light *light) {
        unsigned int depthTextureID = shadowDatas[light]->getDepthTextureID();
        glDeleteTextures(1, &depthTextureID);

        unsigned int shadowMapTextureID = shadowDatas[light]->getShadowMapTextureID();
        glDeleteTextures(1, &shadowMapTextureID);

        unsigned int frameBufferObjectID = shadowDatas[light]->getFboID();
        glDeleteFramebuffers(1, &frameBufferObjectID);
    }

    void ShadowManager::updateVisibleModels(const Frustum<float> &frustum) {
        ScopeProfiler profiler("3d", "upVisibleModel");

        splitFrustum(frustum);

        for (auto &shadowData : shadowDatas) {
            updateFrustumShadowData(shadowData.first, shadowData.second);
        }
    }

    void ShadowManager::forceUpdateAllShadowMaps() {
        bForceUpdateAllShadowMaps = true;
    }

    void ShadowManager::updateShadowMaps() {
        ScopeProfiler profiler("3d", "updateShadowMap");

        glBindTexture(GL_TEXTURE_2D, 0);

        for (auto &shadowData : shadowDatas) {
            glViewport(0, 0, shadowMapResolution, shadowMapResolution);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowData.second->getFboID());
            glClear((unsigned int)GL_DEPTH_BUFFER_BIT | (unsigned int)GL_COLOR_BUFFER_BIT);

            shadowShaderVariable->setShadowData(shadowData.second);
            shadowModelShaderVariable->setShadowData(shadowData.second);

            shadowModelDisplayer->setModels(shadowData.second->retrieveModels());
            shadowModelDisplayer->display(shadowData.second->getLightViewMatrix());

            shadowData.second->applyTextureFilters();
        }

        glViewport(0, 0, sceneWidth, sceneHeight);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    void ShadowManager::loadShadowMaps(const std::unique_ptr<GenericRenderer> &lightingRenderer) {
        int i = 0;
        const std::vector<Light *> &visibleLights = lightManager->getVisibleLights();
        for (auto *visibleLight : visibleLights) {
            if (visibleLight->isProduceShadow()) {
                auto it = shadowDatas.find(visibleLight);
                const ShadowData *shadowData = it->second;

                unsigned int texUnit = lightingRenderer
                        ->addAdditionalTexture(Texture::build(shadowData->getFilteredShadowMapTextureID(), TextureType::ARRAY, TextureParam::buildLinear()));
                ShaderDataSender().sendData(lightsLocation[i].shadowMapTexShaderVar, static_cast<int>(texUnit));

                for (std::size_t j=0; j<nbShadowMaps; ++j) {
                    Matrix4<float> lightProjectionViewMatrix = shadowData->getFrustumShadowData(j)->getLightProjectionMatrix() * shadowData->getLightViewMatrix();
                    ShaderDataSender().sendData(lightsLocation[i].mLightProjectionViewShaderVar[j], lightProjectionViewMatrix);
                }
            }
            ++i;
        }

        auto *depthSplitDistance = new float[nbShadowMaps];
        for (unsigned int shadowMapIndex=0; shadowMapIndex<nbShadowMaps; ++shadowMapIndex) {
            float currSplitDistance = splitDistances[shadowMapIndex];
            depthSplitDistance[shadowMapIndex] = ((projectionMatrix(2, 2)*-currSplitDistance + projectionMatrix(2, 3)) / (currSplitDistance)) / 2.0f + 0.5f;
        }

        ShaderDataSender().sendData(depthSplitDistanceShaderVar, nbShadowMaps, depthSplitDistance);
        delete []depthSplitDistance;
    }

    void ShadowManager::drawLightSceneBox(const Frustum<float> &frustum, const Light *light, const Matrix4<float> &viewMatrix) const {
        auto itShadowData = shadowDatas.find(light);
        if (itShadowData==shadowDatas.end()) {
            throw std::invalid_argument("shadow manager doesn't know this light.");
        }

        const Matrix4<float> &lightViewMatrix = itShadowData->second->getLightViewMatrix();
        AABBox<float> aabboxSceneIndependent = createSceneIndependentBox(frustum, lightViewMatrix);
        OBBox<float> obboxSceneIndependentViewSpace = lightViewMatrix.inverse() * OBBox<float>(aabboxSceneIndependent);

        std::vector<Model *> models;
        modelOctreeManager->getOctreeablesIn(obboxSceneIndependentViewSpace, models, ModelProduceShadowFilter());
        if (!models.empty()) {
            AABBox<float> aabboxSceneDependent = createSceneDependentBox(aabboxSceneIndependent, obboxSceneIndependentViewSpace, models, lightViewMatrix);
            OBBox<float> obboxSceneDependentViewSpace = lightViewMatrix.inverse() * OBBox<float>(aabboxSceneDependent);

            OBBoxModel sceneDependentObboxModel(obboxSceneDependentViewSpace);
            sceneDependentObboxModel.onCameraProjectionUpdate(projectionMatrix);
            sceneDependentObboxModel.setColor(0.0, 1.0, 0.0);
            sceneDependentObboxModel.display(viewMatrix);
        }
    }

}
