#include <GL/glew.h>

#include "graphic/displayer/generic/GenericDisplayerBuilder.h"

namespace urchin {

    GenericDisplayerBuilder::GenericDisplayerBuilder() :
        pNumberOfQuad(1),
        pDimension(2), //2D
        vertexDataType(GL_INT),
        vertexCoord(nullptr),
        deleteVertexCoord(false),
        textureDataType(GL_INT),
        textureCoord(nullptr),
        deleteTextureCoord(false) {

    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::numberOfQuad(unsigned int numberOfQuad) {
        this->pNumberOfQuad = numberOfQuad;
        return this;
    }

    unsigned int GenericDisplayerBuilder::getNumberOfQuad() const {
        return pNumberOfQuad;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::dimension(unsigned int dimension) {
        this->pDimension = dimension;
        return this;
    }

    unsigned int GenericDisplayerBuilder::getDimension() const {
        return pDimension;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::vertexData(unsigned int vertexDataType, void *vertexCoord, bool deleteVertexCoord) { //TODO change dataType with urchin enum
        this->vertexDataType = vertexDataType;
        this->vertexCoord = vertexCoord;
        this->deleteVertexCoord = deleteVertexCoord;
        return this;
    }

    unsigned int GenericDisplayerBuilder::getVertexDataType() const {
        return vertexDataType;
    }

    void *GenericDisplayerBuilder::getVertexCoord() const {
        return vertexCoord;
    }

    bool GenericDisplayerBuilder::isDeleteVertexCoord() const {
        return deleteVertexCoord;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::textureData(unsigned int textureDataType, void *textureCoord, bool deleteTextureCoord) {
        this->textureDataType = textureDataType;
        this->textureCoord = textureCoord;
        this->deleteTextureCoord = deleteTextureCoord;
        return this;
    }

    unsigned int GenericDisplayerBuilder::getTextureDataType() const {
        return textureDataType;
    }

    void *GenericDisplayerBuilder::getTextureCoord() const {
        return textureCoord;
    }

    bool GenericDisplayerBuilder::isDeleteTextureCoord() const {
        return deleteTextureCoord;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::addTexture(Texture texture) {
        textures.push_back(texture);
        return this;
    }

    std::vector<Texture> GenericDisplayerBuilder::getTextures() const {
        return textures;
    }

    std::shared_ptr<GenericDisplayer> GenericDisplayerBuilder::build() {
        if (!vertexCoord) {
            vertexCoord = new int[8]{-1, 1, 1, 1, 1, -1, -1, -1};
            deleteVertexCoord = true;
        }

        if (!textureCoord) {
            textureCoord = new int[8]{0, 1, 1, 1, 1, 0, 0, 0};
            deleteTextureCoord = true;
        }

        return std::make_shared<GenericDisplayer>(this);
    }

}
