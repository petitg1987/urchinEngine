#include <GL/gl.h>

#include "AmbientOcclusionManager.h"
#include "utils/shader/ShaderManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	AmbientOcclusionManager::AmbientOcclusionManager() :
		isInitialized(false),
		sceneWidth(0),
		sceneHeight(0),
		fboID(0),
		ambientOcclusionTexID(0),
		hbaoShader(0),
		mInverseViewProjectionLoc(0),
		depthTexID(0),
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
	void AmbientOcclusionManager::initialize(unsigned int shaderID, unsigned int depthTexID)
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
		hbaoShader = ShaderManager::instance()->createProgram("hbao.vert", "hbao.frag");
		ShaderManager::instance()->bind(hbaoShader);
		mInverseViewProjectionLoc = glGetUniformLocation(hbaoShader, "mInverseViewProjection");
		int depthTexLoc = glGetUniformLocation(hbaoShader, "depthTex");
		glUniform1i(depthTexLoc, GL_TEXTURE0-GL_TEXTURE0);

		//visual data
		ShaderManager::instance()->bind(shaderID);
		ambienOcclusionTexLoc = glGetUniformLocation(shaderID, "ambientOcclusionTex");
		this->depthTexID = depthTexID;
		quadDisplayer = std::make_shared<QuadDisplayerBuilder>()->build();

		isInitialized=true;
	}

	void AmbientOcclusionManager::onResize(int width, int height)
	{
		sceneWidth = width;
		sceneHeight = height;

		createOrUpdateTexture();
	}

	void AmbientOcclusionManager::createOrUpdateTexture()
	{
		if(isInitialized)
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, sceneWidth, sceneHeight, 0, GL_RED, GL_FLOAT, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, fboAttachments[0], ambientOcclusionTexID, 0);
		}
	}

	unsigned int AmbientOcclusionManager::getAmbientOcclusionTextureID() const
	{
		return ambientOcclusionTexID;
	}

	void AmbientOcclusionManager::updateAOTexture(const Camera *const camera)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexID);

		ShaderManager::instance()->bind(hbaoShader);
		glUniformMatrix4fv(mInverseViewProjectionLoc, 1, false, (const float*) (camera->getProjectionMatrix() * camera->getViewMatrix()).inverse());

		quadDisplayer->display();
	}

	void AmbientOcclusionManager::loadAOTexture(unsigned int ambientOcclusionTextureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + ambientOcclusionTextureUnit);
		glBindTexture(GL_TEXTURE_2D, ambientOcclusionTexID);

		glUniform1i(ambienOcclusionTexLoc, ambientOcclusionTextureUnit);
	}

}
