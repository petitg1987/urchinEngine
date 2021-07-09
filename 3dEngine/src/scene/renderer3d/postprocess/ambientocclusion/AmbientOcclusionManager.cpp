#include <map>
#include <string>
#include <locale>
#include <random>

#include <scene/renderer3d/postprocess/ambientocclusion/AmbientOcclusionManager.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/render/GenericRendererBuilder.h>
#include <texture/filter/bilateralblur/BilateralBlurFilterBuilder.h>

namespace urchin {

    //debug parameters
    bool DEBUG_EXPORT_SSAO_KERNEL = false;

    //static
    constexpr unsigned int AmbientOcclusionManager::KERNEL_SAMPLES_SHADER_LIMIT = 64; //must be equals to 'KERNEL_SAMPLES' in AO shader

    AmbientOcclusionManager::AmbientOcclusionManager() :
            nearPlane(0.0f),
            farPlane(0.0f),

            config({}),
            textureSizeX(0),
            textureSizeY(0),

            positioningData({}),

            verticalBlurFilter(nullptr),
            horizontalBlurFilter(nullptr) {

    }

    AmbientOcclusionManager::~AmbientOcclusionManager() {
        if (offscreenRenderTarget) {
            offscreenRenderTarget->cleanup();
        }
    }

    void AmbientOcclusionManager::onTexturesUpdate(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture) {
        this->depthTexture = depthTexture;
        this->normalAndAmbientTexture = normalAndAmbientTexture;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->resolution = Vector2<float>((float)sceneWidth, (float)sceneHeight);

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::createOrUpdateAO() {
        if (depthTexture) {
            generateKernelSamples();
            generateNoiseTexture();

            createOrUpdateAOShader();
            createOrUpdateAOTexture();
            createOrUpdateRenderer();
        }
    }

    void AmbientOcclusionManager::createOrUpdateAOShader() {
        AmbientOcclusionShaderConst aoConstData{config.kernelSamples, config.radius, config.ambientOcclusionStrength, config.depthStartAttenuation,
                                                config.depthEndAttenuation, config.noiseTextureSize, config.bias};
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

        ambientOcclusionShader = ShaderBuilder::createShader("ambientOcclusion.vert.spv", "", "ambientOcclusion.frag.spv", std::move(shaderConstants));
    }

    void AmbientOcclusionManager::createOrUpdateAOTexture() {
        textureSizeX = (unsigned int)(resolution.X / (float)retrieveTextureSizeFactor());
        textureSizeY = (unsigned int)(resolution.Y / (float)retrieveTextureSizeFactor());
        ambientOcclusionTexture = Texture::build(textureSizeX, textureSizeY, TextureFormat::GRAYSCALE_16_FLOAT, nullptr);

        if (offscreenRenderTarget) {
            offscreenRenderTarget->cleanup();
        }
        offscreenRenderTarget = std::make_unique<OffscreenRender>("ambient occlusion", RenderTarget::NO_DEPTH_ATTACHMENT);
        offscreenRenderTarget->addTexture(ambientOcclusionTexture);
        offscreenRenderTarget->initialize();

        if (config.isBlurActivated) {
            verticalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>("ambient occlusion - vertical bilateral blur filter", ambientOcclusionTexture)
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(TextureFormat::GRAYSCALE_16_FLOAT)
                    ->depthTexture(depthTexture)
                    ->blurDirection(BilateralBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize(config.blurSize)
                    ->blurSharpness(config.blurSharpness)
                    ->buildBilateralBlur();

            horizontalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>("ambient occlusion - horizontal bilateral blur filter", verticalBlurFilter->getTexture())
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(TextureFormat::GRAYSCALE_16_FLOAT)
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
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        renderer = GenericRendererBuilder::create("ambient occlusion", offscreenRenderTarget, ambientOcclusionShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(positioningData), &positioningData) //binding 0
                ->addUniformData(sizeof(Vector4<float>) * ssaoKernel.size(), ssaoKernel.data()) //binding 1
                ->addUniformData(sizeof(resolution), &resolution) //binding 2
                ->addUniformTextureReader(TextureReader::build(depthTexture, TextureParam::buildNearest())) //binding 3
                ->addUniformTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest())) //binding 4
                ->addUniformTextureReader(TextureReader::build(noiseTexture, TextureParam::buildRepeatNearest())) //binding 5
                ->build();
    }

    void AmbientOcclusionManager::generateKernelSamples() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
        std::default_random_engine generator(seed);

        ssaoKernel.resize(config.kernelSamples);
        for (unsigned int i = 0; i < config.kernelSamples; ++i) {
            Vector4<float> sample(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator),
                    0.0f /* not used but necessary for shader data alignment on 16 bytes for arrays */);
            sample = sample.normalize();

            float scale = (float)i / (float)config.kernelSamples;
            scale = MathFunction::lerp<float>(0.1f, 1.0f, scale * scale); //use square function to bring most of sample closer to center
            sample *= scale;
            ssaoKernel[i] = sample;
        }

        if (DEBUG_EXPORT_SSAO_KERNEL) {
            exportSVG(SystemInfo::homeDirectory() + "ssaoKernel.html", ssaoKernel);
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
        noiseTexture = Texture::build(config.noiseTextureSize, config.noiseTextureSize, TextureFormat::RGBA_8_INT, &ssaoNoise[0]);
    }

    void AmbientOcclusionManager::exportSVG(const std::string& filename, const std::vector<Vector4<float>>& ssaoKernel) const {
        SVGExporter svgExporter(filename);
        svgExporter.addShape(std::make_unique<SVGCircle>(Point2<float>(0.0, 0.0), config.radius, SVGPolygon::BLUE));
        for (const auto& kernel : ssaoKernel) {
            svgExporter.addShape(std::make_unique<SVGCircle>(Point2<float>(kernel.X, kernel.Y), 0.001f, SVGPolygon::LIME));
        }
        svgExporter.generateSVG(100);
    }

    void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera* camera) {
        nearPlane = camera->getNearPlane();
        farPlane = camera->getFarPlane();

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::updateConfig(const Config& config) {
        if(this->config.textureSize != config.textureSize ||
                this->config.kernelSamples != config.kernelSamples ||
                this->config.radius != config.radius ||
                this->config.ambientOcclusionStrength != config.ambientOcclusionStrength ||
                this->config.depthStartAttenuation != config.depthStartAttenuation ||
                this->config.depthEndAttenuation != config.depthEndAttenuation ||
                this->config.noiseTextureSize != config.noiseTextureSize ||
                this->config.bias != config.bias ||
                this->config.isBlurActivated != config.isBlurActivated ||
                this->config.blurSize != config.blurSize ||
                this->config.blurSharpness != config.blurSharpness) {
            bool updateShader = this->config.depthStartAttenuation != config.depthStartAttenuation || this->config.depthEndAttenuation != config.depthEndAttenuation;
            bool updateTextures = this->config.noiseTextureSize != config.noiseTextureSize;

            this->config = config;
            checkConfig();

            createOrUpdateAO();
            if (updateShader) {
                createOrUpdateAOShader();
            }
            if (updateTextures) {
                onTexturesUpdate(depthTexture, normalAndAmbientTexture);
            }
        }
    }

    const AmbientOcclusionManager::Config& AmbientOcclusionManager::getConfig() const {
        return config;
    }

    void AmbientOcclusionManager::checkConfig() const {
        if(config.kernelSamples > KERNEL_SAMPLES_SHADER_LIMIT) {
            throw std::invalid_argument("Kernel samples value is limited to " + std::to_string(KERNEL_SAMPLES_SHADER_LIMIT) + ". Value: " + std::to_string(config.kernelSamples));
        }
    }

    int AmbientOcclusionManager::retrieveTextureSizeFactor() const {
        if (config.textureSize == AOTextureSize::FULL_SIZE) {
            return 1;
        }else if (config.textureSize == AOTextureSize::HALF_SIZE) {
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

    void AmbientOcclusionManager::updateAOTexture(const Camera* camera) {
        ScopeProfiler sp(Profiler::graphic(), "updateAOTexture");

        positioningData.inverseProjectionViewMatrix = (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse();
        positioningData.projectionMatrix = camera->getProjectionMatrix();
        positioningData.viewMatrix = camera->getViewMatrix();
        renderer->updateUniformData(0, &positioningData);

        offscreenRenderTarget->render();

        if (config.isBlurActivated) {
            verticalBlurFilter->applyFilter();
            horizontalBlurFilter->applyFilter();
        }
    }

    void AmbientOcclusionManager::loadAOTexture(const std::shared_ptr<GenericRenderer>& lightingRenderer, std::size_t aoTextureUnit) const {
        if (lightingRenderer->getUniformTextureReader(aoTextureUnit)->getTexture() != getAmbientOcclusionTexture()) {
            lightingRenderer->updateUniformTextureReader(aoTextureUnit, TextureReader::build(getAmbientOcclusionTexture(), TextureParam::buildLinear()));
        }
    }

}
