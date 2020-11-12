#ifndef URCHINENGINE_GENERICRENDERERBUILDER_H
#define URCHINENGINE_GENERICRENDERERBUILDER_H

#include <memory>
#include <vector>

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/model/CoordType.h"
#include "graphic/render/model/CoordDimension.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/PolygonMode.h"
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

            GenericRendererBuilder *disableCullFace();
            bool isCullFaceEnabled() const;

            GenericRendererBuilder *polygonMode(PolygonMode, float outlineSize = 1.0f);
            PolygonMode getPolygonMode() const;
            float getOutlineSize() const;

            GenericRendererBuilder *addTexture(Texture);
            std::vector<Texture> getTextures() const;

            std::unique_ptr<GenericRenderer> build();

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

            bool cullFaceEnabled;

            PolygonMode pPolygonMode;
            float outlineSize;

            std::vector<Texture> textures;
    };

}

#endif

