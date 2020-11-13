#ifndef URCHINENGINE_GENERICRENDERER_H
#define URCHINENGINE_GENERICRENDERER_H

#include <vector>

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
                void *ptr = nullptr;
                unsigned int pointsCount = 0;
            };

            struct Indices {
                void *ptr = nullptr;
                unsigned int indicesCount = 0;
            };

            explicit GenericRenderer(const GenericRendererBuilder *);
            ~GenericRenderer();

            void updateTexture(std::size_t, Texture);
            unsigned int addAdditionalTexture(Texture);
            void clearAdditionalTextures();

            void draw() const;

        private:
            void initializeTexture(Texture) const;
            void initializeDisplay();

            unsigned int shapeTypeToGlType(ShapeType) const;
            unsigned int coordTypeToSize(CoordType) const;
            unsigned int coordTypeToGlType(CoordType) const;
            unsigned int coordDimensionToSize(CoordDimension) const;

            void resetDrawDefaultValues() const;

            ShapeType shapeType;

            std::vector<PointsCoord> pointsCoords;
            unsigned int pointsCount;

            Indices indices;

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

