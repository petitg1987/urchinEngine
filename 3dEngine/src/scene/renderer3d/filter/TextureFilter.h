#ifndef ENGINE_TEXTUREFILTER_H
#define ENGINE_TEXTUREFILTER_H

#include <string>
#include <memory>
#include <map>

#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class TextureFilterBuilder;

	class TextureFilter
	{
		public:
			TextureFilter(const TextureFilterBuilder *);
			virtual ~TextureFilter();

			void initialize();

			unsigned int getFboId() const;
			unsigned int getTextureID() const;

			void applyOn(unsigned int) const;

		protected:
			virtual std::string getShaderName() const = 0;
			virtual void completeShaderTokens(std::map<std::string, std::string> &) const = 0;

			unsigned int getTextureWidth() const;
			unsigned int getTextureHeight() const;

		private:
			void initializeDisplay();
			void initializeTexture();

			bool isInitialized;

			unsigned int textureWidth, textureHeight;
			unsigned int textureType;
			unsigned int textureAccessFilter;
			float textureAnisotropy;
			unsigned int textureNumberLayer;
			int textureInternalFormat;
			unsigned int textureFormat;

			std::shared_ptr<QuadDisplayer> texQuadDisplayer;
			unsigned int downSampleShader;
			int texLoc;

			unsigned int fboID;
			unsigned int textureID;
	};

}

#endif
