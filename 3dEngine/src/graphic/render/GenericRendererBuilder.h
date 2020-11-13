#ifndef URCHINENGINE_GENERICRENDERERBUILDER_H
#define URCHINENGINE_GENERICRENDERERBUILDER_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

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

            GenericRendererBuilder *vertexCoord(std::vector<Point2<float>> *);
            GenericRendererBuilder *vertexCoord(std::vector<Point3<float>> *);
            CoordType getVertexCoordType() const;
            CoordDimension getVertexCoordDimension() const;
            void *getVertexCoord() const;
            unsigned int getVertexCoordCount() const;

            GenericRendererBuilder *textureCoord(std::vector<Point2<float>> *);
            GenericRendererBuilder *textureCoord(std::vector<Point3<float>> *);
            CoordType getTextureCoordType() const;
            CoordDimension getTextureCoordDimension() const;
            void *getTextureCoord() const;
            unsigned int getTextureCoordCount() const;

            GenericRendererBuilder *enableTransparency();
            bool isTransparencyEnabled() const;

            GenericRendererBuilder *enableDepthTest();
            bool isDepthTestEnabled() const;

            GenericRendererBuilder *disableCullFace();
            bool isCullFaceEnabled() const;

            GenericRendererBuilder *polygonMode(PolygonMode);
            PolygonMode getPolygonMode() const;
            GenericRendererBuilder *outlineSize(float);
            float getOutlineSize() const;

            GenericRendererBuilder *addTexture(Texture);
            std::vector<Texture> getTextures() const;

            std::unique_ptr<GenericRenderer> build();

        private:
            void setVertexData(CoordType, CoordDimension, void *, unsigned int);
            void setTextureData(CoordType, CoordDimension, void *, unsigned int);

            ShapeType shapeType;

            CoordType vertexCoordType;
            CoordDimension vertexCoordDimension;
            void *pVertexCoord;
            unsigned int vertexCoordCount;

            CoordType textureCoordType;
            CoordDimension textureCoordDimension;
            void *pTextureCoord;
            unsigned int textureCoordCount;

            bool transparencyEnabled;

            bool depthTestEnabled;

            bool cullFaceEnabled;

            PolygonMode pPolygonMode;
            float pOutlineSize;

            std::vector<Texture> textures;
    };

}

#endif

