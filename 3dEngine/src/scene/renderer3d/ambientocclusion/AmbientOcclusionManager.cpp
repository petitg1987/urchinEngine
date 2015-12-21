#include <GL/gl.h>
#include <map>
#include <string>
#include <cmath>

#include "AmbientOcclusionManager.h"
#include "utils/shader/ShaderManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

#define DEFAULT_NUM_DIRECTIONS 6
#define DEFAULT_NUM_STEPS 3
#define DEFAULT_RADIUS 0.2

namespace urchin
{

	AmbientOcclusionManager::AmbientOcclusionManager() :
		isInitialized(false),
		sceneWidth(0),
		sceneHeight(0),
		numDirections(DEFAULT_NUM_DIRECTIONS),
		numSteps(DEFAULT_NUM_STEPS),
		radius(DEFAULT_RADIUS),
		fboID(0),
		ambientOcclusionTexID(0),
		hbaoShader(0),
		mInverseViewProjectionLoc(0),
		cameraPlanesLoc(0),
		invResolutionLoc(0),
		nearPlaneScreenRadiusLoc(0),
		depthTexID(0),
		normalAndAmbientTexID(0),
		ambienOcclusionTexLoc(0)
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
		this->depthTexID = depthTexID;
		this->normalAndAmbientTexID = normalAndAmbientTexID;
		quadDisplayer = std::make_shared<QuadDisplayerBuilder>()->build();

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

		createOrUpdateTexture();

		ShaderManager::instance()->bind(hbaoShader);
		Vector2<float> invResolution(1.0f/sceneWidth, 1.0f/sceneHeight);
		glUniform2fv(invResolutionLoc, 1, invResolution);
	}

	void AmbientOcclusionManager::onCameraProjectionUpdate(const Camera *const camera)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Ambient occlusion manager must be initialized.");
		}

		ShaderManager::instance()->bind(hbaoShader);

		float nearPlaneScreenRadiusInPixel = radius * camera->getProjectionMatrix()(1, 1);
		glUniform1f(nearPlaneScreenRadiusLoc, nearPlaneScreenRadiusInPixel);

		float cameraPlanes[2] = {camera->getNearPlane(), camera->getFarPlane()};
		glUniform1fv(cameraPlanesLoc, 2, cameraPlanes);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, sceneWidth, sceneHeight, 0, GL_RED, GL_FLOAT, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, fboAttachments[0], ambientOcclusionTexID, 0);
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

	unsigned int AmbientOcclusionManager::getAmbientOcclusionTextureID() const
	{
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

		ShaderManager::instance()->bind(hbaoShader);
		glUniformMatrix4fv(mInverseViewProjectionLoc, 1, false, (const float*) (camera->getProjectionMatrix() *camera->getViewMatrix()).inverse());

		quadDisplayer->display();

		glBindFramebuffer(GL_FRAMEBUFFER, activeFBO);
	}

	void AmbientOcclusionManager::loadAOTexture(unsigned int ambientOcclusionTextureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + ambientOcclusionTextureUnit);
		glBindTexture(GL_TEXTURE_2D, ambientOcclusionTexID);

		glUniform1i(ambienOcclusionTexLoc, ambientOcclusionTextureUnit);
	}

}
