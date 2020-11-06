#ifndef URCHINENGINE_GENERICRENDERERBUILDER_H
#define URCHINENGINE_GENERICRENDERERBUILDER_H

#include <memory>
#include <vector>

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/model/CoordType.h"
#include "graphic/render/model/CoordDimension.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/Texture.h"

namespace urchin {

    class GenericRendererBuilder {
        public:
            explicit GenericRendererBuilder(ShapeType);

            ShapeType getShapeType() const;

            GenericRendererBuilder *shapeCount(unsigned int);
            unsigned int getShapeCount() const;

            GenericRendererBuilder *vertexData(CoordType, CoordDimension, void *);
            CoordType getVertexCoordType() const;
            CoordDimension getVertexCoordDimension() const;
            void *getVertexCoord() const;

            GenericRendererBuilder *textureData(CoordType, CoordDimension, void *);
            CoordType getTextureCoordType() const;
            CoordDimension getTextureCoordDimension() const;
            void *getTextureCoord() const;

            GenericRendererBuilder *enableTransparency();
            bool isTransparencyEnabled() const;

            GenericRendererBuilder *enableDepthTest();
            bool isDepthTestEnabled() const;

            GenericRendererBuilder *addTexture(Texture);
            std::vector<Texture> getTextures() const;

            std::shared_ptr<GenericRenderer> build();

        private:
            ShapeType shapeType;
            unsigned int pShapeCount;

            CoordType vertexCoordType;
            CoordDimension vertexCoordDimension;
            void *vertexCoord;

            CoordType textureCoordType;
            CoordDimension textureCoordDimension;
            void *textureCoord;

            bool transparencyEnabled;

            bool depthTestEnabled;

            std::vector<Texture> textures;
    };

}

#endif

