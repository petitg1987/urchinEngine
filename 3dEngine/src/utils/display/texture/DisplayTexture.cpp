#include <GL/gl.h>
#include <stdexcept>

#include "utils/display/texture/DisplayTexture.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{

	DisplayTexture::DisplayTexture(unsigned int textureID, DisplayTexture::ColorFactor colorFactor) :
			coordinateX(DisplayTexture::LEFT),
			coordinateY(DisplayTexture::BOTTOM),
			fullscreen(false),
			userMinX(0.0),
			userMaxX(1.0),
			userMinY(0.0),
			userMaxY(1.0),
			textureID(textureID),
			colorFactor(colorFactor),
			layer(-1)
	{
		initializeShader();
	}

	DisplayTexture::DisplayTexture(unsigned int textureID, unsigned int layer, DisplayTexture::ColorFactor colorFactor) :
			coordinateX(DisplayTexture::LEFT),
			coordinateY(DisplayTexture::BOTTOM),
			fullscreen(false),
			userMinX(0.0),
			userMaxX(1.0),
			userMinY(0.0),
			userMaxY(1.0),
			textureID(textureID),
			colorFactor(colorFactor),
			layer(layer)
	{
		initializeShader();
	}

	DisplayTexture::~DisplayTexture()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(2, bufferIDs);
		ShaderManager::instance()->removeProgram(displayTextureShader);
	}

	void DisplayTexture::initializeShader()
	{
		//determine color factor
		float colorFactorValue;
		switch(colorFactor)
		{
			case DisplayTexture::DEFAULT_FACTOR:
				colorFactorValue = 1.0;
				break;
			case DisplayTexture::DEPTH_FACTOR:
				colorFactorValue = 200.0;
				break;
			default:
				throw std::domain_error("Unsupported color factor");
				break;
		}

		const std::string &fragShaderName = (layer==-1) ? "displayTexture.frag" : "displayTextureArray.frag";
		displayTextureShader = ShaderManager::instance()->createProgram("displayTexture.vert", fragShaderName);
		unsigned int previousProgram = ShaderManager::instance()->getCurrentProgram();
		ShaderManager::instance()->bind(displayTextureShader);
		mProjectionLoc  = glGetUniformLocation(displayTextureShader, "mProjection");
		diffuseTexLoc = glGetUniformLocation(displayTextureShader, "colorTex");
		colorFactorLoc = glGetUniformLocation(displayTextureShader, "colorFactor");
		if(layer!= -1)
		{
			layerLoc = glGetUniformLocation(displayTextureShader, "layer");
			glUniform1i(layerLoc, layer);
		}

		int stArray[8] = {0, 1, 1, 1, 1, 0, 0, 0};
		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stArray), &stArray[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_INT, false, 0, 0);

		glUniform1f(colorFactorLoc, colorFactorValue);

		ShaderManager::instance()->bind(previousProgram);
	}

	void DisplayTexture::setPosition(DisplayTexture::CoordinateX coordinateX, DisplayTexture::CoordinateY coordinateY)
	{
		this->coordinateX = coordinateX;
		this->coordinateY = coordinateY;
	}

	/**
	 * @param minX Minimum X (0 : left screen)
	 * @param maxX Maximum X (scene width : right screen)
	 * @param minY Minimum Y (0 : top screen
	 * @param maxY Maximum Y (scene height : bottom screen)
	 */
	void DisplayTexture::setSize(float minX, float maxX, float minY, float maxY)
	{
		userMinX = minX;
		userMaxX = maxX;
		userMinY = minY;
		userMaxY = maxY;
	}

	void DisplayTexture::setFullscreen(bool fullscreen)
	{
		this->fullscreen = fullscreen;
	}

	void DisplayTexture::loadGeometry(int sceneWidth, int sceneHeight)
	{
		float minX, maxX, minY, maxY;
		if(fullscreen)
		{
			minX = 0.0;
			maxX = sceneWidth;
			minY = 0.0;
			maxY = sceneHeight;
		}else
		{
			switch(coordinateX)
			{
				case DisplayTexture::LEFT:
					minX = sceneWidth*0.025;
					maxX = sceneWidth*0.325;
					break;
				case DisplayTexture::CENTER_X:
					minX = sceneWidth*0.35;
					maxX = sceneWidth*0.65;
					break;
				case DisplayTexture::RIGHT:
					minX = sceneWidth*0.675;
					maxX = sceneWidth*0.975;
					break;
				case DisplayTexture::USER_DEFINED_X:
					minX = userMinX;
					maxX = userMaxX;
					break;
				default:
					throw std::domain_error("Unsupported coordinate X");
					break;
			}

			switch(coordinateY)
			{
				case DisplayTexture::BOTTOM:
					minY = sceneHeight*0.675;
					maxY = sceneHeight*0.975;
					break;
				case DisplayTexture::CENTER_Y:
					minY = sceneHeight*0.35;
					maxY = sceneHeight*0.65;
					break;
				case DisplayTexture::TOP:
					minY = sceneHeight*0.025;
					maxY = sceneHeight*0.325;
					break;
				case DisplayTexture::USER_DEFINED_Y:
					minY = userMinY;
					maxY = userMaxY;
					break;
				default:
					throw std::domain_error("Unsupported coordinate Y");
					break;
			}
		}

		//orthogonal matrix with origin at top left screen
		ShaderManager::instance()->bind(displayTextureShader);
		mProjection.setValues(2.0f/(float)sceneWidth, 0.0, -1.0,
				0.0, -2.0f/(float)sceneHeight, 1.0,
				0.0, 0.0, 1.0);
		glUniformMatrix3fv(mProjectionLoc, 1, false, (const float*)mProjection);

		//update the display
		float vertexArray[8] = {
				minX, minY,
				maxX, minY,
				maxX, maxY,
				minX, maxY
		};
		glBindVertexArray(vertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), &vertexArray[0], GL_STREAM_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 2, GL_FLOAT, false, 0, 0);
	}

	void DisplayTexture::display(int width, int height)
	{
		loadGeometry(width, height);

		unsigned int previousProgram = ShaderManager::instance()->getCurrentProgram();
		ShaderManager::instance()->bind(displayTextureShader);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glActiveTexture(GL_TEXTURE0);
		if(layer== -1)
		{
			glBindTexture(GL_TEXTURE_2D, textureID);
		}else
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
		}

		glUniform1i(diffuseTexLoc, 0);

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_QUADS, 0, 4);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		ShaderManager::instance()->bind(previousProgram);
	}

}
