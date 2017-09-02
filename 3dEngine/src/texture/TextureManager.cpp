#include <GL/glew.h>
#include <GL/gl.h>
#include "UrchinCommon.h"

#include "TextureManager.h"

namespace urchin
{

	TextureManager::TextureManager() : Singleton<TextureManager>(),
		anisotropy(getMaxSupportedAnisotropy())
	{

	}

	float TextureManager::getAnisotropy() const
	{
		return anisotropy;
	}

	void TextureManager::setAnisotropy(float anisotropy)
	{
		this->anisotropy = anisotropy;

		clampToMaxAnisotropy();
	}

	float TextureManager::getMaxSupportedAnisotropy() const
	{
		if(GLEW_EXT_texture_filter_anisotropic)
		{
			float largestAnisotropy = 1.0;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largestAnisotropy);
			return largestAnisotropy;
		}

		return 0.0;
	}

	void TextureManager::clampToMaxAnisotropy()
	{
		if(anisotropy > getMaxSupportedAnisotropy())
		{
			anisotropy = getMaxSupportedAnisotropy();
		}
	}

}
