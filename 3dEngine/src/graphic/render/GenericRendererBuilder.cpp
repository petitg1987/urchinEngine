#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(ShapeType shapeType) :
            shapeType(shapeType),
            pShapeCount(1),
            pDimension(2), //2D
            vertexDataType(CoordType::INT),
            vertexCoord(nullptr),
            deleteVertexCoord(false),
            textureDataType(CoordType::INT),
            textureCoord(nullptr),
            deleteTextureCoord(false),
            transparencyEnabled(false) {

    }

    ShapeType GenericRendererBuilder::getShapeType() const {
        return shapeType;
    }

    GenericRendererBuilder *GenericRendererBuilder::shapeCount(unsigned int shapeCount) {
        this->pShapeCount = shapeCount;
        return this;
    }

    unsigned int GenericRendererBuilder::getShapeCount() const {
        return pShapeCount;
    }

    GenericRendererBuilder *GenericRendererBuilder::dimension(unsigned int dimension) {
        this->pDimension = dimension;
        return this;
    }

    unsigned int GenericRendererBuilder::getDimension() const {
        return pDimension;
    }

    GenericRendererBuilder *GenericRendererBuilder::vertexData(CoordType vertexDataType, void *vertexCoord, bool deleteVertexCoord) {
        this->vertexDataType = vertexDataType;
        this->vertexCoord = vertexCoord;
        this->deleteVertexCoord = deleteVertexCoord;
        return this;
    }

    CoordType GenericRendererBuilder::getVertexDataType() const {
        return vertexDataType;
    }

    void *GenericRendererBuilder::getVertexCoord() const {
        return vertexCoord;
    }

    bool GenericRendererBuilder::isDeleteVertexCoord() const {
        return deleteVertexCoord;
    }

    GenericRendererBuilder *GenericRendererBuilder::textureData(CoordType textureDataType, void *textureCoord, bool deleteTextureCoord) {
        this->textureDataType = textureDataType;
        this->textureCoord = textureCoord;
        this->deleteTextureCoord = deleteTextureCoord;
        return this;
    }

    CoordType GenericRendererBuilder::getTextureDataType() const {
        return textureDataType;
    }

    void *GenericRendererBuilder::getTextureCoord() const {
        return textureCoord;
    }

    bool GenericRendererBuilder::isDeleteTextureCoord() const {
        return deleteTextureCoord;
    }

    GenericRendererBuilder *GenericRendererBuilder::enableTransparency() {
        this->transparencyEnabled = true;
        return this;
    }

    bool GenericRendererBuilder::isTransparencyEnabled() const {
        return transparencyEnabled;
    }

    GenericRendererBuilder *GenericRendererBuilder::addTexture(Texture texture) {
        textures.push_back(texture);
        return this;
    }

    std::vector<Texture> GenericRendererBuilder::getTextures() const {
        return textures;
    }

    std::shared_ptr<GenericRenderer> GenericRendererBuilder::build() {
        if (!vertexCoord) {
            vertexCoord = new int[8]{-1, 1, 1, 1, 1, -1, -1, -1};
            deleteVertexCoord = true;
        }

        if (!textureCoord) {
            textureCoord = new int[8]{0, 1, 1, 1, 1, 0, 0, 0};
            deleteTextureCoord = true;
        }

        return std::make_shared<GenericRenderer>(this); //TODO build unique ptr
    }

}
