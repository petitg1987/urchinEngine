#ifndef URCHINENGINE_GENERICDISPLAYERBUILDER_H
#define URCHINENGINE_GENERICDISPLAYERBUILDER_H

#include <memory>
#include <vector>

#include "graphic/displayer/generic/GenericDisplayer.h"
#include "graphic/displayer/CoordDataType.h"
#include "graphic/displayer/Texture.h"

namespace urchin {

    class GenericDisplayerBuilder {
        public:
            GenericDisplayerBuilder();

            GenericDisplayerBuilder *numberOfQuad(unsigned int);
            unsigned int getNumberOfQuad() const;

            GenericDisplayerBuilder *dimension(unsigned int);
            unsigned int getDimension() const;

            GenericDisplayerBuilder *vertexData(CoordDataType, void *, bool);
            CoordDataType getVertexDataType() const;
            void *getVertexCoord() const;
            bool isDeleteVertexCoord() const;

            GenericDisplayerBuilder *textureData(CoordDataType, void *, bool);
            CoordDataType getTextureDataType() const;
            void *getTextureCoord() const;
            bool isDeleteTextureCoord() const;

            GenericDisplayerBuilder *addTexture(Texture);
            std::vector<Texture> getTextures() const;

            std::shared_ptr<GenericDisplayer> build();

        private:
            unsigned int pNumberOfQuad;
            unsigned int pDimension;

            CoordDataType vertexDataType;
            void *vertexCoord;
            bool deleteVertexCoord;

            CoordDataType textureDataType;
            void *textureCoord;
            bool deleteTextureCoord;

            std::vector<Texture> textures;
    };

}

#endif

