#include <scene/renderer3d/postprocess/reflection/ReflectionApplier.h>
#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    ReflectionApplier::ReflectionApplier(const Config& config, RenderTarget& outputRenderTarget) :
            config(config),
            outputRenderTarget(outputRenderTarget),
            nearPlane(0.0f),
            farPlane(0.0f) {

    }

    ReflectionApplier::~ReflectionApplier() {
        clearRenderingObjects();
    }

    void ReflectionApplier::onCameraProjectionUpdate(const Camera& camera) {
        nearPlane = camera.getNearPlane();
        farPlane = camera.getFarPlane();

        projectionData.projectionMatrix = camera.getProjectionMatrix();
        projectionData.inverseProjectionMatrix = camera.getProjectionInverseMatrix();

        if (reflectionColorRenderer) {
            reflectionColorRenderer->updateUniformData(PROJECTION_DATA_UNIFORM_BINDING, &projectionData);
        }
    }

    void ReflectionApplier::refreshInputTexture(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture,
                                                const std::shared_ptr<Texture>& materialTexture, const std::shared_ptr<Texture>& sceneTexture) {
        if (depthTexture.get() != this->depthTexture.get()
                || normalAndAmbientTexture.get() != this->normalAndAmbientTexture.get()
                || materialTexture.get() != this->materialTexture.get()
                || sceneTexture.get() != this->sceneTexture.get()) {
            this->depthTexture = depthTexture;
            this->normalAndAmbientTexture = normalAndAmbientTexture;
            this->materialTexture = materialTexture;
            this->sceneTexture = sceneTexture;

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
    }

    void ReflectionApplier::createOrUpdateRenderTargets() {
        TextureFormat reflectionColorTextureFormat = TextureFormat::RGBA_8_INT;
        auto reflectionColorTextureSizeX = (unsigned int)((float)depthTexture->getWidth() / (float)retrieveTextureSizeFactor());
        auto reflectionColorTextureSizeY = (unsigned int)((float)depthTexture->getHeight() / (float)retrieveTextureSizeFactor());
        reflectionColorTexture = Texture::build("reflection color", reflectionColorTextureSizeX, reflectionColorTextureSizeY, reflectionColorTextureFormat);

        reflectionColorRenderTarget = std::make_unique<OffscreenRender>("reflection color", outputRenderTarget.isTestMode(), RenderTarget::NO_DEPTH_ATTACHMENT);
        static_cast<OffscreenRender*>(reflectionColorRenderTarget.get())->addOutputTexture(reflectionColorTexture);
        reflectionColorRenderTarget->initialize();

        verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(outputRenderTarget.isTestMode(), "reflection color - vertical blur", reflectionColorTexture)
                ->textureSize(reflectionColorTextureSizeX, reflectionColorTextureSizeY)
                ->textureType(TextureType::DEFAULT)
                ->textureFormat(reflectionColorTextureFormat)
                ->depthTexture(depthTexture)
                ->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
                ->blurRadius(config.blurRadius)
                ->maxBlurDistance(config.maxBlurDistance)
                ->buildGaussianBlur();

        horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(outputRenderTarget.isTestMode(), "reflection color - horizontal blur", verticalBlurFilter->getTexture())
                ->textureSize(reflectionColorTextureSizeX, reflectionColorTextureSizeY)
                ->textureType(TextureType::DEFAULT)
                ->textureFormat(reflectionColorTextureFormat)
                ->depthTexture(depthTexture)
                ->blurDirection(GaussianBlurFilterBuilder::HORIZONTAL_BLUR)
                ->blurRadius(config.blurRadius)
                ->maxBlurDistance(config.maxBlurDistance)
                ->buildGaussianBlur();

        verticalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
        horizontalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
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

        reflectionColorRenderer = GenericRendererBuilder::create("reflection color", *reflectionColorRenderTarget, *reflectionColorShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(PROJECTION_DATA_UNIFORM_BINDING, sizeof(projectionData), &projectionData)
                ->addUniformData(POSITIONING_DATA_UNIFORM_BINDING, sizeof(positioningData), &positioningData)
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(NORMAL_AMBIENT_TEX_UNIFORM_BINDING, TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(MATERIAL_TEX_UNIFORM_BINDING, TextureReader::build(materialTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(R_COLOR_SCENE_TEX_UNIFORM_BINDING, TextureReader::build(sceneTexture, TextureParam::buildNearest()))
                ->build();

        reflectionCombineRenderer = GenericRendererBuilder::create("reflection combine", outputRenderTarget, *reflectionCombineShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(R_COMBINE_SCENE_TEX_UNIFORM_BINDING, TextureReader::build(sceneTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(REFLECTION_COLOR_TEX_UNIFORM_BINDING, TextureReader::build(horizontalBlurFilter->getTexture(), TextureParam::buildNearest()))
                ->build();
    }

    void ReflectionApplier::createOrUpdateShaders() {
        ReflectionColorShaderConst reflectionColorConstData {
                .maxDistance = config.maxDistance,
                .hitThreshold = config.hitThreshold,
                .skipPixelCountOnFirstPass = config.skipPixelCountOnFirstPass,
                .numStepsOnSecondPass = config.numStepsOnSecondPass
        };
        std::vector<std::size_t> reflectionColorVariablesSize = {
                sizeof(ReflectionColorShaderConst::maxDistance),
                sizeof(ReflectionColorShaderConst::hitThreshold),
                sizeof(ReflectionColorShaderConst::skipPixelCountOnFirstPass),
                sizeof(ReflectionColorShaderConst::numStepsOnSecondPass),
        };
        auto reflectionColorShaderConstants = std::make_unique<ShaderConstants>(reflectionColorVariablesSize, &reflectionColorConstData);
        reflectionColorShader = ShaderBuilder::createShader("reflectionColor.vert.spv", "reflectionColor.frag.spv", std::move(reflectionColorShaderConstants), reflectionColorRenderTarget->isTestMode());

        ReflectionCombineShaderConst reflectionCombineConstData {
                .reflectionStrength = config.reflectionStrength, //apply reflection strength after AO blur to not lose color precision on 8bit texture
        };
        std::vector<std::size_t> reflectionCombineVariablesSize = {
                sizeof(ReflectionCombineShaderConst::reflectionStrength),
        };
        auto reflectionCombineShaderConstants = std::make_unique<ShaderConstants>(reflectionCombineVariablesSize, &reflectionCombineConstData);
        reflectionCombineShader = ShaderBuilder::createShader("reflectionCombine.vert.spv", "reflectionCombine.frag.spv", std::move(reflectionCombineShaderConstants), outputRenderTarget.isTestMode());
    }

    int ReflectionApplier::retrieveTextureSizeFactor() const {
        if (config.textureSize == ReflectionTextureSize::FULL_SIZE) {
            return 1;
        } else if (config.textureSize == ReflectionTextureSize::HALF_SIZE) {
            return 2;
        }
        throw std::invalid_argument("Unknown texture size value: " + std::to_string(config.textureSize));
    }

    void ReflectionApplier::updateConfig(const Config& config) {
        if (this->config.textureSize != config.textureSize ||
                this->config.maxDistance != config.maxDistance ||
                this->config.hitThreshold != config.hitThreshold ||
                this->config.skipPixelCountOnFirstPass != config.skipPixelCountOnFirstPass ||
                this->config.numStepsOnSecondPass != config.numStepsOnSecondPass ||
                this->config.reflectionStrength != config.reflectionStrength ||
                this->config.blurRadius != config.blurRadius ||
                this->config.maxBlurDistance != config.maxBlurDistance) {
            this->config = config;

            createOrUpdateRenderingObjects();
        }
    }

    const ReflectionApplier::Config& ReflectionApplier::getConfig() const {
        return config;
    }

    void ReflectionApplier::applyReflection(std::uint32_t frameIndex, unsigned int renderingOrder, const Camera& camera) {
        ScopeProfiler sp(Profiler::graphic(), "applySSR");

        unsigned int numDependenciesToReflectionColorTexture = 1;
        positioningData.viewMatrix = camera.getViewMatrix();
        reflectionColorRenderer->updateUniformData(POSITIONING_DATA_UNIFORM_BINDING, &positioningData);
        reflectionColorRenderTarget->render(frameIndex, numDependenciesToReflectionColorTexture);

        unsigned int numDependenciesToVerticalBlurFilterOutputs = 1 /* horizontal blur filter */;
        verticalBlurFilter->applyFilter(frameIndex, numDependenciesToVerticalBlurFilterOutputs);

        unsigned int numDependenciesToHorizontalBlurFilterOutputs = 1 /* reflection combine */;
        horizontalBlurFilter->applyFilter(frameIndex, numDependenciesToHorizontalBlurFilterOutputs);

        reflectionCombineRenderer->enableRenderer(renderingOrder);
    }

}
