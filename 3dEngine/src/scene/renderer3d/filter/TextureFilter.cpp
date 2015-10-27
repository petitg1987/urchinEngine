#include <GL/gl.h>
#include <stdexcept>

#include "TextureFilter.h"
#include "utils/shader/ShaderManager.h"
#include "utils/shader/TokenReplacerShader.h"

namespace urchin
{

	TextureFilter::TextureFilter(TextureFilter::FilterType filterType, unsigned int textureWidth, unsigned int textureHeight) :
		isInitialized(false),
		textureWidth(textureWidth),
		textureHeight(textureHeight),
		numberLayer(1),
		bufferIDs(nullptr),
		vertexArrayObject(0),
		downSampleShader(0),
		texLoc(0),
		fboID(0),
		textureID(0)
	{

	}

	TextureFilter::~TextureFilter()
	{
		if(isInitialized)
		{
			if(vertexArrayObject!=0)
			{
				glDeleteVertexArrays(1, &vertexArrayObject);
			}
			if(bufferIDs!=nullptr)
			{
				glDeleteBuffers(2, bufferIDs);
				delete [] bufferIDs;
			}
			ShaderManager::instance()->removeProgram(downSampleShader);

			glDeleteFramebuffers(1, &fboID);
			glDeleteTextures(1, &textureID);
		}
	}

	void TextureFilter::setNumberLayer(unsigned int numberLayer)
	{
		if(isInitialized)
		{
			throw std::runtime_error("Impossible to change number of layer once the filter initialized.");
		}

		this->numberLayer = numberLayer;
	}

	void TextureFilter::initialize()
	{
		const int vertexArray[8] = {-1, 1, 1, 1, 1, -1,	-1, -1};
		const int stArray[8] = {0, 1, 1, 1, 1, 0, 0, 0};
		bufferIDs = new unsigned int[2];
		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 2, GL_INT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stArray), stArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_INT, false, 0, 0);

		std::map<TokenReplacerShader::ShaderToken, std::string> geometryTokens;
		geometryTokens[TokenReplacerShader::TOKEN0] = std::to_string(3*numberLayer);
		geometryTokens[TokenReplacerShader::TOKEN1] = std::to_string(numberLayer);

		downSampleShader = ShaderManager::instance()->createProgram("downSampleTex.vert", "downSampleTex.frag");
		ShaderManager::instance()->setGeometryShader(downSampleShader, "downSampleTex.geo", geometryTokens);
		ShaderManager::instance()->bind(downSampleShader);
		texLoc = glGetUniformLocation(downSampleShader, "tex");
		glActiveTexture(GL_TEXTURE0); //TODO remove ?
		glUniform1i(texLoc, 0);


		//TODO split initializeTexture() in two method...
		glGenFramebuffers(1, &fboID);
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		GLenum fragData[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, fragData);
		glReadBuffer(GL_NONE);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID); //TODO user should specify type: GL_TEXTURE_2D_ARRAY
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, textureWidth, textureHeight, numberLayer, 0, GL_RG, GL_FLOAT, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, fragData[0], textureID, 0);

		isInitialized = true;
	}

	unsigned int TextureFilter::getFboId() const
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Filter must be initialized before retrieve frame buffer ID.");
			}
		#endif

		return fboID;
	}

	unsigned int TextureFilter::getTextureID() const
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Filter must be initialized before retrieve texture ID.");
			}
		#endif

		return textureID;
	}

	void TextureFilter::applyOn(unsigned int sourceTextureId) const
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Filter must be initialized before apply it.");
			}
		#endif

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		ShaderManager::instance()->bind(downSampleShader);

		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, sourceTextureId);

		glUniform1i(texLoc, 0);

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}
