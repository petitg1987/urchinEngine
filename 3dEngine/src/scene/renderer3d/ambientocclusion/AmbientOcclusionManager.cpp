#include <map>
#include <string>
#include <cmath>
#include <locale>
#include <limits>
#include <random>

#include "AmbientOcclusionManager.h"
#include "utils/shader/ShaderManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"
#include "utils/filter/bilateralblur/BilateralBlurFilterBuilder.h"

#define DEFAULT_TEXTURE_SIZE AOTextureSize::HALF_SIZE
#define DEFAULT_KERNEL_SAMPLES 64
#define DEFAULT_RADIUS 0.35
#define DEFAULT_AO_STRENGTH 0.10
#define DEFAULT_DEPTH_START_ATTENUATION 0.999
#define DEFAULT_DEPTH_END_ATTENUATION 0.9995
#define DEFAULT_NOISE_TEXTURE_SIZE 4
#define DEFAULT_BIAS 0.15
#define DEFAULT_BLUR_SIZE 7
#define DEFAULT_BLUR_SHARPNESS 40.0

namespace urchin
{

	AmbientOcclusionManager::AmbientOcclusionManager(unsigned int depthTexID, unsigned int normalAndAmbientTexID) :
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


		fboID(0),
		ambientOcclusionTexID(0),

		ambientOcclusionShader(0),
		mInverseViewProjectionLoc(0),
        mProjectionLoc(0),
		mViewLoc(0),
        resolutionLoc(0),
		noiseTexId(0),

		depthTexID(depthTexID),
		normalAndAmbientTexID(normalAndAmbientTexID),
		ambientOcclusionTexLoc(0),
		verticalBlurFilter(nullptr),
		horizontalBlurFilter(nullptr),
		isBlurActivated(true)
	{
		glBindTexture(GL_TEXTURE_2D, depthTexID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, normalAndAmbientTexID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()->build();

		//frame buffer object
		glGenFramebuffers(1, &fboID);
		createOrUpdateAOTexture();
		createOrUpdateAOShader();
	}

	AmbientOcclusionManager::~AmbientOcclusionManager()
	{
		glDeleteFramebuffers(1, &fboID);
		glDeleteTextures(1, &ambientOcclusionTexID);

		glDeleteTextures(1, &noiseTexId);
	}

	void AmbientOcclusionManager::createOrUpdateAOShader()
	{
		std::locale::global(std::locale("C")); //for float

		std::map<std::string, std::string> ambientOcclusionTokens;
		ambientOcclusionTokens["KERNEL_SAMPLES"] = std::to_string(kernelSamples);
		ambientOcclusionTokens["RADIUS"] = std::to_string(radius);
		ambientOcclusionTokens["AO_STRENGTH"] = std::to_string(ambientOcclusionStrength);
		ambientOcclusionTokens["DEPTH_START_ATTENUATION"] = std::to_string(depthStartAttenuation);
		ambientOcclusionTokens["DEPTH_END_ATTENUATION"] = std::to_string(depthEndAttenuation);
		ambientOcclusionTokens["NOISE_TEXTURE_SIZE"] = std::to_string(noiseTextureSize);
		ambientOcclusionTokens["BIAS"] = std::to_string(bias);
		ShaderManager::instance()->removeProgram(ambientOcclusionShader);
		ambientOcclusionShader = ShaderManager::instance()->createProgram("ambientOcclusion.vert", "", "ambientOcclusion.frag", ambientOcclusionTokens);
		ShaderManager::instance()->bind(ambientOcclusionShader);

		mInverseViewProjectionLoc = glGetUniformLocation(ambientOcclusionShader, "mInverseViewProjection");
		mProjectionLoc = glGetUniformLocation(ambientOcclusionShader, "mProjection");
		mViewLoc = glGetUniformLocation(ambientOcclusionShader, "mView");
		int depthTexLoc = glGetUniformLocation(ambientOcclusionShader, "depthTex");
		glUniform1i(depthTexLoc, GL_TEXTURE0-GL_TEXTURE0);
		int normalAndAmbientTexLoc = glGetUniformLocation(ambientOcclusionShader, "normalAndAmbientTex");
		glUniform1i(normalAndAmbientTexLoc, GL_TEXTURE1-GL_TEXTURE0);
        resolutionLoc = glGetUniformLocation(ambientOcclusionShader, "resolution");

		generateKernelSamples();
		generateNoiseTexture();
	}

	void AmbientOcclusionManager::loadUniformLocationFor(unsigned int deferredShaderID)
	{
		ShaderManager::instance()->bind(deferredShaderID);
		ambientOcclusionTexLoc = glGetUniformLocation(deferredShaderID, "ambientOcclusionTex");
	}

	void AmbientOcclusionManager::onResize(unsigned int width, unsigned int height)
	{
		sceneWidth = width;
		sceneHeight = height;
		
		createOrUpdateAOTexture();
		createOrUpdateAOShader();

		ShaderManager::instance()->bind(ambientOcclusionShader);
        Vector2<float> resolution(sceneWidth, sceneHeight);
        glUniform2fv(resolutionLoc, 1, (const float *)resolution);
	}

	void AmbientOcclusionManager::createOrUpdateAOTexture()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		GLenum fboAttachments[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, fboAttachments);
		glReadBuffer(GL_NONE);

		glDeleteTextures(1, &ambientOcclusionTexID);
		glGenTextures(1, &ambientOcclusionTexID);

		this->textureSizeX = sceneWidth / retrieveTextureSizeFactor();
		this->textureSizeY = sceneHeight / retrieveTextureSizeFactor();

		glBindTexture(GL_TEXTURE_2D, ambientOcclusionTexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, textureSizeX, textureSizeY, 0, GL_RED, GL_FLOAT, nullptr);
		glFramebufferTexture(GL_FRAMEBUFFER, fboAttachments[0], ambientOcclusionTexID, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		verticalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>()
				->textureSize(textureSizeX, textureSizeY)
				->textureType(GL_TEXTURE_2D)
				->textureInternalFormat(GL_R16F)
				->textureFormat(GL_RED)
				->textureAccessFilter(GL_NEAREST)
				->blurDirection(BilateralBlurFilterBuilder::VERTICAL_BLUR)
				->blurSize(blurSize)
				->blurSharpness(blurSharpness)
				->depthTextureID(depthTexID)
				->buildBilateralBlur();

		horizontalBlurFilter = std::make_unique<BilateralBlurFilterBuilder>()
				->textureSize(textureSizeX, textureSizeY)
				->textureType(GL_TEXTURE_2D)
				->textureInternalFormat(GL_R16F)
				->textureFormat(GL_RED)
				->textureAccessFilter(GL_LINEAR)
				->blurDirection(BilateralBlurFilterBuilder::HORIZONTAL_BLUR)
				->blurSize(blurSize)
				->blurSharpness(blurSharpness)
				->depthTextureID(depthTexID)
				->buildBilateralBlur();

		verticalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
		horizontalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
	}

	void AmbientOcclusionManager::generateKernelSamples()
    {
		unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator(seed);

        std::vector<Vector3<float>> ssaoKernel;
        for (unsigned int i = 0; i < kernelSamples; ++i)
        {
            Vector3<float> sample(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator));
            sample = sample.normalize();

            float scale = static_cast<float>(i) / kernelSamples;
            scale = MathAlgorithm::lerp<float>(0.1f, 1.0f, scale * scale); //use square function to bring most of sample closer to center
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        ShaderManager::instance()->bind(ambientOcclusionShader);
        int samplesLoc = glGetUniformLocation(ambientOcclusionShader, "samples");
        glUniform3fv(samplesLoc, ssaoKernel.size(), (const float *)ssaoKernel[0]);

        #ifdef _DEBUG
//      	exportSVG(std::string(std::getenv("HOME")) + "/ssaoKernel.html", ssaoKernel);
        #endif
    }

	void AmbientOcclusionManager::generateNoiseTexture()
	{
		unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator(seed);

        std::vector<Vector3<float>> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            Vector3<float> noise(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    0.0f);
            ssaoNoise.push_back(noise.normalize());
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glGenTextures(1, &noiseTexId);
        glBindTexture(GL_TEXTURE_2D, noiseTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, noiseTextureSize, noiseTextureSize, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        ShaderManager::instance()->bind(ambientOcclusionShader);
        int noiseTexLoc = glGetUniformLocation(ambientOcclusionShader, "noiseTex");
        glUniform1i(noiseTexLoc, GL_TEXTURE2-GL_TEXTURE0);
	}

	void AmbientOcclusionManager::exportSVG(const std::string &filename, const std::vector<Vector3<float>> &ssaoKernel) const
    {
        SVGExporter svgExporter(filename);
        svgExporter.addShape(new SVGCircle(Point2<float>(0.0, 0.0), radius, SVGPolygon::BLUE));
        for (const auto &kernel : ssaoKernel)
        {
            svgExporter.addShape(new SVGCircle(Point2<float>(kernel.X, kernel.Y), 0.001, SVGPolygon::LIME));
        }
        svgExporter.generateSVG(100);
	}

	void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera *camera)
	{
		nearPlane = camera->getNearPlane();
		farPlane = camera->getFarPlane();
		projectionScale = camera->getProjectionMatrix()(1, 1);

		createOrUpdateAOTexture();
		createOrUpdateAOShader();
	}

	void AmbientOcclusionManager::setTextureSize(AOTextureSize textureSize)
	{
		this->textureSize = textureSize;

		createOrUpdateAOTexture();
		createOrUpdateAOShader();
	}

	void AmbientOcclusionManager::setKernelSamples(unsigned int kernelSamples)
	{
		this->kernelSamples = kernelSamples;

		createOrUpdateAOTexture();
		createOrUpdateAOShader();
	}

	/**
	 * @param radius Scope radius in units.
	 * Example: if 1.0f represents one meter, a radius of 0.5f will represent of radius of 50 centimeters.
	 */
	void AmbientOcclusionManager::setRadius(float radius)
	{
		this->radius = radius;

		createOrUpdateAOShader();
	}

	void AmbientOcclusionManager::setAmbientOcclusionStrength(float ambientOcclusionStrength)
	{
		this->ambientOcclusionStrength = ambientOcclusionStrength;

		createOrUpdateAOShader();
	}

    void AmbientOcclusionManager::setDistanceAttenuation(float depthStartAttenuation, float depthEndAttenuation)
    {
        this->depthStartAttenuation = depthStartAttenuation;
        this->depthEndAttenuation = depthEndAttenuation;

		createOrUpdateAOShader();
    }

    void AmbientOcclusionManager::setNoiseTextureSize(unsigned int noiseTextureSize)
    {
	    this->noiseTextureSize = noiseTextureSize;

		createOrUpdateAOShader();
	}

	void AmbientOcclusionManager::setBias(float bias)
	{
		this->bias = bias;

		createOrUpdateAOShader();
	}

	void AmbientOcclusionManager::setBlurSize(unsigned int blurSize)
	{
		this->blurSize = blurSize;

		createOrUpdateAOTexture();
	}

	void AmbientOcclusionManager::setBlurSharpness(float blurSharpness)
	{
		this->blurSharpness = blurSharpness;

		createOrUpdateAOTexture();
	}

	void AmbientOcclusionManager::activateBlur(bool isBlurActivated)
	{
		this->isBlurActivated = isBlurActivated;
	}

	int AmbientOcclusionManager::retrieveTextureSizeFactor()
	{
		if(textureSize==AOTextureSize::FULL_SIZE)
		{
			return 1;
		}
		if(textureSize==AOTextureSize::HALF_SIZE)
		{
			return 2;
		}

		throw std::invalid_argument("Unknown texture size value: " + std::to_string(textureSize));
	}

	unsigned int AmbientOcclusionManager::getAmbientOcclusionTextureID() const
	{
		if(isBlurActivated)
		{
			return horizontalBlurFilter->getTextureID();
		}

		return ambientOcclusionTexID;
	}

	void AmbientOcclusionManager::updateAOTexture(const Camera *camera)
	{
		ScopeProfiler profiler("3d", "updateAOTexture");

		GLint activeFBO;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &activeFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalAndAmbientTexID);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexId);

		ShaderManager::instance()->bind(ambientOcclusionShader);
		glUniformMatrix4fv(mInverseViewProjectionLoc, 1, GL_FALSE, (const float*) (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse());
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*) camera->getProjectionMatrix());
		glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*) camera->getViewMatrix());

		glViewport(0, 0, textureSizeX, textureSizeY);

		quadDisplayer->display();

		if(isBlurActivated)
		{
			verticalBlurFilter->applyOn(ambientOcclusionTexID);
			horizontalBlurFilter->applyOn(verticalBlurFilter->getTextureID());
		}

		glViewport(0, 0, sceneWidth, sceneHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(activeFBO));
	}

	void AmbientOcclusionManager::loadAOTexture(unsigned int ambientOcclusionTextureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + ambientOcclusionTextureUnit);
		glBindTexture(GL_TEXTURE_2D, getAmbientOcclusionTextureID());

		glUniform1i(ambientOcclusionTexLoc, ambientOcclusionTextureUnit);
	}

}
