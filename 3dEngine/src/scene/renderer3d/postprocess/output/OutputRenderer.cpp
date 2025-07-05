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
        if (!this->sceneTexture) {
            this->sceneTexture = sceneTexture;
            createOrUpdateRenderingObjects();
        } else if (sceneTexture.get() != this->sceneTexture.get()) {
            bool sizeUpdated = this->sceneTexture->getWidth() != sceneTexture->getWidth() || this->sceneTexture->getHeight() != sceneTexture->getHeight();
            this->sceneTexture = sceneTexture;
            if (sizeUpdated) {
                createOrUpdateRenderingObjects();
            } else {
                outputRenderer->updateUniformTextureReader(SCENE_TEX_UNIFORM_BINDING, TextureReader::build(this->sceneTexture, TextureParam::buildNearest()));
            }
        }
    }

    void OutputRenderer::onGammaFactorUpdate(float gammaFactor) {
        this->gammaFactor = gammaFactor;

        createOrUpdateRenderingObjects();
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

        std::vector vertexCoord = {
                Point2(-1.0f, -1.0f), Point2(1.0f, -1.0f), Point2(1.0f, 1.0f),
                Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f), Point2(-1.0f, 1.0f)
        };
        std::vector textureCoord = {
                Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
                Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
        };

        outputRenderer = GenericRendererBuilder::create("output renderer", outputRenderTarget, *outputShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(SCENE_TEX_UNIFORM_BINDING, TextureReader::build(sceneTexture, TextureParam::buildNearest()))
                ->build();
    }

    void OutputRenderer::createOrUpdateShaders() {
        OutputShaderConst outputConstData {
            .gammaFactor = gammaFactor
        };
        std::vector variablesSize = {
            sizeof(OutputShaderConst::gammaFactor)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &outputConstData);
        outputShader = ShaderBuilder::createShader("outputRenderer.vert.spv", "outputRenderer.frag.spv", std::move(shaderConstants), outputRenderTarget.isTestMode());
    }

    void OutputRenderer::render(unsigned int renderingOrder) const {
        ScopeProfiler sp(Profiler::graphic(), "outRenderer");

        outputRenderer->enableRenderer(renderingOrder);
    }

}