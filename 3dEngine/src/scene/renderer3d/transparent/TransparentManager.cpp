#include <scene/renderer3d/transparent/TransparentManager.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/texture/Texture.h>
#include <scene/renderer3d/model/displayer/filter/TransparentMeshFilter.h>

namespace urchin {

    TransparentManager::TransparentManager() :
            sceneWidth(0),
            sceneHeight(0),
            camera(nullptr) {

    }

    TransparentManager::~TransparentManager() {
        if (offscreenRenderTarget) {
            offscreenRenderTarget->cleanup();
        }
    }

    void TransparentManager::onTexturesUpdate(const std::shared_ptr<Texture>& depthTexture) {
        this->depthTexture = depthTexture;

        createOrUpdateRendering();
    }

    void TransparentManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        createOrUpdateRendering();
    }

    void TransparentManager::onCameraProjectionUpdate(const Camera& camera) {
        this->camera = &camera;

        createOrUpdateRendering();
    }

    void TransparentManager::createOrUpdateRendering() {
        if (camera) {
            createOrUpdateTextures();
            createOrUpdateModelSetDisplayer();
        }
    }

    void TransparentManager::createOrUpdateTextures() {
        //TODO reduce texture size (RGBA_16_FLOAT + R_8_INT)
        accumulationTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_32_FLOAT, nullptr);
        accumulationTexture->enableClearColor(Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f));
        revealTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::GRAYSCALE_16_FLOAT, nullptr);
        revealTexture->enableClearColor(Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));

        if (offscreenRenderTarget) {
            offscreenRenderTarget->resetTextures();
        } else {
            offscreenRenderTarget = std::make_unique<OffscreenRender>("transparent - accum/reveal", RenderTarget::EXTERNAL_DEPTH_ATTACHMENT);
        }
        offscreenRenderTarget->setExternalDepthTexture(depthTexture);
        offscreenRenderTarget->addTexture(accumulationTexture);
        offscreenRenderTarget->addTexture(revealTexture);
        offscreenRenderTarget->initialize();
    }

    void TransparentManager::createOrUpdateModelSetDisplayer() {
        modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DIFFUSE_MODE);

        BlendFunction accumulationBlend = BlendFunction::build(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE);
        BlendFunction revealBlend = BlendFunction::build(BlendFactor::ZERO, BlendFactor::ONE_MINUS_SRC_COLOR, BlendFactor::ZERO, BlendFactor::ONE_MINUS_SRC_COLOR);

        modelSetDisplayer->setupShader("", "modelTransparent.frag.spv", std::unique_ptr<ShaderConstants>());
        modelSetDisplayer->setupDepthOperations(true, false /* disable depth write */);
        modelSetDisplayer->setupBlendFunctions({accumulationBlend, revealBlend});
        modelSetDisplayer->setupMeshFilter(std::make_unique<TransparentMeshFilter>());
        modelSetDisplayer->setupFaceCull(false);

        modelSetDisplayer->initialize(*offscreenRenderTarget);
        modelSetDisplayer->onCameraProjectionUpdate(*camera);
    }

    void TransparentManager::updateModels(const std::vector<Model*>& models) {
        modelSetDisplayer->setModels(models);
    }

    void TransparentManager::removeModel(Model* model) {
        modelSetDisplayer->removeModel(model);
    }

    void TransparentManager::updateTransparentTextures(const Camera& camera) {
        ScopeProfiler sp(Profiler::graphic(), "updateTransTex");

        offscreenRenderTarget->disableAllRenderers();
        modelSetDisplayer->prepareRendering(camera.getViewMatrix());
        offscreenRenderTarget->render();
    }

    void TransparentManager::loadTransparentTextures(GenericRenderer& lightingRenderer, std::size_t accumulationTextureUnit, std::size_t revealTextureUnit) const {
        if (lightingRenderer.getUniformTextureReader(accumulationTextureUnit)->getTexture() != accumulationTexture.get()) {
            lightingRenderer.updateUniformTextureReader(accumulationTextureUnit, TextureReader::build(accumulationTexture, TextureParam::buildLinear()));
            lightingRenderer.updateUniformTextureReader(revealTextureUnit, TextureReader::build(revealTexture, TextureParam::buildLinear()));
        }
    }

}