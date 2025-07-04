#include <scene/renderer3d/VisualConfig.h>
#include <scene/renderer3d/postprocess/antialiasing/AntiAliasingApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    TaaApplier::TaaApplier(bool isTestMode) :
            isTestMode(isTestMode),
            isEnabled(false),
            quality(AntiAliasingQuality::HIGH),
            copySceneTexToHistory(true),
            frameCount(0) {

    }

    TaaApplier::~TaaApplier() {
        freeRenderData();
    }

    void TaaApplier::enableApplier(bool enable) {
        this->isEnabled = enable;

        if (enable) {
            createOrUpdateRenderData();
        } else {
            freeRenderData();
        }
    }

    void TaaApplier::updateCamera(Camera& camera, unsigned int sceneWidth, unsigned int sceneHeight) {
        frameCount++;

        constexpr std::array HALTON_SEQUENCE_X = {0.500000f, 0.250000f, 0.750000f, 0.125000f, 0.625000f, 0.375000f, 0.875000f, 0.062500f,
            0.562500f, 0.312500f, 0.812500f, 0.187500f, 0.687500f, 0.437500f, 0.937500f, 0.031250f};
        constexpr std::array HALTON_SEQUENCE_Y = {0.333333f, 0.666667f, 0.111111f, 0.444444f, 0.777778f, 0.222222f, 0.555556f, 0.888889f,
            0.037037f, 0.370370f, 0.703704f, 0.148148f, 0.481481f, 0.814815f, 0.259259f, 0.592593f};

        std::size_t sequenceIndex = frameCount % HALTON_SEQUENCE_X.size();
        float valueX = HALTON_SEQUENCE_X[sequenceIndex] / (float)sceneWidth;
        float valueY = HALTON_SEQUENCE_Y[sequenceIndex] / (float)sceneHeight;
        camera.applyJitter(valueX, valueY);
    }

    void TaaApplier::refreshInputTexture(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& sceneTexture) {
        if (depthTexture.get() != this->depthTexture.get() || sceneTexture.get() != this->sceneTexture.get()) {
            this->depthTexture = depthTexture;
            this->sceneTexture = sceneTexture;
            createOrUpdateRenderData();
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

        outputOrHistoryTextures[0] = Texture::build("aa: output or history 1", sceneTexture->getWidth(), sceneTexture->getHeight(), VisualConfig::SCENE_HDR_TEXTURE_FORMAT);
        outputOrHistoryTextures[0]->enableTextureWriting(OutputUsage::GRAPHICS);
        outputOrHistoryTextures[1] = Texture::build("aa: output or history 2", sceneTexture->getWidth(), sceneTexture->getHeight(), VisualConfig::SCENE_HDR_TEXTURE_FORMAT);
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

        if (velocityRenderTarget) {
            velocityRenderTarget->cleanup();
            velocityRenderTarget.reset();
        }
    }

    void TaaApplier::freeResolveRenderData() {
        resolveRenderer.reset();
        taaResolveShader.reset();

        if (resolveRenderTarget) {
            resolveRenderTarget->cleanup();
            resolveRenderTarget.reset();
        }
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
                ->addUniformData(VELOCITY_POSITIONING_DATA_UNIFORM_BINDING, sizeof(positioningData), &positioningData)
                ->addUniformTextureReader(VELOCITY_DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->build();
    }

    void TaaApplier::createOrUpdateResolveRenderer() {
        taaResolveShader = ShaderBuilder::createShader("taaResolve.vert.spv", "taaResolve.frag.spv", resolveRenderTarget->isTestMode());

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
            createOrUpdateVelocityRenderer();
            createOrUpdateResolveRenderer();
        }
    }

    void TaaApplier::applyAntiAliasing(uint32_t frameIndex, unsigned int numDependenciesToAATexture, const Camera& camera) {
        generateVelocityTexture(frameIndex, camera);

        if (copySceneTexToHistory) {
            resolveRenderTarget->addPreRenderTextureCopier(TextureCopier(*sceneTexture, *outputOrHistoryTextures[getHistoryTextureIndex()]));
        }

        resolveRenderer->updateUniformTextureReader(RESOLVE_HISTORY_TEX_UNIFORM_BINDING, TextureReader::build(outputOrHistoryTextures[getHistoryTextureIndex()], TextureParam::buildLinear()));
        resolveRenderTarget->enableOnlyOutputTexture(outputOrHistoryTextures[getOutputTextureIndex()]);

        resolveRenderTarget->render(frameIndex, numDependenciesToAATexture);

        if (copySceneTexToHistory) {
            resolveRenderTarget->removeAllPreRenderTextureCopiers();
            copySceneTexToHistory = false;
        }
    }

    void TaaApplier::generateVelocityTexture(uint32_t frameIndex, const Camera& camera) {
        positioningData.inverseProjectionViewMatrix = camera.getProjectionViewInverseMatrix();
        velocityRenderer->updateUniformData(VELOCITY_POSITIONING_DATA_UNIFORM_BINDING, &positioningData);
        positioningData.previousProjectionViewMatrix = camera.getProjectionViewMatrix();

        unsigned int numDependenciesToVelocityTexture = 1;
        velocityRenderTarget->render(frameIndex, numDependenciesToVelocityTexture);
    }

}
