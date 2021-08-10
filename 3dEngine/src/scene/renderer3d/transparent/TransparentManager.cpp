#include <scene/renderer3d/transparent/TransparentManager.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/texture/Texture.h>

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
        accumTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::RGBA_32_FLOAT, nullptr); //TODO: update to RGBA_16_FLOAT
        accumTexture->enableClearColor(Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f));
        revealTexture = Texture::build(sceneWidth, sceneHeight, TextureFormat::GRAYSCALE_16_FLOAT, nullptr); //TODO: check if we can use 8 bit textures
        accumTexture->enableClearColor(Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f));

        if (offscreenRenderTarget) {
            offscreenRenderTarget->resetTextures();
        } else {
            offscreenRenderTarget = std::make_unique<OffscreenRender>("transparent - accum/reveal", RenderTarget::READ_WRITE_DEPTH_ATTACHMENT); //TODO review depth: should provide depth of previous pass !
        }
        offscreenRenderTarget->addTexture(accumTexture);
        offscreenRenderTarget->addTexture(revealTexture);
        offscreenRenderTarget->initialize();
    }

    void TransparentManager::createOrUpdateModelSetDisplayer() {
        modelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DIFFUSE_MODE);

        modelSetDisplayer->setCustomShader("", "modelTransparent.frag.spv", std::unique_ptr<ShaderConstants>());

        modelSetDisplayer->setCustomDepthOperations(true, false /* disable depth write */);

        BlendFunction accumBlendFunction = BlendFunction::build(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ONE);
        BlendFunction revealBlendFunction = BlendFunction::build(BlendFactor::ZERO, BlendFactor::ONE_MINUS_SRC_COLOR, BlendFactor::ZERO, BlendFactor::ONE_MINUS_SRC_COLOR);
        modelSetDisplayer->setCustomBlendFunctions({accumBlendFunction, revealBlendFunction});

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
        offscreenRenderTarget->disableAllRenderers();
        modelSetDisplayer->prepareRendering(camera.getViewMatrix());
        offscreenRenderTarget->render();
    }

}