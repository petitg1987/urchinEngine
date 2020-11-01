#ifndef URCHINENGINE_QUADDISPLAYERBUILDER_H
#define URCHINENGINE_QUADDISPLAYERBUILDER_H

#include <memory>

#include "utils/display/quad/QuadDisplayer.h"

namespace urchin {

    class QuadDisplayerBuilder {
        public:
            QuadDisplayerBuilder();

            QuadDisplayerBuilder *numberOfQuad(unsigned int);
            unsigned int getNumberOfQuad() const;

            QuadDisplayerBuilder *dimension(unsigned int);
            unsigned int getDimension() const;

            QuadDisplayerBuilder *bufferUsage(unsigned int);
            unsigned int getBufferUsage() const;

            QuadDisplayerBuilder *vertexData(unsigned int, void *, bool);
            unsigned int getVertexDataType() const;
            void *getVertexCoord() const;
            bool isDeleteVertexCoord() const;

            QuadDisplayerBuilder *textureData(unsigned int, void *, bool);
            unsigned int getTextureDataType() const;
            void *getTextureCoord() const;
            bool isDeleteTextureCoord() const;

            std::shared_ptr<QuadDisplayer> build();

        private:
            unsigned int pNumberOfQuad;
            unsigned int pDimension;

            unsigned int vertexDataType;
            void *vertexCoord;
            bool deleteVertexCoord;

            unsigned int textureDataType;
            void *textureCoord;
            bool deleteTextureCoord;
    };

}

#endif

