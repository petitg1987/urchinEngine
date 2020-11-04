#include "graphic/displayer/generic/GenericDisplayerBuilder.h"

namespace urchin {

    GenericDisplayerBuilder::GenericDisplayerBuilder(ShapeType shapeType) :
            shapeType(shapeType),
            pShapeCount(1),
            pDimension(2), //2D
            vertexDataType(CoordDataType::INT),
            vertexCoord(nullptr),
            deleteVertexCoord(false),
            textureDataType(CoordDataType::INT),
            textureCoord(nullptr),
            deleteTextureCoord(false) {

    }

    ShapeType GenericDisplayerBuilder::getShapeType() const {
        return shapeType;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::shapeCount(unsigned int shapeCount) {
        this->pShapeCount = shapeCount;
        return this;
    }

    unsigned int GenericDisplayerBuilder::getShapeCount() const {
        return pShapeCount;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::dimension(unsigned int dimension) {
        this->pDimension = dimension;
        return this;
    }

    unsigned int GenericDisplayerBuilder::getDimension() const {
        return pDimension;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::vertexData(CoordDataType vertexDataType, void *vertexCoord, bool deleteVertexCoord) {
        this->vertexDataType = vertexDataType;
        this->vertexCoord = vertexCoord;
        this->deleteVertexCoord = deleteVertexCoord;
        return this;
    }

    CoordDataType GenericDisplayerBuilder::getVertexDataType() const {
        return vertexDataType;
    }

    void *GenericDisplayerBuilder::getVertexCoord() const {
        return vertexCoord;
    }

    bool GenericDisplayerBuilder::isDeleteVertexCoord() const {
        return deleteVertexCoord;
    }

    GenericDisplayerBuilder *GenericDisplayerBuilder::textureData(CoordDataType textureDataType, void *textureCoord, bool deleteTextureCoord) {
        this->textureDataType = textureDataType;
        this->textureCoord = textureCoord;
        this->deleteTextureCoord = deleteTextureCoord;
        return this;
    }

    CoordDataType GenericDisplayerBuilder::getTextureDataType() const {
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
