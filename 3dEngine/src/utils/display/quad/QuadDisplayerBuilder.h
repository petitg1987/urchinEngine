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
			unsigned int pBufferUsage;

			unsigned int vertexDataType;
			void *vertexCoord;

			unsigned int textureDataType;
			void *textureCoord;
	};

}

#endif

