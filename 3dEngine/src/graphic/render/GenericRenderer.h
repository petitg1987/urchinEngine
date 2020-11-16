#ifndef URCHINENGINE_GENERICRENDERER_H
#define URCHINENGINE_GENERICRENDERER_H

#include <vector>
#include "UrchinCommon.h"

#include "graphic/render/model/CoordType.h"
#include "graphic/render/model/CoordDimension.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/PolygonMode.h"
#include "graphic/render/model/Texture.h"

namespace urchin {

    class GenericRendererBuilder;

    class GenericRenderer {
        public:
            struct PointsCoord {
                CoordType coordType = CoordType::FLOAT;
                CoordDimension coordDimension = CoordDimension::TWO_DIMENSION;
                const void *ptr = nullptr;
                unsigned int pointsCount = 0;
            };

            struct Indices {
                const void *ptr = nullptr;
                unsigned int indicesCount = 0;
            };

            explicit GenericRenderer(const GenericRendererBuilder *);
            ~GenericRenderer();

            void updatePointsCoord(std::size_t, const std::vector<Point3<float>> *);

            void updateTexture(std::size_t, Texture);
            unsigned int addAdditionalTexture(Texture);
            void clearAdditionalTextures();

            void draw() const;

        private:
            unsigned int computeVerticesCount() const;
            void initializeTexture(Texture) const;
            void initializeDisplay();
            void sendPointsCoord(std::size_t, bool) const;

            unsigned int shapeTypeToGlType(ShapeType) const;
            unsigned int coordTypeToSize(CoordType) const;
            unsigned int coordTypeToGlType(CoordType) const;
            unsigned int coordDimensionToSize(CoordDimension) const;

            void resetDrawDefaultValues() const;

            ShapeType shapeType;

            std::vector<PointsCoord> pointsCoords;
            Indices indices;
            unsigned int verticesCount;

            bool transparencyEnabled;

            bool depthTestEnabled;

            bool cullFaceEnabled;

            PolygonMode polygonMode;
            float outlineSize;

            std::vector<Texture> textures, additionalTextures;

            unsigned int vertexArrayObject;
            std::vector<unsigned int> bufferIds;
    };

}

#endif

