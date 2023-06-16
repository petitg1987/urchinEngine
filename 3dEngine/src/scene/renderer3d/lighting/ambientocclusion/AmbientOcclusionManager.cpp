#include <map>
#include <string>
#include <locale>
#include <random>

#include <scene/renderer3d/lighting/ambientocclusion/AmbientOcclusionManager.h>
#include <texture/filter/bilateralblur/BilateralBlurFilterBuilder.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/GenericComputeBuilder.h>
#include <graphics/render/target/NullRenderTarget.h>

namespace urchin {

    //debug parameters
    bool DEBUG_EXPORT_SSAO_KERNEL = false;

    AmbientOcclusionManager::AmbientOcclusionManager(const Config& config, bool useNullRenderTarget) :
            config(config),
            useNullRenderTarget(useNullRenderTarget),
            nearPlane(0.0f),
            farPlane(0.0f),

            positioningData({}),

            verticalBlurFilter(nullptr),
            horizontalBlurFilter(nullptr) {

    }

    AmbientOcclusionManager::~AmbientOcclusionManager() {
        if (renderTarget) {
            renderTarget->cleanup();
        }
    }

    void AmbientOcclusionManager::onTextureUpdate(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture) {
        this->sceneResolution = Vector2<float>((float)depthTexture->getWidth(), (float)depthTexture->getHeight());
        this->depthTexture = depthTexture;
        this->normalAndAmbientTexture = normalAndAmbientTexture;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::createOrUpdateAO() {
        if (depthTexture) {
            ScopeProfiler sp(Profiler::graphic(), "updateAO");

            generateKernelSamples();
            generateNoiseTexture();

            createOrUpdateAOTexture();
            createOrUpdateRenderer();
        }
    }

    void AmbientOcclusionManager::createOrUpdateAOTexture() {
        TextureFormat textureFormat;
        if (config.textureBits == AOTextureBits::AO_8_BITS) {
            textureFormat = TextureFormat::GRAYSCALE_8_INT;
        } else if (config.textureBits == AOTextureBits::AO_16_BITS) {
            textureFormat = TextureFormat::GRAYSCALE_16_FLOAT;
        } else {
            throw std::runtime_error("Unknown ambient occlusion texture bits: " + std::to_string(config.textureBits));
        }

        auto textureSizeX = (unsigned int)(sceneResolution.X / (float)retrieveTextureSizeFactor());
        auto textureSizeY = (unsigned int)(sceneResolution.Y / (float)retrieveTextureSizeFactor());
        ambientOcclusionTexture = Texture::build("ambient occlusion", textureSizeX, textureSizeY, textureFormat);

        if (useNullRenderTarget) {
            if (!renderTarget) {
                renderTarget = std::make_unique<NullRenderTarget>((unsigned int)sceneResolution.X, (unsigned int)sceneResolution.Y);
            }
        } else {
            if (renderTarget) {
                static_cast<OffscreenRender*>(renderTarget.get())->resetOutputTextures();
            } else {
                renderTarget = std::make_unique<OffscreenRender>("ambient occlusion", RenderTarget::NO_DEPTH_ATTACHMENT);
            }
            static_cast<OffscreenRender*>(renderTarget.get())->addOutputTexture(ambientOcclusionTexture, LoadType::NO_LOAD, std::nullopt, OutputUsage::COMPUTE);
            renderTarget->initialize();
        }

        if (config.isBlurActivated) {
            verticalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>(useNullRenderTarget, "ambient occlusion - vertical bilateral blur filter", ambientOcclusionTexture)
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(textureFormat)
                    ->depthTexture(depthTexture)
                    ->blurDirection(BilateralBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize(config.blurSize)
                    ->blurSharpness(config.blurSharpness)
                    ->buildBilateralBlur();

            horizontalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>(useNullRenderTarget, "ambient occlusion - horizontal bilateral blur filter", verticalBlurFilter->getTexture())
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(textureFormat)
                    ->depthTexture(depthTexture)
                    ->blurDirection(BilateralBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurSize(config.blurSize)
                    ->blurSharpness(config.blurSharpness)
                    ->buildBilateralBlur();

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

//        std::vector<Point2<float>> vertexCoord = {
//                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
//                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
//        };
//        std::vector<Point2<float>> textureCoord = {
//                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
//                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
//        };
        Vector2<float> aoResolution = sceneResolution / (float)retrieveTextureSizeFactor();
//        renderer = GenericRendererBuilder::create("ambient occlusion", *renderTarget, *ambientOcclusionShader, ShapeType::TRIANGLE)
//                ->addData(vertexCoord)
//                ->addData(textureCoord)
//                ->addUniformData(sizeof(projection), &projection) //binding 0
//                ->addUniformData(sizeof(positioningData), &positioningData) //binding 1
//                ->addUniformData(sizeof(Vector4<float>) * ssaoKernel.size(), ssaoKernel.data()) //binding 2
//                ->addUniformData(sizeof(aoResolution), &aoResolution) //binding 3
//                ->addUniformTextureReader(TextureReader::build(depthTexture, TextureParam::buildNearest())) //binding 4
//                ->addUniformTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest())) //binding 5
//                ->addUniformTextureReader(TextureReader::build(noiseTexture, TextureParam::buildRepeatNearest())) //binding 6
//                ->build();

        compute = GenericComputeBuilder::create("ambient occlusion comp", *renderTarget, *ambientOcclusionCompShader)
                ->addUniformData(sizeof(projection), &projection) //binding 0
                ->addUniformData(sizeof(positioningData), &positioningData) //binding 1
                ->addUniformData(sizeof(Vector4<float>) * ssaoKernel.size(), ssaoKernel.data()) //binding 2
                ->addUniformData(sizeof(aoResolution), &aoResolution) //binding 3
                ->addUniformTextureReader(TextureReader::build(depthTexture, TextureParam::buildNearest())) //binding 4
                ->addUniformTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest())) //binding 5
                ->addUniformTextureReader(TextureReader::build(noiseTexture, TextureParam::buildRepeatNearest())) //binding 6
                ->addUniformTextureOutput(ambientOcclusionTexture) //binding 7
                ->build();
    }

    void AmbientOcclusionManager::createOrUpdateAOShader() {
        AmbientOcclusionShaderConst aoConstData{config.kernelSamples, config.radius, config.ambientOcclusionStrength, config.distanceStartAttenuation,
                                                config.distanceEndAttenuation, config.noiseTextureSize, config.bias};
        std::vector<std::size_t> variablesSize = {
                sizeof(AmbientOcclusionShaderConst::kernelSamples),
                sizeof(AmbientOcclusionShaderConst::radius),
                sizeof(AmbientOcclusionShaderConst::ambientOcclusionStrength),
                sizeof(AmbientOcclusionShaderConst::depthStartAttenuation),
                sizeof(AmbientOcclusionShaderConst::depthEndAttenuation),
                sizeof(AmbientOcclusionShaderConst::noiseTextureSize),
                sizeof(AmbientOcclusionShaderConst::bias)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &aoConstData);

        if (renderTarget->isValidRenderTarget()) {
            ambientOcclusionShader = ShaderBuilder::createShader("ambientOcclusion.vert.spv", "", "ambientOcclusion.frag.spv", std::move(shaderConstants));
            ambientOcclusionCompShader = ShaderBuilder::createComputeShader("ambientOcclusion.comp.spv", std::move(shaderConstants));
        } else {
            ambientOcclusionShader = ShaderBuilder::createNullShader();
            ambientOcclusionCompShader = ShaderBuilder::createNullShader();
        }
    }

    void AmbientOcclusionManager::generateKernelSamples() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
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
        std::uniform_real_distribution<float> randomFloats(0.0f, 255.0f);
        std::default_random_engine generator(seed);

        std::vector<uint8_t> ssaoNoise;
        ssaoNoise.reserve(config.noiseTextureSize * config.noiseTextureSize * 4);
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
        svgExporter.addShape(std::make_unique<SVGCircle>(Point2<float>(0.0f, 0.0f), 1.0f, SVGPolygon::BLUE));
        for (const auto& kernel : ssaoKernel) {
            svgExporter.addShape(std::make_unique<SVGCircle>(Point2<float>(kernel.X, kernel.Y), 0.01f, SVGPolygon::LIME));
        }
        svgExporter.generateSVG();
    }

    void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera& camera) {
        nearPlane = camera.getNearPlane();
        farPlane = camera.getFarPlane();

        projection.inverseProjectionMatrix = camera.getProjectionInverseMatrix();
        projection.projectionMatrix = camera.getProjectionMatrix();

        createOrUpdateAO();
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
                this->config.bias != config.bias ||
                this->config.isBlurActivated != config.isBlurActivated ||
                this->config.blurSize != config.blurSize ||
                this->config.blurSharpness != config.blurSharpness) {

            this->config = config;
            checkConfig();

            createOrUpdateAO();
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
        if (config.textureSize == AOTextureSize::FULL_SIZE) {
            return 1;
        } else if (config.textureSize == AOTextureSize::HALF_SIZE) {
            return 2;
        }

        throw std::invalid_argument("Unknown texture size value: " + std::to_string(config.textureSize));
    }

    const std::shared_ptr<Texture>& AmbientOcclusionManager::getAmbientOcclusionTexture() const {
        if (config.isBlurActivated) {
            return horizontalBlurFilter->getTexture();
        }

        return ambientOcclusionTexture;
    }

    void AmbientOcclusionManager::updateAOTexture(std::uint32_t frameIndex, unsigned int numDependenciesToAOTexture, const Camera& camera) {
        ScopeProfiler sp(Profiler::graphic(), "updateAOTexture");

        positioningData.inverseProjectionViewMatrix = camera.getProjectionViewInverseMatrix();
        positioningData.viewMatrix = camera.getViewMatrix();
        if (renderer) {
            renderer->updateUniformData(1, &positioningData);
        } else if (compute) {
            compute->updateUniformData(1, &positioningData);
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

    void AmbientOcclusionManager::loadAOTexture(GenericRenderer& lightingRenderer, std::size_t aoTextureUnit) const {
        if (lightingRenderer.getUniformTextureReader(aoTextureUnit)->getTexture() != getAmbientOcclusionTexture().get()) {
            lightingRenderer.updateUniformTextureReader(aoTextureUnit, TextureReader::build(getAmbientOcclusionTexture(), TextureParam::buildLinear()));
        }
    }

}
