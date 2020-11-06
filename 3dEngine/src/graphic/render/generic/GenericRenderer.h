#ifndef URCHINENGINE_GENERICRENDERER_H
#define URCHINENGINE_GENERICRENDERER_H

#include <vector>

#include "graphic/render/CoordDataType.h"
#include "graphic/render/ShapeType.h"
#include "graphic/render/Texture.h"

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
            void initializeDisplay(bool, bool);

            unsigned int shapeTypeToVertexCount(ShapeType) const;
            unsigned int shapeTypeToGlType(ShapeType) const;
            unsigned int dataTypeToSize(CoordDataType) const;
            unsigned int dataTypeToGlType(CoordDataType) const;

            ShapeType shapeType;
            unsigned int shapeCount;
            unsigned int dimension; //2=2D, 3=3D

            CoordDataType vertexDataType;
            void *vertexCoord;

            CoordDataType textureDataType;
            void *textureCoord;

            bool transparencyEnabled;

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

