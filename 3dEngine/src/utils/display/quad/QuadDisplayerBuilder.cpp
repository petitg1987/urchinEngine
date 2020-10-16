#include <GL/glew.h>

#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin {

    QuadDisplayerBuilder::QuadDisplayerBuilder() :
        pNumberOfQuad(1),
        pDimension(2), //2D
        pBufferUsage(GL_STATIC_DRAW),
        vertexDataType(GL_INT),
        vertexCoord(nullptr),
        deleteVertexCoord(false),
        textureDataType(GL_INT),
        textureCoord(nullptr),
        deleteTextureCoord(false) {

    }

    QuadDisplayerBuilder *QuadDisplayerBuilder::numberOfQuad(unsigned int numberOfQuad) {
        this->pNumberOfQuad = numberOfQuad;
        return this;
    }

    unsigned int QuadDisplayerBuilder::getNumberOfQuad() const {
        return pNumberOfQuad;
    }

    QuadDisplayerBuilder *QuadDisplayerBuilder::dimension(unsigned int dimension) {
        this->pDimension = dimension;
        return this;
    }

    unsigned int QuadDisplayerBuilder::getDimension() const {
        return pDimension;
    }

    QuadDisplayerBuilder *QuadDisplayerBuilder::bufferUsage(unsigned int bufferUsage) {
        this->pBufferUsage = bufferUsage;
        return this;
    }

    unsigned int QuadDisplayerBuilder::getBufferUsage() const {
        return pBufferUsage;
    }

    QuadDisplayerBuilder *QuadDisplayerBuilder::vertexData(unsigned int vertexDataType, void *vertexCoord, bool deleteVertexCoord) {
        this->vertexDataType = vertexDataType;
        this->vertexCoord = vertexCoord;
        this->deleteVertexCoord = deleteVertexCoord;
        return this;
    }

    unsigned int QuadDisplayerBuilder::getVertexDataType() const {
        return vertexDataType;
    }

    void *QuadDisplayerBuilder::getVertexCoord() const {
        return vertexCoord;
    }

    bool QuadDisplayerBuilder::isDeleteVertexCoord() const {
        return deleteVertexCoord;
    }

    QuadDisplayerBuilder *QuadDisplayerBuilder::textureData(unsigned int textureDataType, void *textureCoord, bool deleteTextureCoord) {
        this->textureDataType = textureDataType;
        this->textureCoord = textureCoord;
        this->deleteTextureCoord = deleteTextureCoord;
        return this;
    }

    unsigned int QuadDisplayerBuilder::getTextureDataType() const {
        return textureDataType;
    }

    void *QuadDisplayerBuilder::getTextureCoord() const {
        return textureCoord;
    }

    bool QuadDisplayerBuilder::isDeleteTextureCoord() const {
        return deleteTextureCoord;
    }

    std::shared_ptr<QuadDisplayer> QuadDisplayerBuilder::build() {
        if(!vertexCoord) {
            vertexCoord = new int[8]{-1, 1, 1, 1, 1, -1, -1, -1};
            deleteVertexCoord = true;
        }

        if(!textureCoord) {
            textureCoord = new int[8]{0, 1, 1, 1, 1, 0, 0, 0};
            deleteTextureCoord = true;
        }

        return std::make_shared<QuadDisplayer>(this);
    }

}
