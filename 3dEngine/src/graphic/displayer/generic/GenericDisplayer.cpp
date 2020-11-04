#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "graphic/displayer/generic/GenericDisplayer.h"
#include "graphic/displayer/generic/GenericDisplayerBuilder.h"

namespace urchin {

    GenericDisplayer::GenericDisplayer(const GenericDisplayerBuilder *quadDisplayerBuilder) :
            numberOfQuad(0), //TODO rename and propose more than quad
            dimension(0),
            vertexDataType(0),
            vertexCoord(nullptr),
            textureDataType(0),
            textureCoord(nullptr),
            bufferIDs(),
            vertexArrayObject(0) {
        glGenBuffers(2, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        initialize(quadDisplayerBuilder);
    }

    GenericDisplayer::~GenericDisplayer() {
        if (vertexArrayObject!=0) {
            glDeleteVertexArrays(1, &vertexArrayObject);
        }

        glDeleteBuffers(2, bufferIDs);
    }

    void GenericDisplayer::initialize(const GenericDisplayerBuilder *quadDisplayerBuilder) {
        glBindVertexArray(vertexArrayObject);

        numberOfQuad = quadDisplayerBuilder->getNumberOfQuad();

        dimension = quadDisplayerBuilder->getDimension();

        vertexDataType = quadDisplayerBuilder->getVertexDataType();
        if (vertexDataType!=GL_FLOAT && vertexDataType!=GL_INT && vertexDataType!=GL_UNSIGNED_INT) {
            throw std::invalid_argument("Vertex data type not supported: " + std::to_string(vertexDataType));
        }
        vertexCoord = quadDisplayerBuilder->getVertexCoord();

        textureDataType = quadDisplayerBuilder->getTextureDataType();
        if (textureDataType!=GL_FLOAT && textureDataType!=GL_INT && textureDataType!=GL_UNSIGNED_INT) {
            throw std::invalid_argument("Texture data type not supported: " + std::to_string(textureDataType));
        }
        textureCoord = quadDisplayerBuilder->getTextureCoord();

        textures = quadDisplayerBuilder->getTextures();

        initializeDisplay(quadDisplayerBuilder->isDeleteVertexCoord(), quadDisplayerBuilder->isDeleteTextureCoord());
    }

    void GenericDisplayer::initializeDisplay(bool deleteVertexCoord, bool deleteTextureCoord) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        const unsigned int vertexSize = (vertexDataType==GL_FLOAT ? sizeof(float) : sizeof(int)) * 4*dimension*numberOfQuad;
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, dimension, vertexDataType, GL_FALSE, 0, nullptr);
        if (deleteVertexCoord) {
            if (vertexDataType == GL_FLOAT) {
                delete[] static_cast<float *>(vertexCoord);
            } else if (vertexDataType == GL_INT) {
                delete[] static_cast<int *>(vertexCoord);
            } else if (vertexDataType == GL_UNSIGNED_INT) {
                delete[] static_cast<unsigned int *>(vertexCoord);
            }
            vertexCoord = nullptr;
        }

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        const unsigned int textureSize = (textureDataType==GL_FLOAT ? sizeof(float) : sizeof(int)) * 4*dimension*numberOfQuad;
        glBufferData(GL_ARRAY_BUFFER, textureSize, textureCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, dimension, textureDataType, GL_FALSE, 0, nullptr);
        if (deleteTextureCoord) {
            if (textureDataType == GL_FLOAT) {
                delete[] static_cast<float *>(textureCoord);
            } else if (textureDataType == GL_INT) {
                delete[] static_cast<int *>(textureCoord);
            } else if (textureDataType == GL_UNSIGNED_INT) {
                delete[] static_cast<unsigned int *>(textureCoord);
            }
            textureCoord = nullptr;
        }
    }

    void GenericDisplayer::updateTexture(std::size_t index, Texture texture) {
        assert(textures.size() > index);
        textures[index] = texture;
    }

    void GenericDisplayer::display() const {
        if(numberOfQuad == 0) {
            return;
        }

        for(std::size_t i=0; i<textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            if(textures[i].getType() == Texture::SIMPLE) {
                glBindTexture(GL_TEXTURE_2D, textures[i].getId());
            }else if(textures[i].getType() == Texture::ARRAY) {
                glBindTexture(GL_TEXTURE_2D_ARRAY, textures[i].getId());
            }
        }

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_QUADS, 0, 4 * numberOfQuad);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

}
