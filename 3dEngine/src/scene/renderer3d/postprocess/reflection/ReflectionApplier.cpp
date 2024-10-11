#include <scene/renderer3d/postprocess/reflection/ReflectionApplier.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/target/NullRenderTarget.h>

namespace urchin {

    ReflectionApplier::ReflectionApplier(bool useNullRenderTarget) :
            useNullRenderTarget(useNullRenderTarget) {

    }

    ReflectionApplier::~ReflectionApplier() {
        clearRenderingObjects();
    }

    void ReflectionApplier::onCameraProjectionUpdate(const Camera& camera) {
        projectionData.projectionMatrix = camera.getProjectionMatrix();
        projectionData.inverseProjectionMatrix = camera.getProjectionInverseMatrix();

        if (reflectionColorRenderer) {
            reflectionColorRenderer->updateUniformData(PROJECTION_DATA_UNIFORM_BINDING, &projectionData);
        }
    }

    void ReflectionApplier::refreshInputTexture(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture,
                                                const std::shared_ptr<Texture>& materialTexture, const std::shared_ptr<Texture>& illuminatedTexture) {
        if (depthTexture.get() != this->depthTexture.get()
                || normalAndAmbientTexture.get() != this->normalAndAmbientTexture.get()
                || materialTexture.get() != this->materialTexture.get()
                || illuminatedTexture.get() != this->illuminatedTexture.get()) {
            this->depthTexture = depthTexture;
            this->normalAndAmbientTexture = normalAndAmbientTexture;
            this->materialTexture = materialTexture;
            this->illuminatedTexture = illuminatedTexture;

            createOrUpdateRenderingObjects();
        }
    }

    void ReflectionApplier::createOrUpdateRenderingObjects() {
        clearRenderingObjects();

        if (depthTexture) {
            createOrUpdateRenderTargets();
            createOrUpdateRenderers();
        }
    }

    void ReflectionApplier::clearRenderingObjects() {
        reflectionColorRenderer.reset();
        if (reflectionColorRenderTarget) {
            reflectionColorRenderTarget->cleanup();
            reflectionColorRenderTarget.reset();
        }

        reflectionCombineRenderer.reset();
        if (reflectionCombineRenderTarget) {
            reflectionCombineRenderTarget->cleanup();
            reflectionCombineRenderTarget.reset();
        }
    }

    void ReflectionApplier::createOrUpdateRenderTargets() {
        reflectionColorOutputTexture = Texture::build("reflectionColor", depthTexture->getWidth(), depthTexture->getHeight(), TextureFormat::RGBA_16_FLOAT); //TODO change in 8_int (check bloom is OK)
        if (useNullRenderTarget) {
            reflectionColorRenderTarget = std::make_unique<NullRenderTarget>(depthTexture->getWidth(), depthTexture->getHeight());
        } else {
            reflectionColorRenderTarget = std::make_unique<OffscreenRender>("reflectionColor", RenderTarget::NO_DEPTH_ATTACHMENT);
            static_cast<OffscreenRender*>(reflectionColorRenderTarget.get())->addOutputTexture(reflectionColorOutputTexture);
            reflectionColorRenderTarget->initialize();
        }

        reflectionCombineOutputTexture = Texture::build("reflectionCombine", depthTexture->getWidth(), depthTexture->getHeight(), TextureFormat::RGBA_16_FLOAT); //TODO correct format ?
        if (useNullRenderTarget) {
            reflectionCombineRenderTarget = std::make_unique<NullRenderTarget>(depthTexture->getWidth(), depthTexture->getHeight());
        } else {
            reflectionCombineRenderTarget = std::make_unique<OffscreenRender>("reflectionCombine", RenderTarget::NO_DEPTH_ATTACHMENT);
            static_cast<OffscreenRender*>(reflectionCombineRenderTarget.get())->addOutputTexture(reflectionCombineOutputTexture);
            reflectionCombineRenderTarget->initialize();
        }
    }

    void ReflectionApplier::createOrUpdateRenderers() {
        createOrUpdateShaders();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };

        reflectionColorRenderer = GenericRendererBuilder::create("reflectionColor", *reflectionColorRenderTarget, *reflectionColorShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(PROJECTION_DATA_UNIFORM_BINDING, sizeof(projectionData), &projectionData)
                ->addUniformData(POSITIONING_DATA_UNIFORM_BINDING, sizeof(positioningData), &positioningData)
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(NORMAL_AMBIENT_TEX_UNIFORM_BINDING, TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(MATERIAL_TEX_UNIFORM_BINDING, TextureReader::build(materialTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(R_COLOR_ILLUMINATED_TEX_UNIFORM_BINDING, TextureReader::build(illuminatedTexture, TextureParam::buildLinear()))
                ->build();

        reflectionCombineRenderer = GenericRendererBuilder::create("reflectionCombine", *reflectionCombineRenderTarget, *reflectionCombineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(R_COMBINE_ILLUMINATED_TEX_UNIFORM_BINDING, TextureReader::build(illuminatedTexture, TextureParam::buildLinear()))
                ->addUniformTextureReader(REFLECTION_COLOR_TEX_UNIFORM_BINDING, TextureReader::build(reflectionColorOutputTexture, TextureParam::buildNearest()))
                ->build();
    }

    void ReflectionApplier::createOrUpdateShaders() {
        if (reflectionColorRenderTarget->isValidRenderTarget()) {
            ReflectionColorShaderConst rcConstData {
                    .maxDistance = config.maxDistance,
                    .hitThreshold = config.hitThreshold,
                    .firstPass_skipPixelCount = config.firstPass_skipPixelCount,
                    .secondPass_numSteps = config.secondPass_numSteps
            };
            std::vector<std::size_t> variablesSize = {
                    sizeof(ReflectionColorShaderConst::maxDistance),
                    sizeof(ReflectionColorShaderConst::hitThreshold),
                    sizeof(ReflectionColorShaderConst::firstPass_skipPixelCount),
                    sizeof(ReflectionColorShaderConst::secondPass_numSteps),
            };
            auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &rcConstData);

            reflectionColorShader = ShaderBuilder::createShader("reflectionColor.vert.spv", "reflectionColor.frag.spv", std::move(shaderConstants));
        } else {
            reflectionColorShader = ShaderBuilder::createNullShader();
        }

        if (reflectionCombineRenderTarget->isValidRenderTarget()) {
            ReflectionCombineShaderConst rcConstData{
                    .reflectionStrength = config.reflectionStrength, //apply reflection strength after AO blur to not lose color precision on 8bit texture
            };
            std::vector<std::size_t> variablesSize = {
                    sizeof(ReflectionCombineShaderConst::reflectionStrength),
            };
            auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &rcConstData);

            reflectionCombineShader = ShaderBuilder::createShader("reflectionCombine.vert.spv", "reflectionCombine.frag.spv", std::move(shaderConstants));
        } else {
            reflectionCombineShader = ShaderBuilder::createNullShader();
        }
    }

    const std::shared_ptr<Texture>& ReflectionApplier::getOutputTexture() const {
        return reflectionCombineOutputTexture;
    }

    void ReflectionApplier::updateConfig(const Config& config) {
        if (this->config.textureSize != config.textureSize ||
                this->config.maxDistance != config.maxDistance ||
                this->config.hitThreshold != config.hitThreshold ||
                this->config.firstPass_skipPixelCount != config.firstPass_skipPixelCount ||
                this->config.secondPass_numSteps != config.secondPass_numSteps ||
                this->config.reflectionStrength != config.reflectionStrength) {
            this->config = config;

            createOrUpdateRenderingObjects();
        }
    }

    const ReflectionApplier::Config& ReflectionApplier::getConfig() const {
        return config;
    }

    void ReflectionApplier::applyReflection(std::uint32_t frameIndex, unsigned int numDependenciesToReflectionTexture, const Camera& camera) {
        ScopeProfiler sp(Profiler::graphic(), "applySSR");

        unsigned int numDependenciesToReflectionColorTexture = 1;
        positioningData.viewMatrix = camera.getViewMatrix();
        reflectionColorRenderer->updateUniformData(POSITIONING_DATA_UNIFORM_BINDING, &positioningData);
        reflectionColorRenderTarget->render(frameIndex, numDependenciesToReflectionColorTexture);

        reflectionCombineRenderTarget->render(frameIndex, numDependenciesToReflectionTexture);
    }

}
