#ifndef ENGINE_TEXTUREFILTER_H
#define ENGINE_TEXTUREFILTER_H

#include <string>

namespace urchin
{

	class TextureFilterBuilder;

	class TextureFilter
	{
		public:
			enum FilterType
			{
				DOWN_SAMPLE,
				BLUR
			};

			TextureFilter(const TextureFilterBuilder *);
			~TextureFilter();

			unsigned int getFboId() const;
			unsigned int getTextureID() const;

			void applyOn(unsigned int) const;

		private:
			void initializeDisplay();
			void initializeTexture();

			unsigned int textureWidth, textureHeight;
			unsigned int textureType;
			unsigned int textureNumberLayer;
			int textureInternalFormat;
			unsigned int textureFormat;

			unsigned int *bufferIDs;
			unsigned int vertexArrayObject;
			enum //buffer IDs indexes
			{
				VAO_VERTEX_POSITION = 0,
				VAO_TEX_COORD
			};
			enum //shader input
			{
				SHADER_VERTEX_POSITION = 0,
				SHADER_TEX_COORD
			};
			unsigned int downSampleShader;
			int texLoc;

			unsigned int fboID;
			unsigned int textureID;
	};

}

#endif
