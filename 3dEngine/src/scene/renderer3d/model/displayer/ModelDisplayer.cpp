#include <GL/glew.h>
#include <GL/gl.h>
#include <algorithm>
#include <stdexcept>

#include "ModelDisplayer.h"

namespace urchin
{
	/**
	 * @param displayMode Defines a display mode allowing to choose what we display and how we display models.
	 * The available display modes are:
	 *  - DEFAULT_MODE provide:
	 *      * Depth information
	 *      * Diffuse information (output number: 0)
	 *      * Normal information and ambient factor (output number: 1)
	 *  - DEPTH_ONLY_MODE provide:
	 *      * Depth information
	 */
	ModelDisplayer::ModelDisplayer(DisplayMode displayMode) :
		isInitialized(false),
		geometryShaderName(""),
		fragmentShaderName(""),
		displayMode(displayMode),
		modelShader(0),
		mProjectionLoc(0),
		mModelLoc(0),
		mViewLoc(0),
		mNormalLoc(0),
		ambientFactorLoc(0),
		customUniform(nullptr),
		customModelUniform(nullptr)
	{

	}

	ModelDisplayer::~ModelDisplayer()
	{
		ShaderManager::instance()->removeProgram(modelShader);
	}

	void ModelDisplayer::initialize()
	{
		if(isInitialized)
		{
			throw std::runtime_error("Model displayer is already initialized.");
		}

		if(displayMode==DEFAULT_MODE)
		{
			//shader creation
			std::string vertexShaderName = "model.vert";
			if(fragmentShaderName.compare("")==0)
			{ //use default fragment shader
				fragmentShaderName = "model.frag";
			}
			if(geometryShaderName.compare("")==0)
			{ //use default geometry shader
				geometryShaderName = "";
			}
			createShader(vertexShaderName, geometryShaderName, fragmentShaderName);

			mNormalLoc = glGetUniformLocation(modelShader, "mNormal");
			ambientFactorLoc = glGetUniformLocation(modelShader, "ambientFactor");
			int diffuseTexLoc = glGetUniformLocation(modelShader, "diffuseTex");
			int normalTexLoc = glGetUniformLocation(modelShader, "normalTex");

			//activate texture
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(diffuseTexLoc, 0);
			glActiveTexture(GL_TEXTURE1);
			glUniform1i(normalTexLoc, 1);

			//setup mesh parameters
			meshParameter.setDiffuseTextureUnit(GL_TEXTURE0);
			meshParameter.setNormalTextureUnit(GL_TEXTURE1);
			meshParameter.setAmbientFactorLoc(ambientFactorLoc);
		}else if(displayMode==DEPTH_ONLY_MODE)
		{
			//shader creation
			std::string vertexShaderName = "modelDepthOnly.vert";
			if(fragmentShaderName.compare("")==0)
			{ //use default fragment shader
				fragmentShaderName = "modelDepthOnly.frag";
			}
			if(geometryShaderName.compare("")==0)
			{ //use default geometry shader
				geometryShaderName = "";
			}
			createShader(vertexShaderName, geometryShaderName, fragmentShaderName);

			mNormalLoc = 0;
			ambientFactorLoc = 0;

			//setup mesh parameters
			meshParameter.setDiffuseTextureUnit(-1);
			meshParameter.setNormalTextureUnit(-1);
			meshParameter.setAmbientFactorLoc(-1);
		}else
		{
			throw std::invalid_argument("Unknown display mode.");
		}

		//default matrix
		projectionMatrix = Matrix4<float>();
		ShaderManager::instance()->bind(modelShader);
		glUniformMatrix4fv(mProjectionLoc, 1, false, (const float*)projectionMatrix);

		isInitialized=true;
	}

	void ModelDisplayer::createShader(const std::string &vertexShaderName, const std::string &geometryShaderName, const std::string &fragmentShaderName)
	{
		modelShader = ShaderManager::instance()->createProgram(vertexShaderName, fragmentShaderName, fragmentTokens);
		if(geometryShaderName.compare("")!=0)
		{
			ShaderManager::instance()->setGeometryShader(modelShader, geometryShaderName, geometryTokens);
		}
		ShaderManager::instance()->bind(modelShader);

		mProjectionLoc = glGetUniformLocation(modelShader, "mProjection");
		mModelLoc = glGetUniformLocation(modelShader, "mModel");
		mViewLoc = glGetUniformLocation(modelShader, "mView");
	}

	void ModelDisplayer::onCameraProjectionUpdate(const Camera *const camera)
	{
		this->projectionMatrix = camera->getProjectionMatrix();

		ShaderManager::instance()->bind(modelShader);
		glUniformMatrix4fv(mProjectionLoc, 1, false, (const float*)projectionMatrix);
	}

	int ModelDisplayer::getUniformLocation(const std::string &name)
	{
		return glGetUniformLocation(modelShader, name.c_str());
	}

	void ModelDisplayer::setCustomGeometryShader(const std::string &geometryShaderName, const std::map<TokenReplacerShader::ShaderToken, std::string> &geometryTokens)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to set custom geometry shader once the model displayer initialized.");
		}

		this->geometryShaderName = geometryShaderName;
		this->geometryTokens = geometryTokens;
	}

	void ModelDisplayer::setCustomFragmentShader(const std::string &fragmentShaderName, const std::map<TokenReplacerShader::ShaderToken, std::string> &fragmentTokens)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to set custom fragment shader once the model displayer initialized.");
		}

		this->fragmentShaderName = fragmentShaderName;
		this->fragmentTokens = fragmentTokens;
	}

	void ModelDisplayer::setCustomUniform(CustomUniform *customUniform)
	{
		this->customUniform = customUniform;
	}

	void ModelDisplayer::setCustomModelUniform(CustomModelUniform *customModelUniform)
	{
		this->customModelUniform = customModelUniform;
	}

	void ModelDisplayer::setModels(const std::set<Model *> &models)
	{
		this->models = models;
	}

	void ModelDisplayer::updateAnimation(float invFrameRate)
	{
		for(std::set<Model *>::iterator it=models.begin();it!=models.end();++it)
		{
			(*it)->updateAnimation(invFrameRate);
		}
	}

	void ModelDisplayer::display(const Matrix4<float> &viewMatrix)
	{
		if(!isInitialized)
		{
			throw std::runtime_error("Model displayer must be initialized before displayed.");
		}

		ShaderManager::instance()->bind(modelShader);
		glUniformMatrix4fv(mViewLoc, 1, false, (const float*)viewMatrix);
		if(customUniform!=nullptr)
		{
			customUniform->loadCustomUniforms();
		}

		for(std::set<Model *>::iterator it=models.begin();it!=models.end();++it)
		{
			glUniformMatrix4fv(mModelLoc, 1, false, (const float*)(*it)->getTransform().getTransformMatrix());
			if(displayMode==DEFAULT_MODE)
			{
				glUniformMatrix3fv(mNormalLoc, 1, true, (const float*)(*it)->getTransform().getTransformMatrix().toMatrix3().inverse());
			}
			if(customModelUniform!=nullptr)
			{
				customModelUniform->loadCustomUniforms(*it);
			}

			(*it)->display(meshParameter);
		}
	}

#ifdef _DEBUG
	void ModelDisplayer::drawBBox(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const
	{
		for(std::set<Model *>::iterator it=models.begin();it!=models.end();++it)
		{
			(*it)->drawBBox(projectionMatrix, viewMatrix);
		}
	}
#endif

}
