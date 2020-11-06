#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(ShapeType shapeType) :
            shapeType(shapeType),
            pShapeCount(1),
            vertexCoordType(CoordType::INT),
            vertexCoordDimension(CoordDimension::_2D),
            vertexCoord(nullptr),
            textureCoordType(CoordType::INT),
            textureCoordDimension(CoordDimension::_2D),
            textureCoord(nullptr),
            transparencyEnabled(false),
            depthTestEnabled(false) {

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

    GenericRendererBuilder *GenericRendererBuilder::vertexData(CoordType vertexCoordType, CoordDimension vertexCoordDimension, void *vertexCoord) {
        this->vertexCoordType = vertexCoordType;
        this->vertexCoordDimension = vertexCoordDimension;
        this->vertexCoord = vertexCoord;
        return this;
    }

    CoordType GenericRendererBuilder::getVertexCoordType() const {
        return vertexCoordType;
    }

    CoordDimension GenericRendererBuilder::getVertexCoordDimension() const {
        return vertexCoordDimension;
    }

    void *GenericRendererBuilder::getVertexCoord() const {
        return vertexCoord;
    }

    GenericRendererBuilder *GenericRendererBuilder::textureData(CoordType textureCoordType, CoordDimension textureCoordDimension, void *textureCoord) {
        this->textureCoordType = textureCoordType;
        this->textureCoordDimension = textureCoordDimension;
        this->textureCoord = textureCoord;
        return this;
    }

    CoordType GenericRendererBuilder::getTextureCoordType() const {
        return textureCoordType;
    }

    CoordDimension GenericRendererBuilder::getTextureCoordDimension() const {
        return textureCoordDimension;
    }

    void *GenericRendererBuilder::getTextureCoord() const {
        return textureCoord;
    }

    GenericRendererBuilder *GenericRendererBuilder::enableTransparency() {
        this->transparencyEnabled = true;
        return this;
    }

    bool GenericRendererBuilder::isTransparencyEnabled() const {
        return transparencyEnabled;
    }

    GenericRendererBuilder *GenericRendererBuilder::enableDepthTest() {
        this->depthTestEnabled = true;
        return this;
    }

    bool GenericRendererBuilder::isDepthTestEnabled() const {
        return depthTestEnabled;
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
            vertexCoordType = CoordType::INT;
            vertexCoordDimension = CoordDimension::_2D;
            vertexCoord = new int[8]{-1, 1, 1, 1, 1, -1, -1, -1}; //TODO remove ....
        }

        if (!textureCoord) { //TODO keep null if not specified, no ?
            textureCoordType = CoordType::INT;
            textureCoordDimension = CoordDimension::_2D;
            textureCoord = new int[8]{0, 1, 1, 1, 1, 0, 0, 0}; //TODO remove ....
        }

        return std::make_shared<GenericRenderer>(this); //TODO build unique ptr
    }

}
