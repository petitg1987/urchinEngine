#ifndef URCHINENGINE_GENERICRENDERER_H
#define URCHINENGINE_GENERICRENDERER_H

#include <vector>

#include "graphic/render/model/CoordType.h"
#include "graphic/render/model/CoordDimension.h"
#include "graphic/render/model/ShapeType.h"
#include "graphic/render/model/Texture.h"

namespace urchin {

    class GenericRendererBuilder;

    class GenericRenderer {
        public:
            explicit GenericRenderer(const GenericRendererBuilder *);
            ~GenericRenderer();

            void updateTexture(std::size_t, Texture);
            unsigned int addAdditionalTexture(Texture);
            void clearAdditionalTextures();

            void draw() const;

        private:
            void initializeTexture(Texture) const;
            void initializeDisplay();

            unsigned int shapeTypeToVertexCount(ShapeType) const;
            unsigned int shapeTypeToGlType(ShapeType) const;
            unsigned int coordTypeToSize(CoordType) const;
            unsigned int coordTypeToGlType(CoordType) const;
            unsigned int coordDimensionToSize(CoordDimension) const;

            ShapeType shapeType;
            unsigned int shapeCount;

            CoordType vertexCoordType;
            CoordDimension vertexCoordDimension;
            void *vertexCoord; //TODO don't save void* as can be destroy

            CoordType textureCoordType;
            CoordDimension textureCoordDimension;
            void *textureCoord; //TODO don't save void* as can be destroy

            bool transparencyEnabled;
            bool depthTestEnabled;

            std::vector<Texture> textures, additionalTextures;

            unsigned int bufferIDs[2], vertexArrayObject;
            enum //buffer IDs indices
            {
                VAO_VERTEX_POSITION = 0,
                VAO_TEX_COORD
            };
            enum //shader input
            {
                SHADER_VERTEX_POSITION = 0,
                SHADER_TEX_COORD
            };
    };

}

#endif

