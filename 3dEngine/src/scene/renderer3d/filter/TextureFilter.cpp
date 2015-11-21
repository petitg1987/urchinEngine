#include <GL/gl.h>
#include <stdexcept>

#include "TextureFilter.h"
#include "TextureFilterBuilder.h"
#include "utils/shader/ShaderManager.h"
#include "utils/shader/TokenReplacerShader.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	TextureFilter::TextureFilter(const TextureFilterBuilder *textureFilterBuilder):
		isInitialized(false),
		textureWidth(textureFilterBuilder->getTextureWidth()),
		textureHeight(textureFilterBuilder->getTextureHeight()),
		textureType(textureFilterBuilder->getTextureType()),
		textureAccessFilter(textureFilterBuilder->getTextureAccessFilter()),
		textureAnisotropy(textureFilterBuilder->getTextureAnisotropy()),
		textureNumberLayer(textureFilterBuilder->getTextureNumberLayer()),
		textureInternalFormat(textureFilterBuilder->getTextureInternalFormat()),
		textureFormat(textureFilterBuilder->getTextureFormat()),

		downSampleShader(0),
		texLoc(0),
		fboID(0),
		textureID(0)
	{

	}

	TextureFilter::~TextureFilter()
	{
		ShaderManager::instance()->removeProgram(downSampleShader);

		glDeleteFramebuffers(1, &fboID);
		glDeleteTextures(1, &textureID);
	}

	void TextureFilter::initialize()
	{
		#ifdef _DEBUG
			if(isInitialized)
			{
				throw std::runtime_error("Texture filter is already initialized.");
			}
		#endif

		initializeDisplay();
		initializeTexture();

		isInitialized = true;
	}

	void TextureFilter::initializeDisplay()
	{
		texQuadDisplayer = std::make_shared<QuadDisplayerBuilder>()
				->build();

		std::map<std::string, std::string> shaderTokens;
		if(textureFormat==GL_RGB)
		{
			shaderTokens["OUTPUT_TYPE"] = "vec3";
			shaderTokens["SOURCE_TEX_COMPONENTS"] = "rgb";
		}else if(textureFormat==GL_RG)
		{
			shaderTokens["OUTPUT_TYPE"] = "vec2";
			shaderTokens["SOURCE_TEX_COMPONENTS"] = "rg";
		}else
		{
			throw std::invalid_argument("Unsupported texture format for filter: " + textureFormat);
		}
		this->completeShaderTokens(shaderTokens);

		if(textureType==GL_TEXTURE_2D_ARRAY)
		{
			std::map<std::string, std::string> geometryTokens;
			geometryTokens["MAX_VERTICES"] = std::to_string(3*textureNumberLayer);
			geometryTokens["NUMBER_LAYER"] = std::to_string(textureNumberLayer);

			downSampleShader = ShaderManager::instance()->createProgram("textureFilter.vert", getShaderName()+"Array.frag", shaderTokens);
			ShaderManager::instance()->setGeometryShader(downSampleShader, "textureFilter.geo", geometryTokens);
		}else
		{
			downSampleShader = ShaderManager::instance()->createProgram("textureFilter.vert", getShaderName()+".frag", shaderTokens);
		}

		ShaderManager::instance()->bind(downSampleShader);
		texLoc = glGetUniformLocation(downSampleShader, "tex");
		glUniform1i(texLoc, 0);
	}

	void TextureFilter::initializeTexture()
	{
		glGenFramebuffers(1, &fboID);
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		GLenum fboAttachments[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, fboAttachments);
		glReadBuffer(GL_NONE);

		glGenTextures(1, &textureID);
		glBindTexture(textureType, textureID);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, textureAccessFilter);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, textureAccessFilter);
		glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureAnisotropy);
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
		glFramebufferTexture(GL_FRAMEBUFFER, fboAttachments[0], textureID, 0);
	}

	unsigned int TextureFilter::getFboId() const
	{
		return fboID;
	}

	unsigned int TextureFilter::getTextureID() const
	{
		return textureID;
	}

	unsigned int TextureFilter::getTextureWidth() const
	{
		return textureWidth;
	}

	unsigned int TextureFilter::getTextureHeight() const
	{
		return textureHeight;
	}

	void TextureFilter::applyOn(unsigned int sourceTextureId) const
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Texture filter must be initialized before display.");
			}
		#endif

		ShaderManager::instance()->bind(downSampleShader);

		glViewport(0, 0, textureWidth, textureHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(textureType, sourceTextureId);

		glUniform1i(texLoc, 0);

		texQuadDisplayer->display();

		glBindTexture(textureType, 0);
	}
}
