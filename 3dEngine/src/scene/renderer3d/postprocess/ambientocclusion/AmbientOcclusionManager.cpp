#include <map>
#include <string>
#include <locale>
#include <random>

#include "AmbientOcclusionManager.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "texture/filter/bilateralblur/BilateralBlurFilterBuilder.h"

#define DEFAULT_TEXTURE_SIZE AOTextureSize::HALF_SIZE
#define DEFAULT_KERNEL_SAMPLES 64
#define DEFAULT_RADIUS 0.35f
#define DEFAULT_AO_STRENGTH 0.10f
#define DEFAULT_DEPTH_START_ATTENUATION 0.999f
#define DEFAULT_DEPTH_END_ATTENUATION 0.9995f
#define DEFAULT_NOISE_TEXTURE_SIZE 4
#define DEFAULT_BIAS 0.15f
#define DEFAULT_BLUR_SIZE 7
#define DEFAULT_BLUR_SHARPNESS 40.0f

namespace urchin {

    //Debug parameters
    bool DEBUG_EXPORT_SSAO_KERNEL = false;

    AmbientOcclusionManager::AmbientOcclusionManager() :
            sceneWidth(0),
            sceneHeight(0),
            nearPlane(0.0f),
            farPlane(0.0f),
            projectionScale(0.0f),

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

            verticalBlurFilter(nullptr),
            horizontalBlurFilter(nullptr),
            isBlurActivated(true) {

    }

    void AmbientOcclusionManager::createOrUpdateAOShader() {
        std::locale::global(std::locale("C")); //for float

        std::map<std::string, std::string> ambientOcclusionTokens;
        ambientOcclusionTokens["KERNEL_SAMPLES"] = std::to_string(kernelSamples);
        ambientOcclusionTokens["RADIUS"] = std::to_string(radius);
        ambientOcclusionTokens["AO_STRENGTH"] = std::to_string(ambientOcclusionStrength);
        ambientOcclusionTokens["DEPTH_START_ATTENUATION"] = std::to_string(depthStartAttenuation);
        ambientOcclusionTokens["DEPTH_END_ATTENUATION"] = std::to_string(depthEndAttenuation);
        ambientOcclusionTokens["NOISE_TEXTURE_SIZE"] = std::to_string(noiseTextureSize);
        ambientOcclusionTokens["BIAS"] = std::to_string(bias);
        ambientOcclusionShader = ShaderBuilder().createShader("ambientOcclusion.vert", "", "ambientOcclusion.frag", ambientOcclusionTokens);

        mInverseViewProjectionShaderVar = ShaderVar(ambientOcclusionShader, "mInverseViewProjection");
        mProjectionShaderVar = ShaderVar(ambientOcclusionShader, "mProjection");
        mViewShaderVar = ShaderVar(ambientOcclusionShader, "mView");
        resolutionShaderVar = ShaderVar(ambientOcclusionShader, "resolution");

        int depthTexUnit = 0;
        int normalAndAmbientTexUnit = 1;
        int noiseTexUnit = 2;
        ShaderDataSender()
                .sendData(ShaderVar(ambientOcclusionShader, "depthTex"), depthTexUnit)
                .sendData(ShaderVar(ambientOcclusionShader, "normalAndAmbientTex"), normalAndAmbientTexUnit)
                .sendData(ShaderVar(ambientOcclusionShader, "noiseTex"), noiseTexUnit);

        generateKernelSamples();
    }

    void AmbientOcclusionManager::initiateShaderVariables(const std::unique_ptr<Shader>& lightingShader) {
        ambientOcclusionTexShaderVar = ShaderVar(lightingShader, "ambientOcclusionTex");
    }

    void AmbientOcclusionManager::onTexturesUpdate(const std::shared_ptr<Texture>& depthTexture, const std::shared_ptr<Texture>& normalAndAmbientTexture) {
        this->depthTexture = depthTexture;
        generateNoiseTexture();

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, -1.0f),
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(-1.0f, -1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f),
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f)
        };
        renderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(depthTexture, TextureParam::buildNearest()))
                ->addTexture(TextureReader::build(normalAndAmbientTexture, TextureParam::buildNearest()))
                ->addTexture(TextureReader::build(noiseTexture, TextureParam::buildRepeatNearest()))
                ->build();

        createOrUpdateAOTexture();
        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        createOrUpdateAOTexture();
        createOrUpdateAOShader();

        ShaderDataSender().sendData(resolutionShaderVar, Vector2<float>((float)sceneWidth, (float)sceneHeight));
    }

    void AmbientOcclusionManager::createOrUpdateAOTexture() {
        if(depthTexture) {
            textureSizeX = (unsigned int)((float)sceneWidth / (float)retrieveTextureSizeFactor());
            textureSizeY = (unsigned int)((float)sceneHeight / (float)retrieveTextureSizeFactor());
            ambientOcclusionTexture = Texture::build(textureSizeX, textureSizeY, TextureFormat::GRAYSCALE_16_FLOAT, nullptr);

            offscreenRenderTarget = std::make_unique<OffscreenRender>();
            offscreenRenderTarget->onResize(textureSizeX, textureSizeY);
            offscreenRenderTarget->addTexture(ambientOcclusionTexture);

            verticalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>()
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(TextureFormat::GRAYSCALE_16_FLOAT)
                    ->blurDirection(BilateralBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize(blurSize)
                    ->blurSharpness(blurSharpness)
                    ->depthTexture(depthTexture)
                    ->buildBilateralBlur();

            horizontalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>()
                    ->textureSize(textureSizeX, textureSizeY)
                    ->textureType(TextureType::DEFAULT)
                    ->textureFormat(TextureFormat::GRAYSCALE_16_FLOAT)
                    ->blurDirection(BilateralBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurSize(blurSize)
                    ->blurSharpness(blurSharpness)
                    ->depthTexture(depthTexture)
                    ->buildBilateralBlur();

            verticalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
            horizontalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
        }
    }

    void AmbientOcclusionManager::generateKernelSamples() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator(seed);

        std::vector<Vector3<float>> ssaoKernel;
        for (unsigned int i = 0; i < kernelSamples; ++i) {
            Vector3<float> sample(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator));
            sample = sample.normalize();

            float scale = static_cast<float>(i) / static_cast<float>(kernelSamples);
            scale = MathFunction::lerp<float>(0.1f, 1.0f, scale * scale); //use square function to bring most of sample closer to center
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        ShaderDataSender().sendData(ShaderVar(ambientOcclusionShader, "samples"), (unsigned int)ssaoKernel.size(), &ssaoKernel[0]);

        if (DEBUG_EXPORT_SSAO_KERNEL) {
            exportSVG(std::string(std::getenv("HOME")) + "/ssaoKernel.html", ssaoKernel);
        }
    }

    void AmbientOcclusionManager::generateNoiseTexture() {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator(seed);

        std::vector<Vector3<float>> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++) {
            Vector3<float> noise(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    0.0f);
            ssaoNoise.push_back(noise.normalize());
        }

        noiseTexture = Texture::build(noiseTextureSize, noiseTextureSize, TextureFormat::RGB_16_FLOAT, &ssaoNoise[0]);
    }

    void AmbientOcclusionManager::exportSVG(const std::string& filename, const std::vector<Vector3<float>>& ssaoKernel) const {
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
        projectionScale = camera->getProjectionMatrix()(1, 1);

        createOrUpdateAOTexture();
        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setTextureSize(AOTextureSize textureSize) {
        this->textureSize = textureSize;

        createOrUpdateAOTexture();
        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setKernelSamples(unsigned int kernelSamples) {
        this->kernelSamples = kernelSamples;

        createOrUpdateAOTexture();
        createOrUpdateAOShader();
    }

    /**
     * @param radius Scope radius in units.
     * Example: if 1.0f represents one meter, a radius of 0.5f will represent of radius of 50 centimeters.
     */
    void AmbientOcclusionManager::setRadius(float radius) {
        this->radius = radius;

        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setAmbientOcclusionStrength(float ambientOcclusionStrength) {
        this->ambientOcclusionStrength = ambientOcclusionStrength;

        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setDistanceAttenuation(float depthStartAttenuation, float depthEndAttenuation) {
        this->depthStartAttenuation = depthStartAttenuation;
        this->depthEndAttenuation = depthEndAttenuation;

        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setNoiseTextureSize(unsigned int noiseTextureSize) {
        this->noiseTextureSize = noiseTextureSize;

        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setBias(float bias) {
        this->bias = bias;

        createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setBlurSize(unsigned int blurSize) {
        this->blurSize = blurSize;

        createOrUpdateAOTexture();
    }

    void AmbientOcclusionManager::setBlurSharpness(float blurSharpness) {
        this->blurSharpness = blurSharpness;

        createOrUpdateAOTexture();
    }

    void AmbientOcclusionManager::activateBlur(bool isBlurActivated) {
        this->isBlurActivated = isBlurActivated;
    }

    int AmbientOcclusionManager::retrieveTextureSizeFactor() {
        if (textureSize==AOTextureSize::FULL_SIZE) {
            return 1;
        }
        if (textureSize==AOTextureSize::HALF_SIZE) {
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

        ShaderDataSender()
            .sendData(mInverseViewProjectionShaderVar, (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse())
            .sendData(mProjectionShaderVar, camera->getProjectionMatrix())
            .sendData(mViewShaderVar, camera->getViewMatrix());

        offscreenRenderTarget->activeShader(ambientOcclusionShader);
        offscreenRenderTarget->display(renderer);

        if (isBlurActivated) {
            verticalBlurFilter->applyOn(ambientOcclusionTexture);
            horizontalBlurFilter->applyOn(verticalBlurFilter->getTexture());
        }
    }

    void AmbientOcclusionManager::loadAOTexture(const std::unique_ptr<GenericRenderer>& lightingRenderer) const {
        unsigned int aoTextureUnit = lightingRenderer->addAdditionalTexture(TextureReader::build(getAmbientOcclusionTexture(), TextureParam::buildLinear()));
        ShaderDataSender().sendData(ambientOcclusionTexShaderVar, (int)aoTextureUnit);
    }

}
