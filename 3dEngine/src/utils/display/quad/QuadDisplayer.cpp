#include <GL/glew.h>
#include <stdexcept>

#include "utils/display/quad/QuadDisplayer.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin {

    QuadDisplayer::QuadDisplayer(const QuadDisplayerBuilder *quadDisplayerBuilder) :
            numberOfQuad(0),
            dimension(0),
            bufferUsage(0),
            vertexDataType(0),
            vertexCoord(nullptr),
            textureDataType(0),
            textureCoord(nullptr),
            bufferIDs(),
            vertexArrayObject(0) {
        glGenBuffers(2, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        update(quadDisplayerBuilder);
    }

    QuadDisplayer::~QuadDisplayer() {
        if(vertexArrayObject!=0) {
            glDeleteVertexArrays(1, &vertexArrayObject);
        }

        glDeleteBuffers(2, bufferIDs);
    }

    void QuadDisplayer::update(const QuadDisplayerBuilder *quadDisplayerBuilder) {
        glBindVertexArray(vertexArrayObject);

        numberOfQuad = quadDisplayerBuilder->getNumberOfQuad();
        dimension = quadDisplayerBuilder->getDimension();
        bufferUsage = quadDisplayerBuilder->getBufferUsage();
        vertexDataType = quadDisplayerBuilder->getVertexDataType();
        vertexCoord = quadDisplayerBuilder->getVertexCoord();
        textureDataType = quadDisplayerBuilder->getTextureDataType();
        textureCoord = quadDisplayerBuilder->getTextureCoord();

        if(vertexDataType!=GL_FLOAT && vertexDataType!=GL_INT && vertexDataType!=GL_UNSIGNED_INT) {
            throw std::invalid_argument("Vertex data type not supported: " + std::to_string(vertexDataType));
        } else if(textureDataType!=GL_FLOAT && textureDataType!=GL_INT && textureDataType!=GL_UNSIGNED_INT) {
            throw std::invalid_argument("Texture data type not supported: " + std::to_string(textureDataType));
        }

        initializeDisplay(quadDisplayerBuilder->isDeleteVertexCoord(), quadDisplayerBuilder->isDeleteTextureCoord());
    }

    void QuadDisplayer::initializeDisplay(bool deleteVertexCoord, bool deleteTextureCoord) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        const unsigned int vertexSize = (vertexDataType==GL_FLOAT ? sizeof(float) : sizeof(int)) * 4*dimension*numberOfQuad;
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexCoord, bufferUsage);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, dimension, vertexDataType, GL_FALSE, 0, nullptr);
        if(deleteVertexCoord) {
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
        glBufferData(GL_ARRAY_BUFFER, textureSize, textureCoord, bufferUsage);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, dimension, textureDataType, GL_FALSE, 0, nullptr);
        if(deleteTextureCoord) {
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

    void QuadDisplayer::display() const {
        if(numberOfQuad > 0) {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            glBindVertexArray(vertexArrayObject);
            glDrawArrays(GL_QUADS, 0, 4 * numberOfQuad);

            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
        }
    }

}
