#include <scene/renderer3d/transparent/TransparentManager.h>
#include <scene/renderer3d/transparent/TransparentModelShaderVariable.h>
#include <scene/renderer3d/transparent/TransparentMeshFilter.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/api/vulkan/render/target/OffscreenRender.h>
#include <graphics/api/vulkan/render/target/NullRenderTarget.h>
#include <graphics/shader/ShaderBuilder.h>
#include <graphics/api/vulkan/texture/Texture.h>

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

    void TransparentManager::onTextureUpdate(const std::shared_ptr<Texture>& depthTexture) {
        this->sceneWidth = depthTexture->getWidth();
        this->sceneHeight = depthTexture->getHeight();
        this->depthTexture = depthTexture;

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
        accumulationTexture = Texture::build("transparent: accumulation", sceneWidth, sceneHeight, TextureFormat::RGBA_16_FLOAT, nullptr);
        revealTexture = Texture::build("transparent: reveal", sceneWidth, sceneHeight, TextureFormat::GRAYSCALE_8_INT, nullptr);

        if (useNullRenderTarget) {
            if (!renderTarget) {
                renderTarget = std::make_unique<NullRenderTarget>(accumulationTexture->getWidth(), accumulationTexture->getHeight());
            }
        } else {
            if (renderTarget) {
                static_cast<OffscreenRender*>(renderTarget.get())->resetOutputTextures();
            } else {
                renderTarget = std::make_unique<OffscreenRender>("transparent - accum/reveal", RenderTarget::EXTERNAL_DEPTH_ATTACHMENT);
            }
            renderTarget->setExternalDepthTexture(depthTexture);
            static_cast<OffscreenRender*>(renderTarget.get())->addOutputTexture(accumulationTexture, LoadType::LOAD_CLEAR, std::make_optional(Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f)));
            static_cast<OffscreenRender*>(renderTarget.get())->addOutputTexture(revealTexture, LoadType::LOAD_CLEAR, std::make_optional(Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f)));
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

        BlendFunction accumulationBlend = BlendFunction::build(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE);
        BlendFunction revealBlend = BlendFunction::build(BlendFactor::ZERO, BlendFactor::ONE_MINUS_SRC_COLOR, BlendFactor::ZERO, BlendFactor::ONE_MINUS_SRC_COLOR);

        modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEFAULT_MODE);
        modelSetDisplayer->setupShader("modelTransparent.vert.spv", "", "modelTransparent.frag.spv", std::move(shaderConstants));
        modelSetDisplayer->setupDepthOperations(true, false /* disable depth write */);
        modelSetDisplayer->setupBlendFunctions({accumulationBlend, revealBlend});
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

    void TransparentManager::updateTransparentTextures(std::uint32_t frameIndex, unsigned int numDependenciesToTransparentTextures, const Camera& camera) const {
        ScopeProfiler sp(Profiler::graphic(), "updateTransTex");
        unsigned int renderingOrder = 0;

        renderTarget->disableAllRenderers();
        modelSetDisplayer->prepareRendering(renderingOrder, camera.getProjectionViewMatrix());
        renderTarget->render(frameIndex, numDependenciesToTransparentTextures);
    }

    void TransparentManager::loadTransparentTextures(GenericRenderer& lightingRenderer, std::size_t accumulationTextureUnit, std::size_t revealTextureUnit) const {
        if (lightingRenderer.getUniformTextureReader(accumulationTextureUnit)->getTexture() != accumulationTexture.get()) {
            lightingRenderer.updateUniformTextureReader(accumulationTextureUnit, TextureReader::build(accumulationTexture, TextureParam::buildLinear()));
            lightingRenderer.updateUniformTextureReader(revealTextureUnit, TextureReader::build(revealTexture, TextureParam::buildLinear()));
        }
    }

}