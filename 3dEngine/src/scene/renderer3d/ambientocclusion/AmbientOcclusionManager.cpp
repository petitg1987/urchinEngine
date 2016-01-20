#include <GL/gl.h>
#include <map>
#include <string>
#include <cmath>

#include "AmbientOcclusionManager.h"
#include "utils/shader/ShaderManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"
#include "utils/filter/bilateralblur/BilateralBlurFilterBuilder.h"

#define DEFAULT_TEXTURE_SIZE 1 //=AOTextureSize::HALF_SIZE
#define DEFAULT_NUM_DIRECTIONS 8
#define DEFAULT_NUM_STEPS 4
#define DEFAULT_RADIUS 0.3
#define DEFAULT_BIAS_ANGLE_IN_DEGREE 25.0
#define DEFAULT_BLUR_SIZE 5
#define DEFAULT_BLUR_SHARPNESS 40.0

namespace urchin
{

	AmbientOcclusionManager::AmbientOcclusionManager() :
		isInitialized(false),
		sceneWidth(0),
		sceneHeight(0),
		nearPlane(0.0f),
		farPlane(0.0f),

		textureSize((AOTextureSize)DEFAULT_TEXTURE_SIZE),
		textureSizeX(1),
		textureSizeY(1),
		numDirections(DEFAULT_NUM_DIRECTIONS),
		numSteps(DEFAULT_NUM_STEPS),
		radius(DEFAULT_RADIUS),
		biasAngleInDegree(DEFAULT_BIAS_ANGLE_IN_DEGREE),
		blurSize(DEFAULT_BLUR_SIZE),
		blurSharpness(DEFAULT_BLUR_SHARPNESS),

		fboID(0),
		ambientOcclusionTexID(0),

		hbaoShader(0),
		mInverseViewProjectionLoc(0),
		cameraPlanesLoc(0),
		invResolutionLoc(0),
		nearPlaneScreenRadiusLoc(0),

		depthTexID(0),
		normalAndAmbientTexID(0),
		ambienOcclusionTexLoc(0),
		verticalBlurFilter(nullptr),
		horizontalBlurFilter(nullptr)
	{

	}

	AmbientOcclusionManager::~AmbientOcclusionManager()
	{
		glDeleteFramebuffers(1, &fboID);
		glDeleteTextures(1, &ambientOcclusionTexID);
	}

	/**
	 * @param shaderId Shader which use ambient occlusion texture
	 * @param depthTexID Depth texture of the scene
	 */
	void AmbientOcclusionManager::initialize(unsigned int shaderID, unsigned int depthTexID, unsigned int normalAndAmbientTexID)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Ambient occlusion manager is already initialized.");
		}

		//frame buffer object
		glGenFramebuffers(1, &fboID);
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		createOrUpdateTexture();

		//ambient occlusion shader
		std::map<std::string, std::string> hbaoTokens;
		hbaoTokens["NUM_DIRECTIONS"] = std::to_string(numDirections);
		hbaoTokens["NUM_STEPS"] = std::to_string(numSteps);
		hbaoTokens["RADIUS"] = std::to_string(radius);
		hbaoTokens["BIAS_ANGLE"] = std::to_string(std::cos((90.0f-biasAngleInDegree) / (180.0f/M_PI)));
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

		//visual data
		ShaderManager::instance()->bind(shaderID);
		ambienOcclusionTexLoc = glGetUniformLocation(shaderID, "ambientOcclusionTex");
		quadDisplayer = std::make_shared<QuadDisplayerBuilder>()->build();

		this->depthTexID = depthTexID;
		glBindTexture(GL_TEXTURE_2D, depthTexID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->normalAndAmbientTexID = normalAndAmbientTexID;
		glBindTexture(GL_TEXTURE_2D, normalAndAmbientTexID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		isInitialized=true;
	}

	void AmbientOcclusionManager::onResize(int width, int height)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Ambient occlusion manager must be initialized.");
		}

		sceneWidth = width;
		sceneHeight = height;
		
		computeTextureSize();
		createOrUpdateTexture();

		ShaderManager::instance()->bind(hbaoShader);
		Vector2<float> invResolution(1.0f/sceneWidth, 1.0f/sceneHeight);
		glUniform2fv(invResolutionLoc, 1, invResolution);
	}

	void AmbientOcclusionManager::createOrUpdateTexture()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		GLenum fboAttachments[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, fboAttachments);
		glReadBuffer(GL_NONE);

		glDeleteTextures(1, &ambientOcclusionTexID);
		glGenTextures(1, &ambientOcclusionTexID);

		glBindTexture(GL_TEXTURE_2D, ambientOcclusionTexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, textureSizeX, textureSizeY, 0, GL_RED, GL_FLOAT, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, fboAttachments[0], ambientOcclusionTexID, 0);

		verticalBlurFilter = std::make_shared<BilateralBlurFilterBuilder>()
				->textureSize(textureSizeX, textureSizeY)
				->textureType(GL_TEXTURE_2D)
				->textureInternalFormat(GL_R16F)
				->textureFormat(GL_RED)
				->blurDirection(BilateralBlurFilterBuilder::VERTICAL_BLUR)
				->blurSize(blurSize)
				->blurSharpness(blurSharpness)
				->depthTextureID(depthTexID)
				->buildBilateralBlur();

		horizontalBlurFilter = std::make_shared<BilateralBlurFilterBuilder>()
				->textureSize(textureSizeX, textureSizeY)
				->textureType(GL_TEXTURE_2D)
				->textureInternalFormat(GL_R16F)
				->textureFormat(GL_RED)
				->blurDirection(BilateralBlurFilterBuilder::HORIZONTAL_BLUR)
				->blurSize(blurSize)
				->blurSharpness(blurSharpness)
				->depthTextureID(depthTexID)
				->buildBilateralBlur();

		verticalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
		horizontalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
	}

	void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera *const camera)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Ambient occlusion manager must be initialized.");
		}

		nearPlane = camera->getNearPlane();
		farPlane = camera->getFarPlane();

		ShaderManager::instance()->bind(hbaoShader);

		float nearPlaneScreenRadiusInPixel = radius * camera->getProjectionMatrix()(1, 1);
		glUniform1f(nearPlaneScreenRadiusLoc, nearPlaneScreenRadiusInPixel);

		float cameraPlanes[2] = {nearPlane, farPlane};
		glUniform1fv(cameraPlanesLoc, 2, cameraPlanes);

		verticalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
		horizontalBlurFilter->onCameraProjectionUpdate(nearPlane, farPlane);
	}

	void AmbientOcclusionManager::setTextureSize(AOTextureSize textureSize)
	{
		this->textureSize = textureSize;
		computeTextureSize();
	}

	void AmbientOcclusionManager::setNumDirections(unsigned int numDirections)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to change number of directions once the scene initialized.");
		}

		this->numDirections = numDirections;
	}

	void AmbientOcclusionManager::setNumSteps(unsigned int numSteps)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to change number of steps once the scene initialized.");
		}

		this->numSteps = numSteps;
	}

	/**
	 * @param radius Scope radius in units.
	 * Example: if 1.0f represents one meter, a radius of 0.5f will represent of radius of 50 centimeters.
	 */
	void AmbientOcclusionManager::setRadius(float radius)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to change radius once the scene initialized.");
		}

		this->radius = radius;
	}

	/**
	 * @param biasAngle Bias angle in degree. If angle between two faces is below the bias angle: faces will not produce occlusion on each other.
	 * A value of 0 degree will produce maximum of occlusion. A value of 90 degrees won't produce occlusion.
	 * This bias angle allows to eliminate some unexpected artifacts.
	 */
	void AmbientOcclusionManager::setBiasAngleInDegree(float biasAngleInDegree)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to change bias angle once the scene initialized.");
		}

		this->biasAngleInDegree = biasAngleInDegree;
	}

	void AmbientOcclusionManager::setBlurSize(unsigned int blurSize)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to change blur size once the scene initialized.");
		}

		this->blurSize = blurSize;
	}

	void AmbientOcclusionManager::setBlurSharpness(float blurSharpness)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to change blur sharpness once the scene initialized.");
		}

		this->blurSharpness = blurSharpness;
	}

	void AmbientOcclusionManager::computeTextureSize()
	{
		if(textureSize==AOTextureSize::FULL_SIZE)
		{
			textureSizeX = sceneWidth;
			textureSizeY = sceneHeight;
		}else if(textureSize==AOTextureSize::HALF_SIZE)
		{
			textureSizeX = sceneWidth / 2;
			textureSizeY = sceneHeight / 2;
		}else
		{
			throw std::invalid_argument("Unknown texture size value: " + textureSize);
		}
	}

	unsigned int AmbientOcclusionManager::getAmbientOcclusionTextureID() const
	{
		return horizontalBlurFilter->getTextureID();
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

		ShaderManager::instance()->bind(hbaoShader);
		glUniformMatrix4fv(mInverseViewProjectionLoc, 1, false, (const float*) (camera->getProjectionMatrix() *camera->getViewMatrix()).inverse());

		glViewport(0, 0, textureSizeX, textureSizeY);

		quadDisplayer->display();

		verticalBlurFilter->applyOn(ambientOcclusionTexID);
		horizontalBlurFilter->applyOn(verticalBlurFilter->getTextureID());

		glViewport(0, 0, sceneWidth, sceneHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, activeFBO);
	}

	void AmbientOcclusionManager::loadAOTexture(unsigned int ambientOcclusionTextureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + ambientOcclusionTextureUnit);
		glBindTexture(GL_TEXTURE_2D, horizontalBlurFilter->getTextureID());

		glUniform1i(ambienOcclusionTexLoc, ambientOcclusionTextureUnit);
	}

}
