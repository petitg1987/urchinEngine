#ifndef URCHINENGINE_GENERICDISPLAYER_H
#define URCHINENGINE_GENERICDISPLAYER_H

#include <vector>

namespace urchin {

    class GenericDisplayerBuilder;

    class GenericDisplayer {
        public:
            explicit GenericDisplayer(const GenericDisplayerBuilder *);
            ~GenericDisplayer();

            void updateTextureId(std::size_t, unsigned int);

            void display() const;

        private:
            void initialize(const GenericDisplayerBuilder *);
            void initializeDisplay(bool, bool);

            unsigned int numberOfQuad;
            unsigned int dimension; //2=2D, 3=3D

            unsigned int vertexDataType;
            void *vertexCoord;

            unsigned int textureDataType;
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

            std::vector<unsigned int> textureIds;
    };

}

#endif

