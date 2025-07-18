#include <cstring>

#include <scene/renderer3d/postprocess/reflection/ReflectionApplier.h>
#include <scene/renderer3d/VisualConfig.h>
#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    ReflectionApplier::ReflectionApplier(const Config& config, bool isTestMode) :
            config(config),
            isTestMode(isTestMode),
            nearPlane(0.0f),
            farPlane(0.0f),
            projectionData({}),
            positioningData({}) {
        std::memset((void*)&projectionData, 0, sizeof(projectionData));
        std::memset((void*)&positioningData, 0, sizeof(positioningData));
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
        reflectionColorRenderTarget.reset();

        reflectionCombineRenderer.reset();
        reflectionCombineRenderTarget.reset();
    }

    void ReflectionApplier::createOrUpdateRenderTargets() {
        auto reflectionColorTextureFormat = TextureFormat::RGBA_8_INT;
        auto reflectionColorTextureSizeX = (unsigned int)((float)depthTexture->getWidth() / (float)retrieveTextureSizeFactor());
        auto reflectionColorTextureSizeY = (unsigned int)((float)depthTexture->getHeight() / (float)retrieveTextureSizeFactor());
        reflectionColorTexture = Texture::build("reflection color", reflectionColorTextureSizeX, reflectionColorTextureSizeY, reflectionColorTextureFormat);
        reflectionColorRenderTarget = std::make_unique<OffscreenRender>("reflection color", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        reflectionColorRenderTarget->addOutputTexture(reflectionColorTexture);
        reflectionColorRenderTarget->initialize();

        verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(isTestMode, "reflection color - vertical blur", reflectionColorTexture)
                ->textureSize(reflectionColorTextureSizeX, reflectionColorTextureSizeY)
                ->textureType(TextureType::DEFAULT)
                ->textureFormat(reflectionColorTextureFormat)
                ->depthTexture(depthTexture)
                ->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
                ->blurRadius(config.blurRadius)
                ->maxBlurDistance(config.maxBlurDistance)
                ->buildGaussianBlur();

        horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(isTestMode, "reflection color - horizontal blur", verticalBlurFilter->getTexture())
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

        reflectionCombineTexture = Texture::build("reflection combine", depthTexture->getWidth(), depthTexture->getHeight(), VisualConfig::SCENE_TEXTURE_FORMAT);
        reflectionCombineRenderTarget = std::make_unique<OffscreenRender>("reflection combine", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        reflectionCombineRenderTarget->addOutputTexture(reflectionCombineTexture);
        reflectionCombineRenderTarget->initialize();
    }

    void ReflectionApplier::createOrUpdateRenderers() {
        createOrUpdateShaders();

        std::vector vertexCoord = {
                Point2(-1.0f, -1.0f), Point2(1.0f, -1.0f), Point2(1.0f, 1.0f),
                Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f), Point2(-1.0f, 1.0f)
        };
        std::vector textureCoord = {
                Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
                Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
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

        reflectionCombineRenderer = GenericRendererBuilder::create("reflection combine", *reflectionCombineRenderTarget, *reflectionCombineShader, ShapeType::TRIANGLE)
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
        std::vector reflectionColorVariablesSize = {
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
        std::vector reflectionCombineVariablesSize = {
                sizeof(ReflectionCombineShaderConst::reflectionStrength),
        };
        auto reflectionCombineShaderConstants = std::make_unique<ShaderConstants>(reflectionCombineVariablesSize, &reflectionCombineConstData);
        reflectionCombineShader = ShaderBuilder::createShader("reflectionCombine.vert.spv", "reflectionCombine.frag.spv", std::move(reflectionCombineShaderConstants), reflectionCombineRenderTarget->isTestMode());
    }

    int ReflectionApplier::retrieveTextureSizeFactor() const {
        if (config.textureSize == FULL_SIZE) {
            return 1;
        } else if (config.textureSize == HALF_SIZE) {
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

    const std::shared_ptr<Texture>& ReflectionApplier::getOutputTexture() const {
        return reflectionCombineTexture;
    }

    void ReflectionApplier::applyReflection(uint32_t frameCount, unsigned int numDependenciesToReflectionCombineTexture, const Camera& camera) {
        ScopeProfiler sp(Profiler::graphic(), "applySSR");

        unsigned int numDependenciesToReflectionColorTexture = 1 /* vertical blur filter */;
        positioningData.viewMatrix = camera.getViewMatrix();
        reflectionColorRenderer->updateUniformData(POSITIONING_DATA_UNIFORM_BINDING, &positioningData);
        reflectionColorRenderTarget->render(frameCount, numDependenciesToReflectionColorTexture);

        unsigned int numDependenciesToVerticalBlurFilterOutputs = 1 /* horizontal blur filter */;
        verticalBlurFilter->applyFilter(frameCount, numDependenciesToVerticalBlurFilterOutputs);

        unsigned int numDependenciesToHorizontalBlurFilterOutputs = 1 /* reflection combine */;
        horizontalBlurFilter->applyFilter(frameCount, numDependenciesToHorizontalBlurFilterOutputs);

        reflectionCombineRenderTarget->render(frameCount, numDependenciesToReflectionCombineTexture);
    }

}
