#include <GL/gl.h>
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
#define DEFAULT_NUM_DIRECTIONS 6
#define DEFAULT_NUM_STEPS 6
#define DEFAULT_RADIUS 0.1
#define DEFAULT_AO_EXPONENT 1.6
#define DEFAULT_BIAS_ANGLE_IN_DEGREE 10.0
#define DEFAULT_BLUR_SIZE 7
#define DEFAULT_BLUR_SHARPNESS 40.0
#define RANDOM_TEXTURE_SIZE 4

namespace urchin
{

	AmbientOcclusionManager::AmbientOcclusionManager(unsigned int depthTexID, unsigned int normalAndAmbientTexID) :
		sceneWidth(0),
		sceneHeight(0),
		nearPlane(0.0f),
		farPlane(0.0f),
		projectionScale(0.0f),

		textureSize((AOTextureSize)DEFAULT_TEXTURE_SIZE),
		textureSizeX(0),
		textureSizeY(0),
		numDirections(DEFAULT_NUM_DIRECTIONS),
		numSteps(DEFAULT_NUM_STEPS),
		radius(DEFAULT_RADIUS),
		ambientOcclusionExponent(DEFAULT_AO_EXPONENT),
		biasAngleInDegree(DEFAULT_BIAS_ANGLE_IN_DEGREE),
		blurSize(DEFAULT_BLUR_SIZE),
		blurSharpness(DEFAULT_BLUR_SHARPNESS),
		randomTextureSize(RANDOM_TEXTURE_SIZE),

		fboID(0),
		ambientOcclusionTexID(0),

		hbaoShader(0),
		mInverseViewProjectionLoc(0),
		cameraPlanesLoc(0),
		invResolutionLoc(0),
		nearPlaneScreenRadiusLoc(0),
		randomTexID(0),

		depthTexID(depthTexID),
		normalAndAmbientTexID(normalAndAmbientTexID),
		ambienOcclusionTexLoc(0),
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

		glDeleteTextures(1, &randomTexID);
	}

	void AmbientOcclusionManager::createOrUpdateAOShader()
	{
		std::locale::global(std::locale("C")); //for float

		std::map<std::string, std::string> hbaoTokens;
		hbaoTokens["NUM_DIRECTIONS"] = std::to_string(numDirections);
		hbaoTokens["NUM_STEPS"] = std::to_string(numSteps);
		hbaoTokens["RADIUS"] = std::to_string(radius);
		hbaoTokens["AO_EXPONENT"] = std::to_string(ambientOcclusionExponent);
		hbaoTokens["TEXTURE_SIZE_FACTOR"] = std::to_string(1.0f / static_cast<float>(retrieveTextureSizeFactor()));
		hbaoTokens["BIAS_ANGLE"] = std::to_string(std::cos((90.0f-biasAngleInDegree) / (180.0f/M_PI)));
		hbaoTokens["RANDOM_TEXTURE_SIZE"] = std::to_string(randomTextureSize);
		ShaderManager::instance()->removeProgram(hbaoShader);
		hbaoShader = ShaderManager::instance()->createProgram("hbao.vert", "hbao.frag", hbaoTokens);
		ShaderManager::instance()->bind(hbaoShader);

		mInverseViewProjectionLoc = glGetUniformLocation(hbaoShader, "mInverseViewProjection");
		cameraPlanesLoc = glGetUniformLocation(hbaoShader, "cameraPlanes");
		int depthTexLoc = glGetUniformLocation(hbaoShader, "depthTex");
		glUniform1i(depthTexLoc, GL_TEXTURE0-GL_TEXTURE0);
		int normalAndAmbientTexLoc = glGetUniformLocation(hbaoShader, "normalAndAmbientTex");
		glUniform1i(normalAndAmbientTexLoc, GL_TEXTURE1-GL_TEXTURE0);
		invResolutionLoc = glGetUniformLocation(hbaoShader, "invResolution");
		nearPlaneScreenRadiusLoc = glGetUniformLocation(hbaoShader, "nearPlaneScreenRadius");

		float nearPlaneScreenRadiusInPixel = radius * projectionScale * sceneHeight;
		glUniform1f(nearPlaneScreenRadiusLoc, nearPlaneScreenRadiusInPixel);

		float cameraPlanes[2] = {nearPlane, farPlane};
		glUniform1fv(cameraPlanesLoc, 2, cameraPlanes);

		generateRandomTexture(hbaoShader);
	}

	void AmbientOcclusionManager::loadUniformLocationFor(unsigned int deferredShaderID)
	{
		ShaderManager::instance()->bind(deferredShaderID);
		ambienOcclusionTexLoc = glGetUniformLocation(deferredShaderID, "ambientOcclusionTex");
	}

	void AmbientOcclusionManager::onResize(int width, int height)
	{
		sceneWidth = width;
		sceneHeight = height;
		
		createOrUpdateAOTexture();
		createOrUpdateAOShader();

		ShaderManager::instance()->bind(hbaoShader);
		Vector2<float> invResolution(1.0f/sceneWidth, 1.0f/sceneHeight);
		glUniform2fv(invResolutionLoc, 1, invResolution);
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, textureSizeX, textureSizeY, 0, GL_RED, GL_FLOAT, 0);
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

	void AmbientOcclusionManager::generateRandomTexture(unsigned int hbaoShader)
	{
		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

		Vector3<float> *hbaoRandom = new Vector3<float>[randomTextureSize*randomTextureSize];
		for(unsigned int i=0; i<randomTextureSize*randomTextureSize; ++i)
		{
			//random rotation
			float randomAngleRadian = (2.0f*M_PI) * (distribution(generator) / static_cast<float>(numDirections));
			hbaoRandom[i].X = std::sin(randomAngleRadian);
			hbaoRandom[i].Y = std::cos(randomAngleRadian);

			//random step distance
			hbaoRandom[i].Z = distribution(generator);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glGenTextures(1, &randomTexID);
		glBindTexture (GL_TEXTURE_2D, randomTexID);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, randomTextureSize, randomTextureSize, 0, GL_RGB, GL_FLOAT, hbaoRandom);
		delete [] hbaoRandom;

		ShaderManager::instance()->bind(hbaoShader);
		int randomTexLoc = glGetUniformLocation(hbaoShader, "randomTex");
		glUniform1i(randomTexLoc, GL_TEXTURE2-GL_TEXTURE0);
	}

	void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera *const camera)
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

	void AmbientOcclusionManager::setNumDirections(unsigned int numDirections)
	{
		this->numDirections = numDirections;

		createOrUpdateAOTexture();
		createOrUpdateAOShader();
	}

	void AmbientOcclusionManager::setNumSteps(unsigned int numSteps)
	{
		this->numSteps = numSteps;

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

	void AmbientOcclusionManager::setAmbientOcclusionExponent(float ambientOcclusionExponent)
	{
		this->ambientOcclusionExponent = ambientOcclusionExponent;

		createOrUpdateAOShader();
	}

	/**
	 * @param biasAngle Bias angle in degree. If angle between two faces is > (180 degree - 2*bias angle): faces will not produce occlusion on each other.
	 * A value of 0 degree will produce maximum of occlusion. A value of 90 degrees won't produce occlusion.
	 * This bias angle allows to eliminate some unexpected artifacts especially when camera is near to a surface.
	 */
	void AmbientOcclusionManager::setBiasAngleInDegree(float biasAngleInDegree)
	{
		this->biasAngleInDegree = biasAngleInDegree;

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
		}else if(textureSize==AOTextureSize::HALF_SIZE)
		{
			return 2;
		}

		throw std::invalid_argument("Unknown texture size value: " + textureSize);
	}

	unsigned int AmbientOcclusionManager::getAmbientOcclusionTextureID() const
	{
		if(isBlurActivated)
		{
			 return horizontalBlurFilter->getTextureID();
		}

		return ambientOcclusionTexID;
	}

	void AmbientOcclusionManager::updateAOTexture(const Camera *const camera)
	{
		int activeFBO;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &activeFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalAndAmbientTexID);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, randomTexID);

		ShaderManager::instance()->bind(hbaoShader);
		glUniformMatrix4fv(mInverseViewProjectionLoc, 1, false, (const float*) (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse());

		glViewport(0, 0, textureSizeX, textureSizeY);

		quadDisplayer->display();

		if(isBlurActivated)
		{
			verticalBlurFilter->applyOn(ambientOcclusionTexID);
			horizontalBlurFilter->applyOn(verticalBlurFilter->getTextureID());
		}

		glViewport(0, 0, sceneWidth, sceneHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, activeFBO);
	}

	void AmbientOcclusionManager::loadAOTexture(unsigned int ambientOcclusionTextureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + ambientOcclusionTextureUnit);
		glBindTexture(GL_TEXTURE_2D, getAmbientOcclusionTextureID());

		glUniform1i(ambienOcclusionTexLoc, ambientOcclusionTextureUnit);
	}

}
