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

            GenericRendererBuilder *addPointsCoord(const std::vector<Point2<float>> *);
            GenericRendererBuilder *addPointsCoord(const std::vector<Point3<float>> *);
            GenericRendererBuilder *addPointsCoord(const std::vector<Vector3<float>> *);
            const std::vector<GenericRenderer::PointsCoord> &getPointsCoords() const;

            GenericRendererBuilder *indices(const std::vector<unsigned int> *);
            const GenericRenderer::Indices &getIndices() const;

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
            const std::vector<Texture> &getTextures() const;

            std::unique_ptr<GenericRenderer> build();

        private:
            ShapeType shapeType;
            std::vector<GenericRenderer::PointsCoord> pointsCoords;
            GenericRenderer::Indices pIndices;
            bool transparencyEnabled;
            bool depthTestEnabled;
            bool cullFaceEnabled;
            PolygonMode pPolygonMode;
            float pOutlineSize;
            std::vector<Texture> textures;
    };

}

#endif

