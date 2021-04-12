#include <map>
#include <string>
#include <locale>
#include <random>

#include "AmbientOcclusionManager.h"
#include "graphic/render/shader/builder/ShaderBuilder.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "texture/filter/bilateralblur/BilateralBlurFilterBuilder.h"

#define DEFAULT_TEXTURE_SIZE AOTextureSize::HALF_SIZE
#define DEFAULT_KERNEL_SAMPLES 32
#define DEFAULT_RADIUS 0.35f
#define DEFAULT_AO_STRENGTH 0.10f
#define DEFAULT_DEPTH_START_ATTENUATION 0.999f
#define DEFAULT_DEPTH_END_ATTENUATION 0.9995f
#define DEFAULT_NOISE_TEXTURE_SIZE 4
#define DEFAULT_BIAS 0.15f
#define DEFAULT_BLUR_SIZE 7
#define DEFAULT_BLUR_SHARPNESS 40.0f

namespace urchin {

    //debug parameters
    constexpr bool DEBUG_EXPORT_SSAO_KERNEL = false;

    //static
    constexpr unsigned int AmbientOcclusionManager::KERNEL_SAMPLES_LIMIT = 64; //limited by default const value of 'KERNEL_SAMPLES' in AO shader

    AmbientOcclusionManager::AmbientOcclusionManager() :
            nearPlane(0.0f),
            farPlane(0.0f),

            textureSize(DEFAULT_TEXTURE_SIZE),
            textureSizeX(0),
            textureSizeY(0),
            kernelSamples(DEFAULT_KERNEL_SAMPLES),
            radius(DEFAULT_RADIUS),
            ambientOcclusionStrength(DEFAULT_AO_STRENGTH),
            depthStartAttenuation(DEFAULT_DEPTH_START_ATTENUATION),
            depthEndAttenuation(DEFAULT_DEPTH_END_ATTENUATION),
            noiseTextureSize(DEFAULT_NOISE_TEXTURE_SIZE),
            bias(DEFAULT_BIAS),
            blurSize(DEFAULT_BLUR_SIZE),
            blurSharpness(DEFAULT_BLUR_SHARPNESS),

            positioningData({}),

            verticalBlurFilter(nullptr),
            horizontalBlurFilter(nullptr),
            isBlurActivated(true) {

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
        AmbientOcclusionShaderConst aoConstData{};
        aoConstData.kernelSamples = kernelSamples;
        aoConstData.radius = radius;
        aoConstData.ambientOcclusionStrength = ambientOcclusionStrength;
        aoConstData.depthStartAttenuation = depthStartAttenuation;
        aoConstData.depthEndAttenuation = depthEndAttenuation;
        aoConstData.noiseTextureSize = noiseTextureSize;
        aoConstData.bias = bias;
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

        ambientOcclusionShader = ShaderBuilder::createShader("spirv/ambientOcclusion.vert.spv", "", "spirv/ambientOcclusion.frag.spv", std::move(shaderConstants));
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

        if (isBlurActivated) {
            verticalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>(ambientOcclusionTexture)
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(TextureFormat::GRAYSCALE_16_FLOAT)
                    ->depthTexture(depthTexture)
                    ->blurDirection(BilateralBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize(blurSize)
                    ->blurSharpness(blurSharpness)
                    ->buildBilateralBlur();

            horizontalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>(verticalBlurFilter->getTexture())
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(TextureFormat::GRAYSCALE_16_FLOAT)
                    ->depthTexture(depthTexture)
                    ->blurDirection(BilateralBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurSize(blurSize)
                    ->blurSharpness(blurSharpness)
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
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, -1.0f),
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(-1.0f, -1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f),
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f)
        };
        renderer = GenericRendererBuilder::create("ambient occlusion", offscreenRenderTarget, ambientOcclusionShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addShaderData(sizeof(positioningData), &positioningData) //binding 0
                ->addShaderData(sizeof(Vector4<float>) * ssaoKernel.size(), ssaoKernel.data()) //binding 1
                ->addShaderData(sizeof(resolution), &resolution) //binding 2
                ->addTextureReader(TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addTextureReader(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->addTextureReader(TextureReader::build(noiseTexture, TextureParam::buildRepeatNearest()))
                ->build();
    }

    void AmbientOcclusionManager::generateKernelSamples() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
        std::default_random_engine generator(seed);

        ssaoKernel.resize(kernelSamples);
        for (unsigned int i = 0; i < kernelSamples; ++i) {
            Vector4<float> sample(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator),
                    0.0f /* not used but necessary for shader data alignment on 16 bytes for arrays */);
            sample = sample.normalize();

            float scale = (float)i / (float)kernelSamples;
            scale = MathFunction::lerp<float>(0.1f, 1.0f, scale * scale); //use square function to bring most of sample closer to center
            sample *= scale;
            ssaoKernel[i] = sample;
        }

        if (DEBUG_EXPORT_SSAO_KERNEL) {
            exportSVG(std::string(std::getenv("HOME")) + "/ssaoKernel.html", ssaoKernel);
        }
    }

    void AmbientOcclusionManager::generateNoiseTexture() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0f, 255.0f);
        std::default_random_engine generator(seed);

        std::vector<uint8_t> ssaoNoise;
        ssaoNoise.reserve(noiseTextureSize * noiseTextureSize * 4);
        for (unsigned int i = 0; i < noiseTextureSize * noiseTextureSize; i++) {
            ssaoNoise.emplace_back((uint8_t)(randomFloats(generator))); //x
            ssaoNoise.emplace_back((uint8_t)(randomFloats(generator))); //y
            ssaoNoise.emplace_back(0); //z
            ssaoNoise.emplace_back(255); //w (not used)
        }
        noiseTexture = Texture::build(noiseTextureSize, noiseTextureSize, TextureFormat::RGBA_8_INT, &ssaoNoise[0]);
    }

    void AmbientOcclusionManager::exportSVG(const std::string& filename, const std::vector<Vector4<float>>& ssaoKernel) const {
        SVGExporter svgExporter(filename);
        svgExporter.addShape(new SVGCircle(Point2<float>(0.0, 0.0), radius, SVGPolygon::BLUE));
        for (const auto& kernel : ssaoKernel) {
            svgExporter.addShape(new SVGCircle(Point2<float>(kernel.X, kernel.Y), 0.001f, SVGPolygon::LIME));
        }
        svgExporter.generateSVG(100);
    }

    void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera* camera) {
        nearPlane = camera->getNearPlane();
        farPlane = camera->getFarPlane();

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::setTextureSize(AOTextureSize textureSize) {
        this->textureSize = textureSize;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::setKernelSamples(unsigned int kernelSamples) {
        if(kernelSamples > KERNEL_SAMPLES_LIMIT) {
            throw std::runtime_error("Kernel samples value is limited to " + std::to_string(KERNEL_SAMPLES_LIMIT));
        }
        this->kernelSamples = kernelSamples;

        createOrUpdateAO();
    }

    /**
     * @param radius Scope radius in units.
     * Example: if 1.0f represents one meter, a radius of 0.5f will represent of radius of 50 centimeters.
     */
    void AmbientOcclusionManager::setRadius(float radius) {
        this->radius = radius;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::setAmbientOcclusionStrength(float ambientOcclusionStrength) {
        this->ambientOcclusionStrength = ambientOcclusionStrength;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::setDistanceAttenuation(float depthStartAttenuation, float depthEndAttenuation) {
        this->depthStartAttenuation = depthStartAttenuation;
        this->depthEndAttenuation = depthEndAttenuation;

        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setNoiseTextureSize(unsigned int noiseTextureSize) {
        this->noiseTextureSize = noiseTextureSize;

        onTexturesUpdate(depthTexture, normalAndAmbientTexture);
    }

    void AmbientOcclusionManager::setBias(float bias) {
        this->bias = bias;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::setBlurSize(unsigned int blurSize) {
        this->blurSize = blurSize;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::setBlurSharpness(float blurSharpness) {
        this->blurSharpness = blurSharpness;

        createOrUpdateAO();
    }

    void AmbientOcclusionManager::activateBlur(bool isBlurActivated) {
        this->isBlurActivated = isBlurActivated;

        createOrUpdateAO();
    }

    int AmbientOcclusionManager::retrieveTextureSizeFactor() {
        if (textureSize == AOTextureSize::FULL_SIZE) {
            return 1;
        }else if (textureSize == AOTextureSize::HALF_SIZE) {
            return 2;
        }

        throw std::invalid_argument("Unknown texture size value: " + std::to_string(textureSize));
    }

    const std::shared_ptr<Texture>& AmbientOcclusionManager::getAmbientOcclusionTexture() const {
        if (isBlurActivated) {
            return horizontalBlurFilter->getTexture();
        }

        return ambientOcclusionTexture;
    }

    void AmbientOcclusionManager::updateAOTexture(const Camera* camera) {
        ScopeProfiler sp(Profiler::graphic(), "updateAOTexture");

        positioningData.inverseProjectionViewMatrix = (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse();
        positioningData.projectionMatrix = camera->getProjectionMatrix();
        positioningData.viewMatrix = camera->getViewMatrix();
        renderer->updateShaderData(0, &positioningData);

        offscreenRenderTarget->render();

        if (isBlurActivated) {
            verticalBlurFilter->applyFilter();
            horizontalBlurFilter->applyFilter();
        }
    }

    void AmbientOcclusionManager::loadAOTexture(const std::shared_ptr<GenericRenderer>& lightingRenderer, std::size_t aoTextureUnit) const {
        if (lightingRenderer->getTextureReader(aoTextureUnit)->getTexture() != getAmbientOcclusionTexture()) {
            lightingRenderer->updateTextureReader(aoTextureUnit, TextureReader::build(getAmbientOcclusionTexture(), TextureParam::buildLinear()));
        }
    }

}
