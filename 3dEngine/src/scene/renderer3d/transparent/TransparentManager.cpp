#include <scene/renderer3d/transparent/TransparentManager.h>
#include <scene/renderer3d/transparent/TransparentModelShaderVariable.h>
#include <scene/renderer3d/transparent/TransparentMeshFilter.h>
#include <graphics/render/target/NullRenderTarget.h>

namespace urchin {

    TransparentManager::TransparentManager(bool useNullRenderTarget, LightManager& lightManager) :
            useNullRenderTarget(useNullRenderTarget),
            lightManager(lightManager),
            sceneWidth(0),
            sceneHeight(0),
            camera(nullptr) {

    }

    TransparentManager::~TransparentManager() {
        if (renderTarget) {
            renderTarget->cleanup();
        }
    }

    void TransparentManager::onTextureUpdate(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& illuminatedTexture) {
        this->sceneWidth = depthTexture->getWidth();
        this->sceneHeight = depthTexture->getHeight();
        this->depthTexture = depthTexture;
        this->illuminatedTexture = illuminatedTexture;

        createOrUpdateRendering();
    }

    void TransparentManager::onCameraProjectionUpdate(const Camera& camera) {
        this->camera = &camera;

        createOrUpdateRendering();
    }

    void TransparentManager::createOrUpdateRendering() {
        if (camera && sceneWidth != 0 && sceneHeight != 0) {
            createOrUpdateTextures();
            createOrUpdateModelSetDisplayer();
        }
    }

    void TransparentManager::createOrUpdateTextures() {
        outputTexture = Texture::build("transparent", sceneWidth, sceneHeight, TextureFormat::RGBA_16_FLOAT);
        outputTexture->enableTextureWriting(OutputUsage::GRAPHICS);

        if (useNullRenderTarget) {
            if (!renderTarget) {
                renderTarget = std::make_unique<NullRenderTarget>(outputTexture->getWidth(), outputTexture->getHeight());
            }
        } else {
            if (renderTarget) {
                static_cast<OffscreenRender*>(renderTarget.get())->resetOutputTextures();
            } else {
                renderTarget = std::make_unique<OffscreenRender>("transparent", RenderTarget::EXTERNAL_DEPTH_ATTACHMENT);
            }
            renderTarget->setExternalDepthTexture(depthTexture);
            renderTarget->addPreRenderTextureCopier(TextureCopier(*illuminatedTexture, *outputTexture));
            static_cast<OffscreenRender*>(renderTarget.get())->addOutputTexture(outputTexture, LoadType::LOAD_CONTENT);
            renderTarget->initialize();
        }
    }

    void TransparentManager::createOrUpdateModelSetDisplayer() {
        ModelTransparentShaderConst modelTransparentConstData{};
        modelTransparentConstData.maxLights = lightManager.getMaxLights();
        modelTransparentConstData.maxEmissiveFactor = Material::MAX_EMISSIVE_FACTOR;
        std::vector<std::size_t> variablesSize = {
                sizeof(modelTransparentConstData.maxLights),
                sizeof(modelTransparentConstData.maxEmissiveFactor)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &modelTransparentConstData);

        modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_NO_INSTANCING_MODE);
        modelSetDisplayer->setupShader("modelTransparent.vert.spv", "", "modelTransparent.frag.spv", std::move(shaderConstants));
        modelSetDisplayer->setupDepthOperations(true, false /* disable depth write */);
        modelSetDisplayer->setupBlendFunctions({BlendFunction::buildDefault()});
        modelSetDisplayer->setupMeshFilter(std::make_unique<TransparentMeshFilter>());
        modelSetDisplayer->setupFaceCull(false);
        modelSetDisplayer->setupCustomShaderVariable(std::make_unique<TransparentModelShaderVariable>(camera->getNearPlane(), camera->getFarPlane(), lightManager));

        modelSetDisplayer->initialize(*renderTarget);
    }

    const ModelSetDisplayer& TransparentManager::getModelSetDisplayer() const {
        return *modelSetDisplayer;
    }

    void TransparentManager::updateModels(const std::vector<Model*>& models) const {
        modelSetDisplayer->updateModels(models);
    }

    void TransparentManager::removeModel(Model* model) const {
        if (modelSetDisplayer) {
            modelSetDisplayer->removeModel(model);
        }
    }

    const std::shared_ptr<Texture>& TransparentManager::getOutputTexture() const {
        return outputTexture;
    }

    void TransparentManager::drawTransparentModels(std::uint32_t frameIndex, unsigned int numDependenciesToTransparentTextures, const Camera& camera) const {
        ScopeProfiler sp(Profiler::graphic(), "updateTransTex");
        unsigned int renderingOrder = 0;

        struct SortUserData {
            const Camera* camera = nullptr;
            std::map<const Model*, float> modelsDistanceToCamera;
        };

        SortUserData sortUserData = {.camera = &camera, .modelsDistanceToCamera = {}};
        for (const Model* model : modelSetDisplayer->getModels()) {
            Point3<float> modelPosition = model->getTransform().getPosition();
            if (!camera.getFrustum().collideWithPoint(model->getTransform().getPosition())) {
                Line3D<float> line(model->getAABBox().getMin(), model->getAABBox().getMax());
                Point3<float> intersection1;
                Point3<float> intersection2;
                bool hasIntersections = false;
                camera.getFrustum().planesIntersectPoints(line, intersection1, intersection2, hasIntersections);
                if (hasIntersections) {
                    modelPosition = LineSegment3D<float>(intersection1, intersection2).closestPoint(camera.getPosition()); //TODO check it's correct
                }
            }

            float distanceToCamera = camera.getPosition().squareDistance(modelPosition);
            sortUserData.modelsDistanceToCamera.try_emplace(model, distanceToCamera);
        }

        auto modelSorter = [](const Model* model1, const Model* model2, const void* userData) {
            const auto* sortUserData = static_cast<const SortUserData*>(userData);
            float distanceCameraToModel1 = sortUserData->modelsDistanceToCamera.at(model1);
            float distanceCameraToModel2 = sortUserData->modelsDistanceToCamera.at(model2);
            if (distanceCameraToModel1 != distanceCameraToModel2) {
                return distanceCameraToModel1 > distanceCameraToModel2;
            }
            return model1 > model2; //random but ensure the "strict weak ordering" required by std::sort
        };

        renderTarget->disableAllProcessors();
        modelSetDisplayer->prepareRendering(renderingOrder, camera.getProjectionViewMatrix(), modelSorter, &sortUserData);
        renderTarget->render(frameIndex, numDependenciesToTransparentTextures);
    }

}