#include <GL/glew.h>
#include <GL/gl.h>
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
			layer(layer),
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
		#ifdef _DEBUG
			if(isInitialized)
			{
				throw std::runtime_error("Impossible to change position of a texture displayer initialized.");
			}
		#endif

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
		#ifdef _DEBUG
			if(isInitialized)
			{
				throw std::runtime_error("Impossible to change size of a texture displayer initialized.");
			}
		#endif

		userMinX = minX;
		userMaxX = maxX;
		userMinY = minY;
		userMaxY = maxY;
	}

	void TextureDisplayer::setFullScreen(bool fullScreen)
	{
		#ifdef _DEBUG
			if(isInitialized)
			{
				throw std::runtime_error("Impossible to change full screen flag of a texture displayer initialized.");
			}
		#endif

		this->fullScreen = fullScreen;
	}

	void TextureDisplayer::initialize(int sceneWidth, int sceneHeight, float nearPlane, float farPlane)
	{
		#ifdef _DEBUG
			if(isInitialized)
			{
				throw std::runtime_error("Display texture is already initialized.");
			}
		#endif

		initializeShader(nearPlane, farPlane);

		float minX, maxX, minY, maxY;
		if(fullScreen)
		{
			minX = 0.0;
			maxX = sceneWidth;
			minY = 0.0;
			maxY = sceneHeight;
		}else
		{
			switch(coordinateX)
			{
				case TextureDisplayer::LEFT:
					minX = sceneWidth*0.025;
					maxX = sceneWidth*0.325;
					break;
				case TextureDisplayer::CENTER_X:
					minX = sceneWidth*0.35;
					maxX = sceneWidth*0.65;
					break;
				case TextureDisplayer::RIGHT:
					minX = sceneWidth*0.675;
					maxX = sceneWidth*0.975;
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
					minY = sceneHeight*0.675;
					maxY = sceneHeight*0.975;
					break;
				case TextureDisplayer::CENTER_Y:
					minY = sceneHeight*0.35;
					maxY = sceneHeight*0.65;
					break;
				case TextureDisplayer::TOP:
					minY = sceneHeight*0.025;
					maxY = sceneHeight*0.325;
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
		glUniformMatrix3fv(mProjectionLoc, 1, false, (const float*)mProjection);

		//update the display
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_FLOAT, new float[8]{minX, minY, maxX, minY, maxX, maxY, minX, maxY})
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
		displayTextureShader = ShaderManager::instance()->createProgram("displayTexture.vert", fragShaderName, textureDisplayTokens);

		ShaderManager::instance()->bind(displayTextureShader);
		unsigned int colorIntensityLoc = glGetUniformLocation(displayTextureShader, "colorIntensity");
		glUniform1f(colorIntensityLoc, colorIntensity);

		unsigned int cameraPlanesLoc = glGetUniformLocation(displayTextureShader, "cameraPlanes");
		float cameraPlanes[2] = {nearPlane, farPlane};
		glUniform1fv(cameraPlanesLoc, 2, cameraPlanes);

		unsigned int diffuseTexLoc = glGetUniformLocation(displayTextureShader, "colorTex");
		glUniform1i(diffuseTexLoc, GL_TEXTURE0-GL_TEXTURE0);

		if(layer!=-1)
		{
			unsigned int layerLoc = glGetUniformLocation(displayTextureShader, "layer");
			glUniform1i(layerLoc, layer);
		}

		mProjectionLoc  = glGetUniformLocation(displayTextureShader, "mProjection");
	}

	void TextureDisplayer::display()
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Display texture must be initialized before display.");
			}
		#endif

		ShaderManager::instance()->bind(displayTextureShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture((layer==-1) ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY, textureID);

		quadDisplayer->display();
	}

}
