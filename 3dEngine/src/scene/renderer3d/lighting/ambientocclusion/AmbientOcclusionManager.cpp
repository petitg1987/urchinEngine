#include <string>
#include <random>

#include <scene/renderer3d/lighting/ambientocclusion/AmbientOcclusionManager.h>
#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericComputeBuilder.h>

namespace urchin {

    //debug parameters
    constexpr auto DEBUG_EXPORT_SSAO_KERNEL = False();

    AmbientOcclusionManager::AmbientOcclusionManager(const Config& config, bool isTestMode) :
            config(config),
            isTestMode(isTestMode),
            nearPlane(0.0f),
            farPlane(0.0f),
            positioningData({}) {

    }

    AmbientOcclusionManager::~AmbientOcclusionManager() {
        if (renderTarget) {
            renderTarget->cleanup();
        }
    }

    void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera& camera) {
        nearPlane = camera.getNearPlane();
        farPlane = camera.getFarPlane();

        projection.inverseProjectionMatrix = camera.getProjectionInverseMatrix();
        projection.projectionMatrix = camera.getProjectionMatrix();

        createOrUpdateRenderingObjects();
    }

    void AmbientOcclusionManager::refreshInputTextures(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture) {
        if (depthTexture.get() != this->depthTexture.get() || normalAndAmbientTexture.get() != this->normalAndAmbientTexture.get()) {
            this->sceneResolution = Vector2((float)depthTexture->getWidth(), (float)depthTexture->getHeight());
            this->depthTexture = depthTexture;
            this->normalAndAmbientTexture = normalAndAmbientTexture;

            createOrUpdateRenderingObjects();
        }
    }

    void AmbientOcclusionManager::createOrUpdateRenderingObjects() {
        if (depthTexture) {
            ScopeProfiler sp(Profiler::graphic(), "updateAO");

            generateKernelSamples();
            generateNoiseTexture();

            createOrUpdateRenderTarget();
            createOrUpdateRenderer();
        }
    }

    void AmbientOcclusionManager::createOrUpdateRenderTarget() {
        TextureFormat textureFormat;
        if (config.textureBits == AO_8_BITS) {
            textureFormat = TextureFormat::GRAYSCALE_8_INT;
        } else if (config.textureBits == AO_16_BITS) {
            textureFormat = TextureFormat::GRAYSCALE_16_FLOAT;
        } else {
            throw std::runtime_error("Unknown ambient occlusion texture bits: " + std::to_string(config.textureBits));
        }

        auto textureSizeX = (unsigned int)(sceneResolution.X / (float)retrieveTextureSizeFactor());
        auto textureSizeY = (unsigned int)(sceneResolution.Y / (float)retrieveTextureSizeFactor());
        ambientOcclusionTexture = Texture::build("ambient occlusion", textureSizeX, textureSizeY, textureFormat);

        if (renderTarget) {
            renderTarget->resetOutput();
        } else {
            renderTarget = std::make_unique<OffscreenRender>("ambient occlusion", isTestMode, RenderTarget::NO_DEPTH_ATTACHMENT);
        }
        renderTarget->addOutputTexture(ambientOcclusionTexture, LoadType::NO_LOAD, std::nullopt, OutputUsage::COMPUTE);
        renderTarget->initialize();

        if (config.isBlurActivated) {
            verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(isTestMode, "ambient occlusion - vertical blur", ambientOcclusionTexture)
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(textureFormat)
                    ->depthTexture(depthTexture)
                    ->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurRadius(config.blurRadius)
                    ->maxBlurDistance(config.maxBlurDistance)
                    ->buildGaussianBlur();

            horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(isTestMode, "ambient occlusion - horizontal blur", verticalBlurFilter->getTexture())
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(textureFormat)
                    ->depthTexture(depthTexture)
                    ->blurDirection(GaussianBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurRadius(config.blurRadius)
                    ->maxBlurDistance(config.maxBlurDistance)
                    ->buildGaussianBlur();

            verticalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
            horizontalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
        } else {
            verticalBlurFilter = nullptr;
            horizontalBlurFilter = nullptr;
        }
    }

    void AmbientOcclusionManager::createOrUpdateRenderer() {
        assert(renderTarget);

        createOrUpdateAOShader();

        compute = GenericComputeBuilder::create("ambient occlusion comp", *renderTarget, *ambientOcclusionShader, Vector2(8, 8))
                ->addUniformData(PROJECTION_UNIFORM_BINDING, sizeof(projection), &projection)
                ->addUniformData(POSITIONING_DATA_UNIFORM_BINDING, sizeof(positioningData), &positioningData)
                ->addUniformData(SSAO_KERNEL_UNIFORM_BINDING, sizeof(Vector4<float>) * ssaoKernel.size(), ssaoKernel.data())
                ->addUniformTextureReader(DEPTH_TEX_UNIFORM_BINDING, TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(NORMAL_AMBIENT_TEX_UNIFORM_BINDING, TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->addUniformTextureReader(NOISE_TEX_UNIFORM_BINDING, TextureReader::build(noiseTexture, TextureParam::buildRepeatNearest()))
                ->addUniformTextureOutput(AO_TEX_UNIFORM_BINDING, ambientOcclusionTexture)
                ->build();
    }

    void AmbientOcclusionManager::createOrUpdateAOShader() {
        AmbientOcclusionShaderConst aoConstData{
                .kernelSamples = config.kernelSamples,
                .radius = config.radius,
                .depthStartAttenuation = config.distanceStartAttenuation,
                .depthEndAttenuation = config.distanceEndAttenuation,
                .biasMultiplier = config.biasMultiplier,
                .biasDistanceMultiplier = config.biasDistanceMultiplier
        };
        std::vector variablesSize = {
                sizeof(AmbientOcclusionShaderConst::kernelSamples),
                sizeof(AmbientOcclusionShaderConst::radius),
                sizeof(AmbientOcclusionShaderConst::depthStartAttenuation),
                sizeof(AmbientOcclusionShaderConst::depthEndAttenuation),
                sizeof(AmbientOcclusionShaderConst::biasMultiplier),
                sizeof(AmbientOcclusionShaderConst::biasDistanceMultiplier),
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &aoConstData);
        ambientOcclusionShader = ShaderBuilder::createComputeShader("ambientOcclusion.comp.spv", std::move(shaderConstants), renderTarget->isTestMode());
    }

    void AmbientOcclusionManager::generateKernelSamples() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution randomFloats(0.0f, 1.0f);
        std::default_random_engine generator(seed);

        ssaoKernel.resize(config.kernelSamples);
        for (unsigned int i = 0; i < config.kernelSamples; ++i) {
            Vector4 sample(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator),
                    0.0f /* not used but necessary for shader data alignment on 16 bytes for arrays */);
            sample = sample.normalize();

            float scale = (float)i / (float)config.kernelSamples;
            scale = std::lerp(0.1f, 1.0f, scale * scale); //use square function to bring most of the samples closer to center
            sample *= scale;
            ssaoKernel[i] = sample;
        }

        if (DEBUG_EXPORT_SSAO_KERNEL) {
            exportSVG(SystemInfo::homeDirectory() + "ssaoKernel.svg", ssaoKernel);
        }
    }

    void AmbientOcclusionManager::generateNoiseTexture() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution randomFloats(0.0f, 255.0f);
        std::default_random_engine generator(seed);

        std::vector<uint8_t> ssaoNoise;
        ssaoNoise.reserve((std::size_t)config.noiseTextureSize * (std::size_t)config.noiseTextureSize * 4);
        for (unsigned int i = 0; i < config.noiseTextureSize * config.noiseTextureSize; i++) {
            ssaoNoise.emplace_back((uint8_t)(randomFloats(generator))); //x
            ssaoNoise.emplace_back((uint8_t)(randomFloats(generator))); //y
            ssaoNoise.emplace_back(0); //z
            ssaoNoise.emplace_back(255); //w (not used)
        }
        noiseTexture = Texture::build("AO noise", config.noiseTextureSize, config.noiseTextureSize, TextureFormat::RGBA_8_INT, ssaoNoise.data(), TextureDataType::INT_8);
    }

    void AmbientOcclusionManager::exportSVG(std::string filename, const std::vector<Vector4<float>>& ssaoKernel) const {
        SVGExporter svgExporter(std::move(filename));
        svgExporter.addShape(std::make_unique<SVGCircle>(Point2(0.0f, 0.0f), 1.0f, BLUE));
        for (const auto& kernel : ssaoKernel) {
            svgExporter.addShape(std::make_unique<SVGCircle>(Point2(kernel.X, kernel.Y), 0.01f, LIME));
        }
        svgExporter.generateSVG();
    }

    float AmbientOcclusionManager::getAmbientOcclusionStrength() const {
        return config.ambientOcclusionStrength;
    }

    void AmbientOcclusionManager::updateConfig(const Config& config) {
        if (this->config.textureSize != config.textureSize ||
                this->config.textureBits != config.textureBits ||
                this->config.kernelSamples != config.kernelSamples ||
                this->config.radius != config.radius ||
                this->config.ambientOcclusionStrength != config.ambientOcclusionStrength ||
                this->config.distanceStartAttenuation != config.distanceStartAttenuation ||
                this->config.distanceEndAttenuation != config.distanceEndAttenuation ||
                this->config.noiseTextureSize != config.noiseTextureSize ||
                this->config.biasMultiplier != config.biasMultiplier ||
                this->config.biasDistanceMultiplier != config.biasDistanceMultiplier ||
                this->config.isBlurActivated != config.isBlurActivated ||
                this->config.blurRadius != config.blurRadius ||
                this->config.maxBlurDistance != config.maxBlurDistance) {
            bool ambientOcclusionStrengthUpdated = this->config.ambientOcclusionStrength != config.ambientOcclusionStrength;

            this->config = config;
            checkConfig();

            createOrUpdateRenderingObjects();
            if (ambientOcclusionStrengthUpdated) {
                notifyObservers(this, AMBIENT_OCCLUSION_STRENGTH_UPDATE);
            }
        }
    }

    const AmbientOcclusionManager::Config& AmbientOcclusionManager::getConfig() const {
        return config;
    }

    void AmbientOcclusionManager::checkConfig() const {
        if (config.kernelSamples > KERNEL_SAMPLES_SHADER_LIMIT) {
            throw std::invalid_argument("Kernel samples value is limited to " + std::to_string(KERNEL_SAMPLES_SHADER_LIMIT) + ". Value: " + std::to_string(config.kernelSamples));
        }
    }

    int AmbientOcclusionManager::retrieveTextureSizeFactor() const {
        if (config.textureSize == FULL_SIZE) {
            return 1;
        } else if (config.textureSize == HALF_SIZE) {
            return 2;
        }
        throw std::invalid_argument("Unknown texture size value: " + std::to_string(config.textureSize));
    }

    void AmbientOcclusionManager::updateAOTexture(uint32_t frameIndex, unsigned int numDependenciesToAOTexture, const Camera& camera) {
        ScopeProfiler sp(Profiler::graphic(), "updateAOTexture");

        positioningData.inverseProjectionViewMatrix = camera.getProjectionViewInverseMatrix();
        positioningData.viewMatrix = camera.getViewMatrix();
        if (compute) {
            compute->updateUniformData(POSITIONING_DATA_UNIFORM_BINDING, &positioningData);
        }

        if (config.isBlurActivated) {
            unsigned int numDependenciesToRawAOTexture = 1 /* vertical blur filter */;
            renderTarget->render(frameIndex, numDependenciesToRawAOTexture);

            unsigned int numDependenciesToVerticalBlurFilterOutputs = 1 /* horizontal blur filter */;
            verticalBlurFilter->applyFilter(frameIndex, numDependenciesToVerticalBlurFilterOutputs);

            unsigned int numDependenciesToHorizontalBlurFilterOutputs = numDependenciesToAOTexture;
            horizontalBlurFilter->applyFilter(frameIndex, numDependenciesToHorizontalBlurFilterOutputs);
        } else {
            unsigned int numDependenciesToRawAOTexture = numDependenciesToAOTexture;
            renderTarget->render(frameIndex, numDependenciesToRawAOTexture);
        }
    }

    const std::shared_ptr<Texture>& AmbientOcclusionManager::getAmbientOcclusionTexture() const {
        if (config.isBlurActivated) {
            return horizontalBlurFilter->getTexture();
        }
        return ambientOcclusionTexture;
    }

    void AmbientOcclusionManager::loadAOTexture(GenericRenderer& deferredSecondPassRenderer, uint32_t aoTextureUniformBinding) const {
        if (deferredSecondPassRenderer.getUniformTextureReader(aoTextureUniformBinding)->getTexture() != getAmbientOcclusionTexture().get()) {
            deferredSecondPassRenderer.updateUniformTextureReader(aoTextureUniformBinding, TextureReader::build(getAmbientOcclusionTexture(), TextureParam::buildLinear()));
        }
    }

}
