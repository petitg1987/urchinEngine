#include <cstring>

#include "scene/renderer3d/VisualConfig.h"
#include "scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h"
#include "graphics/render/shader/ShaderBuilder.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    TaaApplier::TaaApplier(bool isTestMode, bool isEnabled, AntiAliasingQuality quality) :
            isTestMode(isTestMode),
            isEnabled(isEnabled),
            quality(quality),
            currentJitter(Vector2(0.0f, 0.0f)),
            copySceneTexToHistory(true),
            frameCount(0) {
        std::memset((void*)&velocityData, 0, sizeof(velocityData));

        velocityData.currentJitter = currentJitter;
        velocityData.previousJitter = currentJitter;
    }

    TaaApplier::~TaaApplier() {
        freeRenderData();
    }

    void TaaApplier::enableApplier(bool enable) {
        this->isEnabled = enable;

        if (isEnabled) {
            createOrUpdateRenderData();
        } else {
            freeRenderData();
        }
    }

    void TaaApplier::applyCameraJitter(Camera& camera, unsigned int sceneWidth, unsigned int sceneHeight) {
        std::size_t sequenceIndex = frameCount % HALTON_SEQUENCE_X.size();
        currentJitter = Vector2((HALTON_SEQUENCE_X[sequenceIndex] * 2.0f - 1.0f) / (float)sceneWidth, (HALTON_SEQUENCE_Y[sequenceIndex] * 2.0f - 1.0f) / (float)sceneHeight);
        camera.applyJitter(currentJitter);
    }

    void TaaApplier::refreshInputTexture(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& sceneTexture) {
        if (depthTexture.get() != this->depthTexture.get() || sceneTexture.get() != this->sceneTexture.get()) {
            this->depthTexture = depthTexture;
            this->sceneTexture = sceneTexture;

            if (isEnabled) {
                createOrUpdateRenderData();
            }
        }
    }

    int TaaApplier::getOutputTextureIndex() const {
        return frameCount % 2 == 0 ? 0 : 1;
    }

    int TaaApplier::getHistoryTextureIndex() const {
        return frameCount % 2 == 1 ? 0 : 1;
    }

    const std::shared_ptr<Texture>& TaaApplier::getOutputTexture() const {
        return outputOrHistoryTextures[getOutputTextureIndex()];
    }

    void TaaApplier::createOrUpdateRenderData() {
        createOrUpdateVelocityRenderData();
        createOrUpdateResolveRenderData();
    }

    void TaaApplier::createOrUpdateVelocityRenderData() {
        freeVelocityRenderData();

        //A pixel with a coordinates of (x, y) and a value of (0.012, 0) means this pixel has moved of the distance (0.0010416, 0) between previous frame and current frame.
        //A velocity of x=1.0 represents a full move from the left of the screen to the right of the screen.
        velocityTexture = Texture::build("aa: velocity", sceneTexture->getWidth(), sceneTexture->getHeight(), TextureFormat::RG_16_FLOAT);

        velocityRenderTarget = std::make_unique<OffscreenRender>("anti aliasing - velocity", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        velocityRenderTarget->addOutputTexture(velocityTexture);
        velocityRenderTarget->initialize();

        createOrUpdateVelocityRenderer();
    }

    void TaaApplier::createOrUpdateResolveRenderData() {
        freeResolveRenderData();

        outputOrHistoryTextures[0] = Texture::build("aa: output or history 1", sceneTexture->getWidth(), sceneTexture->getHeight(), VisualConfig::SCENE_TEXTURE_FORMAT);
        outputOrHistoryTextures[0]->enableTextureWriting(OutputUsage::GRAPHICS);
        outputOrHistoryTextures[1] = Texture::build("aa: output or history 2", sceneTexture->getWidth(), sceneTexture->getHeight(), VisualConfig::SCENE_TEXTURE_FORMAT);
        outputOrHistoryTextures[1]->enableTextureWriting(OutputUsage::GRAPHICS);

        resolveRenderTarget = std::make_unique<OffscreenRender>("anti aliasing - resolve", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        resolveRenderTarget->addOutputTexture(outputOrHistoryTextures[getOutputTextureIndex()]);
        resolveRenderTarget->addOutputTexture(outputOrHistoryTextures[getHistoryTextureIndex()]);
        resolveRenderTarget->enableOnlyOutputTexture(outputOrHistoryTextures[getOutputTextureIndex()]);
        resolveRenderTarget->initialize();

        createOrUpdateResolveRenderer();
        copySceneTexToHistory = true;
    }

    void TaaApplier::freeRenderData() {
        freeVelocityRenderData();
        freeResolveRenderData();
    }

    void TaaApplier::freeVelocityRenderData() {
        velocityRenderer.reset();
        taaVelocityShader.reset();
        velocityRenderTarget.reset();
    }

    void TaaApplier::freeResolveRenderData() {
        resolveRenderer.reset();
        taaResolveShader.reset();
        resolveRenderTarget.reset();
        for (std::shared_ptr<Texture>& outputOrHistoryTexture : outputOrHistoryTextures) {
            outputOrHistoryTexture.reset();
        }
    }

    void TaaApplier::createOrUpdateVelocityRenderer() {
        taaVelocityShader = ShaderBuilder::createShader("taaVelocity.vert.spv", "taaVelocity.frag.spv", velocityRenderTarget->isTestMode());

        std::vector vertexCoord = {
            Point2(-1.0f, -1.0f), Point2(1.0f, -1.0f), Point2(1.0f, 1.0f),
            Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f), Point2(-1.0f, 1.0f)
        };
        std::vector textureCoord = {
            Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
            Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
        };
        velocityRenderer = GenericRendererBuilder::create("anti aliasing: velocity", *velocityRenderTarget, *taaVelocityShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(VELOCITY_DATA_UNIFORM_BINDING, sizeof(velocityData), &velocityData)
                ->addUniformTextureReader(VELOCITY_DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->build();
    }

    void TaaApplier::createOrUpdateResolveRenderer() {
        AntiAliasingShaderConst antiAliasingShaderConst = {.highQuality = (quality == AntiAliasingQuality::HIGH)};
        std::vector variablesSize = {sizeof(AntiAliasingShaderConst::highQuality)};
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &antiAliasingShaderConst);
        taaResolveShader = ShaderBuilder::createShader("taaResolve.vert.spv", "taaResolve.frag.spv", std::move(shaderConstants), resolveRenderTarget->isTestMode());

        std::vector vertexCoord = {
            Point2(-1.0f, -1.0f), Point2(1.0f, -1.0f), Point2(1.0f, 1.0f),
            Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f), Point2(-1.0f, 1.0f)
        };
        std::vector textureCoord = {
            Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
            Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
        };
        resolveRenderer = GenericRendererBuilder::create("anti aliasing: resolve", *resolveRenderTarget, *taaResolveShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(RESOLVE_SCENE_TEX_UNIFORM_BINDING, TextureReader::build(sceneTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(RESOLVE_DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(RESOLVE_VELOCITY_TEX_UNIFORM_BINDING, TextureReader::build(velocityTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(RESOLVE_HISTORY_TEX_UNIFORM_BINDING, TextureReader::build(outputOrHistoryTextures[getHistoryTextureIndex()], TextureParam::buildLinear()))
                ->build();
    }

    void TaaApplier::updateQuality(AntiAliasingQuality quality) {
        this->quality = quality;

        if (isEnabled) {
            createOrUpdateResolveRenderer();
        }
    }

    void TaaApplier::applyAntiAliasing(uint32_t frameCount, unsigned int numDependenciesToAATexture, const Camera& camera) {
        assert(isEnabled);
        this->frameCount = frameCount;

        generateVelocityTexture(frameCount, camera);

        if (copySceneTexToHistory) {
            resolveRenderTarget->addPreRenderTextureCopier(TextureCopier(*sceneTexture, *outputOrHistoryTextures[getHistoryTextureIndex()]));
        }

        resolveRenderer->updateUniformTextureReader(RESOLVE_HISTORY_TEX_UNIFORM_BINDING, TextureReader::build(outputOrHistoryTextures[getHistoryTextureIndex()], TextureParam::buildLinear()));
        resolveRenderTarget->enableOnlyOutputTexture(outputOrHistoryTextures[getOutputTextureIndex()]);

        resolveRenderTarget->render(frameCount, numDependenciesToAATexture);

        if (copySceneTexToHistory) {
            resolveRenderTarget->removeAllPreRenderTextureCopiers();
            copySceneTexToHistory = false;
        }
    }

    void TaaApplier::generateVelocityTexture(uint32_t frameCount, const Camera& camera) {
        velocityData.inverseProjectionViewMatrix = camera.getProjectionViewInverseMatrix();
        velocityData.currentJitter = currentJitter;
        velocityRenderer->updateUniformData(VELOCITY_DATA_UNIFORM_BINDING, &velocityData);
        velocityData.previousProjectionViewMatrix = camera.getProjectionViewMatrix();
        velocityData.previousJitter = currentJitter;

        unsigned int numDependenciesToVelocityTexture = 1;
        velocityRenderTarget->render(frameCount, numDependenciesToVelocityTexture);
    }

}
