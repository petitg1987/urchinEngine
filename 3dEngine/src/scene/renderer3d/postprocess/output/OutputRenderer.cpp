#include <scene/renderer3d/postprocess/output/OutputRenderer.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    OutputRenderer::OutputRenderer(RenderTarget& outputRenderTarget, float gammaFactor) :
            outputRenderTarget(outputRenderTarget),
            gammaFactor(gammaFactor) {

    }

    OutputRenderer::~OutputRenderer() {
        clearRenderingObjects();
    }

    void OutputRenderer::refreshInputTexture(const std::shared_ptr<Texture>& sceneTexture) {
        if (sceneTexture.get() != this->sceneTexture.get()) {
            this->sceneTexture = sceneTexture;

            createOrUpdateRenderingObjects();
        }
    }

    void OutputRenderer::onGammaFactorUpdate(float gammaFactor) {
        this->gammaFactor = gammaFactor;

        if (outputRenderer) {
            outputRenderer->updateUniformData(GAMMA_UNIFORM_BINDING, &gammaFactor);
        }
    }

    void OutputRenderer::createOrUpdateRenderingObjects() {
        clearRenderingObjects();

        if (sceneTexture) {
            createOrUpdateRenderer();
        }
    }

    void OutputRenderer::clearRenderingObjects() {
        outputRenderer.reset();
    }

    void OutputRenderer::createOrUpdateRenderer() {
        createOrUpdateShaders();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };

        outputRenderer = GenericRendererBuilder::create("output renderer", outputRenderTarget, *outputShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(GAMMA_UNIFORM_BINDING, sizeof(gammaFactor), &gammaFactor)
                ->addUniformTextureReader(SCENE_TEX_UNIFORM_BINDING, TextureReader::build(sceneTexture, TextureParam::buildNearest()))
                ->build();
    }

    void OutputRenderer::createOrUpdateShaders() {
        outputShader = ShaderBuilder::createShader("outputRenderer.vert.spv", "outputRenderer.frag.spv", outputRenderTarget.isTestMode());
    }

    void OutputRenderer::render(unsigned int renderingOrder) {
        ScopeProfiler sp(Profiler::graphic(), "outRenderer");

        outputRenderer->enableRenderer(renderingOrder);
    }

}