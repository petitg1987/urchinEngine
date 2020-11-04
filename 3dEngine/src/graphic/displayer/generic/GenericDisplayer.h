#ifndef URCHINENGINE_GENERICDISPLAYER_H
#define URCHINENGINE_GENERICDISPLAYER_H

#include <vector>

#include "graphic/displayer/CoordDataType.h"
#include "graphic/displayer/Texture.h"

namespace urchin {

    class GenericDisplayerBuilder;

    class GenericDisplayer {
        public:
            explicit GenericDisplayer(const GenericDisplayerBuilder *);
            ~GenericDisplayer();

            void updateTexture(std::size_t, Texture);

            void display() const; //TODO make sure that all caller use all feature of GenericDisplayer

        private:
            void initialize(const GenericDisplayerBuilder *);
            void initializeDisplay(bool, bool);
            unsigned int dataTypeToSize(CoordDataType) const;
            unsigned int dataTypeToGlType(CoordDataType) const;

            unsigned int numberOfQuad;
            unsigned int dimension; //2=2D, 3=3D

            CoordDataType vertexDataType;
            void *vertexCoord;

            CoordDataType textureDataType;
            void *textureCoord;

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

            std::vector<Texture> textures;
    };

}

#endif

