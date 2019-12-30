#include <GL/glew.h>
#include <stdexcept>

#include "utils/display/texture/TextureDisplayer.h"
#include "utils/shader/ShaderManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	TextureDisplayer::TextureDisplayer(unsigned int textureID, TextureDisplayer::ColorType colorType, float colorIntensity) :
			isInitialized(false),
			coordinateX(TextureDisplayer::LEFT),
			coordinateY(TextureDisplayer::BOTTOM),
			fullScreen(false),
			userMinX(0.0),
			userMaxX(1.0),
			userMinY(0.0),
			userMaxY(1.0),
			textureID(textureID),
			colorType(colorType),
			colorIntensity(colorIntensity),
			displayTextureShader(0),
			layer(-1),
			mProjectionLoc(-1),
			diffuseTexLoc(-1)
	{

	}

	TextureDisplayer::TextureDisplayer(unsigned int textureID, unsigned int layer, TextureDisplayer::ColorType colorType, float colorIntensity) :
			isInitialized(false),
			coordinateX(TextureDisplayer::LEFT),
			coordinateY(TextureDisplayer::BOTTOM),
			fullScreen(false),
			userMinX(0.0),
			userMaxX(1.0),
			userMinY(0.0),
			userMaxY(1.0),
			textureID(textureID),
			colorType(colorType),
			colorIntensity(colorIntensity),
			displayTextureShader(0),
			layer((int)layer),
			mProjectionLoc(-1),
			diffuseTexLoc(-1)
	{

	}

	TextureDisplayer::~TextureDisplayer()
	{
		ShaderManager::instance()->removeProgram(displayTextureShader);
	}

	void TextureDisplayer::setPosition(TextureDisplayer::CoordinateX coordinateX, TextureDisplayer::CoordinateY coordinateY)
	{
	    if(isInitialized)
        {
	        throw std::runtime_error("No position update allowed after initialization.");
        }

		this->coordinateX = coordinateX;
		this->coordinateY = coordinateY;
	}

	/**
	 * @param minX Minimum X (0 : left screen)
	 * @param maxX Maximum X (scene width : right screen)
	 * @param minY Minimum Y (0 : top screen
	 * @param maxY Maximum Y (scene height : bottom screen)
	 */
	void TextureDisplayer::setSize(float minX, float maxX, float minY, float maxY)
	{
        if(isInitialized)
        {
            throw std::runtime_error("No size update allowed after initialization.");
        }

		userMinX = minX;
		userMaxX = maxX;
		userMinY = minY;
		userMaxY = maxY;
	}

	void TextureDisplayer::setFullScreen(bool fullScreen)
	{
        if(isInitialized)
        {
            throw std::runtime_error("No full screen flag update allowed after initialization.");
        }

		this->fullScreen = fullScreen;
	}

	void TextureDisplayer::initialize(unsigned int sceneWidth, unsigned int sceneHeight, float nearPlane, float farPlane)
	{
        if(isInitialized)
        {
            throw std::runtime_error("Texture displayer cannot be initialized twice.");
        }

		initializeShader(nearPlane, farPlane);

		float minX, maxX, minY, maxY;
		if(fullScreen)
		{
			minX = 0.0f;
			maxX = (float)sceneWidth;
			minY = 0.0f;
			maxY = (float)sceneHeight;
		}else
		{
			switch(coordinateX)
			{
				case TextureDisplayer::LEFT:
					minX = (float)sceneWidth * 0.025f;
					maxX = (float)sceneWidth * 0.325f;
					break;
				case TextureDisplayer::CENTER_X:
					minX = (float)sceneWidth * 0.35f;
					maxX = (float)sceneWidth * 0.65f;
					break;
				case TextureDisplayer::RIGHT:
					minX = (float)sceneWidth * 0.675f;
					maxX = (float)sceneWidth * 0.975f;
					break;
				case TextureDisplayer::USER_DEFINED_X:
					minX = userMinX;
					maxX = userMaxX;
					break;
				default:
					throw std::domain_error("Unsupported coordinate X");
			}

			switch(coordinateY)
			{
				case TextureDisplayer::BOTTOM:
					minY = (float)sceneHeight * 0.675f;
					maxY = (float)sceneHeight * 0.975f;
					break;
				case TextureDisplayer::CENTER_Y:
					minY = (float)sceneHeight * 0.35f;
					maxY = (float)sceneHeight * 0.65f;
					break;
				case TextureDisplayer::TOP:
					minY = (float)sceneHeight * 0.025f;
					maxY = (float)sceneHeight * 0.325f;
					break;
				case TextureDisplayer::USER_DEFINED_Y:
					minY = userMinY;
					maxY = userMaxY;
					break;
				default:
					throw std::domain_error("Unsupported coordinate Y");
			}
		}

		//orthogonal matrix with origin at top left screen
		ShaderManager::instance()->bind(displayTextureShader);
		mProjection.setValues(2.0f/(float)sceneWidth, 0.0, -1.0,
				0.0, -2.0f/(float)sceneHeight, 1.0,
				0.0, 0.0, 1.0);
		glUniformMatrix3fv(mProjectionLoc, 1, GL_FALSE, (const float*)mProjection);

		//update the display
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_FLOAT, new float[8]{minX, minY, maxX, minY, maxX, maxY, minX, maxY}, true)
				->build();

		isInitialized = true;
	}

	void TextureDisplayer::initializeShader(float nearPlane, float farPlane)
	{
		std::map<std::string, std::string> textureDisplayTokens;
		textureDisplayTokens["IS_DEFAULT_VALUE"] = colorType==ColorType::DEFAULT_VALUE ? "true" : "false";
		textureDisplayTokens["IS_DEPTH_VALUE"] = colorType==ColorType::DEPTH_VALUE ? "true" : "false";
		textureDisplayTokens["IS_GRAYSCALE_VALUE"] = colorType==ColorType::GRAYSCALE_VALUE ? "true" : "false";
		textureDisplayTokens["IS_INVERSE_GRAYSCALE_VALUE"] = colorType==ColorType::INVERSE_GRAYSCALE_VALUE ? "true" : "false";

		const std::string &fragShaderName = (layer==-1) ? "displayTexture.frag" : "displayTextureArray.frag";
		displayTextureShader = ShaderManager::instance()->createProgram("displayTexture.vert", "", fragShaderName, textureDisplayTokens);

		ShaderManager::instance()->bind(displayTextureShader);
		auto colorIntensityLoc = glGetUniformLocation(displayTextureShader, "colorIntensity");
		glUniform1f(colorIntensityLoc, colorIntensity);

		auto cameraPlanesLoc = glGetUniformLocation(displayTextureShader, "cameraPlanes");
		float cameraPlanes[2] = {nearPlane, farPlane};
		glUniform1fv(cameraPlanesLoc, 2, cameraPlanes);

		auto diffuseTexLoc = glGetUniformLocation(displayTextureShader, "colorTex");
		glUniform1i(diffuseTexLoc, GL_TEXTURE0-GL_TEXTURE0);

		if(layer!=-1)
		{
			auto layerLoc = glGetUniformLocation(displayTextureShader, "layer");
			glUniform1i(layerLoc, layer);
		}

		mProjectionLoc  = glGetUniformLocation(displayTextureShader, "mProjection");
	}

	void TextureDisplayer::display()
	{
        if(!isInitialized)
        {
            throw std::runtime_error("Texture displayer must be initialized before display.");
        }

		ShaderManager::instance()->bind(displayTextureShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture((layer==-1) ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY, textureID);

		quadDisplayer->display();
	}

}
