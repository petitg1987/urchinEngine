#ifndef URCHINENGINE_GENERICRENDERERBUILDER_H
#define URCHINENGINE_GENERICRENDERERBUILDER_H

#include <memory>
#include <vector>

#include "graphic/render/generic/GenericRenderer.h"
#include "graphic/render/CoordDataType.h"
#include "graphic/render/ShapeType.h"
#include "graphic/render/Texture.h"

namespace urchin {

    class GenericRendererBuilder {
        public:
            explicit GenericRendererBuilder(ShapeType);

            ShapeType getShapeType() const;

            GenericRendererBuilder *shapeCount(unsigned int);
            unsigned int getShapeCount() const;

            GenericRendererBuilder *dimension(unsigned int);
            unsigned int getDimension() const;

            GenericRendererBuilder *vertexData(CoordDataType, void *, bool);
            CoordDataType getVertexDataType() const;
            void *getVertexCoord() const;
            bool isDeleteVertexCoord() const;

            GenericRendererBuilder *textureData(CoordDataType, void *, bool);
            CoordDataType getTextureDataType() const;
            void *getTextureCoord() const;
            bool isDeleteTextureCoord() const;

            GenericRendererBuilder *enableTransparency();
            bool isTransparencyEnabled() const;

            GenericRendererBuilder *addTexture(Texture);
            std::vector<Texture> getTextures() const;

            std::shared_ptr<GenericRenderer> build();

        private:
            ShapeType shapeType;
            unsigned int pShapeCount;
            unsigned int pDimension;

            CoordDataType vertexDataType;
            void *vertexCoord;
            bool deleteVertexCoord;

            CoordDataType textureDataType;
            void *textureCoord;
            bool deleteTextureCoord;

            bool transparencyEnabled;

            std::vector<Texture> textures;
    };

}

#endif

