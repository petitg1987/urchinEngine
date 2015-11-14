#ifndef ENGINE_QUADDISPLAYERBUILDER_H
#define ENGINE_QUADDISPLAYERBUILDER_H

#include <memory>

#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class QuadDisplayerBuilder
	{
		public:
			QuadDisplayerBuilder();
			~QuadDisplayerBuilder();

			QuadDisplayerBuilder *numberOfQuad(unsigned int);
			unsigned int getNumberOfQuad() const;

			QuadDisplayerBuilder *dimension(unsigned int);
			unsigned int getDimension() const;

			QuadDisplayerBuilder *bufferUsage(unsigned int);
			unsigned int getBufferUsage() const;

			QuadDisplayerBuilder *vertexData(unsigned int, void *);
			unsigned int getVertexDataType() const;
			void *getVertexCoord() const;

			QuadDisplayerBuilder *textureData(unsigned int, void *);
			unsigned int getTextureDataType() const;
			void *getTextureCoord() const;

			std::shared_ptr<QuadDisplayer> build();

		private:
			unsigned int pNumberOfQuad;
			unsigned int pDimension;

			unsigned int pBufferUsage;

			unsigned int vertexDataType;
			void *vertexCoord;

			unsigned int textureDataType;
			void *textureCoord;
	};

}

#endif

