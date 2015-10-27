#ifndef ENGINE_TEXTUREFILTER_H
#define ENGINE_TEXTUREFILTER_H

#include <string>

namespace urchin
{

	struct TextureReference
	{

	};


	class TextureFilter
	{
		public:
			enum FilterType
			{
				DOWN_SAMPLE,
				BLUR
			};

			TextureFilter(TextureFilter::FilterType, unsigned int, unsigned int);
			~TextureFilter();

			void setNumberLayer(unsigned int);

			void initialize();

			unsigned int getFboId() const;
			unsigned int getTextureID() const;

			void applyOn(unsigned int) const;

		private:
			bool isInitialized;

			unsigned int textureWidth, textureHeight;
			unsigned int numberLayer;

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
