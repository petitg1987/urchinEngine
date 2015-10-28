#include <GL/gl.h>
#include <stdexcept>

#include "TextureFilter.h"
#include "TextureFilterBuilder.h"
#include "utils/shader/ShaderManager.h"
#include "utils/shader/TokenReplacerShader.h"

namespace urchin
{

	TextureFilter::TextureFilter(TextureFilterBuilder *textureFilterBuilder):
		textureWidth(),
		textureHeight(),
		textureType(),
		textureNumberLayer(),
		textureInternalFormat(),
		textureFormat(),

		bufferIDs(nullptr),
		vertexArrayObject(0),
		downSampleShader(0),
		texLoc(0),
		fboID(0),
		textureID(0)
	{
		initializeDisplay();
		initializeTexture();
	}

	TextureFilter::~TextureFilter()
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

	void TextureFilter::initializeDisplay()
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
		geometryTokens[TokenReplacerShader::TOKEN0] = std::to_string(3*textureNumberLayer);
		geometryTokens[TokenReplacerShader::TOKEN1] = std::to_string(textureNumberLayer);

		downSampleShader = ShaderManager::instance()->createProgram("downSampleTex.vert", "downSampleTex.frag");
		ShaderManager::instance()->setGeometryShader(downSampleShader, "downSampleTex.geo", geometryTokens);
		ShaderManager::instance()->bind(downSampleShader);
		texLoc = glGetUniformLocation(downSampleShader, "tex");
		glUniform1i(texLoc, 0);
	}

	void TextureFilter::initializeTexture()
	{
		glGenFramebuffers(1, &fboID);
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		GLenum fragData[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, fragData);
		glReadBuffer(GL_NONE);

		glGenTextures(1, &textureID);
		glBindTexture(textureType, textureID);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if(textureType==GL_TEXTURE_2D_ARRAY)
		{
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, textureInternalFormat, textureWidth, textureHeight, textureNumberLayer, 0, textureFormat, GL_FLOAT, 0);
		}else if(textureType==GL_TEXTURE_2D)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, textureInternalFormat, textureWidth, textureHeight, 0, textureFormat, GL_FLOAT, 0);
		}else
		{
			throw std::invalid_argument("Unsupported texture type for filter: " + textureType);
		}
		glFramebufferTexture(GL_FRAMEBUFFER, fragData[0], textureID, 0);
	}

	unsigned int TextureFilter::getFboId() const
	{
		return fboID;
	}

	unsigned int TextureFilter::getTextureID() const
	{
		return textureID;
	}

	void TextureFilter::applyOn(unsigned int sourceTextureId) const
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		ShaderManager::instance()->bind(downSampleShader);

		glViewport(0, 0, textureWidth, textureHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(textureType, sourceTextureId);

		glUniform1i(texLoc, 0);

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindTexture(textureType, 0);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}
