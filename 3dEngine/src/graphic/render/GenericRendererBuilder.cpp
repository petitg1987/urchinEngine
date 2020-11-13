#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(ShapeType shapeType) :
            shapeType(shapeType),
            vertexCoordType(CoordType::FLOAT),
            vertexCoordDimension(CoordDimension::TWO_DIMENSION),
            pVertexCoord(nullptr),
            vertexCoordCount(0),
            textureCoordType(CoordType::FLOAT),
            textureCoordDimension(CoordDimension::TWO_DIMENSION),
            pTextureCoord(nullptr),
            textureCoordCount(0),
            transparencyEnabled(false),
            depthTestEnabled(false),
            cullFaceEnabled(true),
            pPolygonMode(PolygonMode::FILL),
            pOutlineSize(1.0f) {

    }

    ShapeType GenericRendererBuilder::getShapeType() const {
        return shapeType;
    }

    GenericRendererBuilder *GenericRendererBuilder::vertexCoord(std::vector<Point2<float>> *vertexCoord) {
        setVertexData(CoordType::FLOAT, CoordDimension::TWO_DIMENSION, &(*vertexCoord)[0], vertexCoord->size());
        return this;
    }

    GenericRendererBuilder *GenericRendererBuilder::vertexCoord(std::vector<Point3<float>> *vertexCoord) {
        setVertexData(CoordType::FLOAT, CoordDimension::THREE_DIMENSION, &(*vertexCoord)[0], vertexCoord->size());
        return this;
    }

    CoordType GenericRendererBuilder::getVertexCoordType() const {
        return vertexCoordType;
    }

    CoordDimension GenericRendererBuilder::getVertexCoordDimension() const {
        return vertexCoordDimension;
    }

    void *GenericRendererBuilder::getVertexCoord() const {
        return pVertexCoord;
    }

    unsigned int GenericRendererBuilder::getVertexCoordCount() const {
        return vertexCoordCount;
    }

    GenericRendererBuilder *GenericRendererBuilder::textureCoord(std::vector<Point2<float>> *textureCoord) {
        setTextureData(CoordType::FLOAT, CoordDimension::TWO_DIMENSION, &(*textureCoord)[0], textureCoord->size());
        return this;
    }

    GenericRendererBuilder *GenericRendererBuilder::textureCoord(std::vector<Point3<float>> *textureCoord) {
        setTextureData(CoordType::FLOAT, CoordDimension::THREE_DIMENSION, &(*textureCoord)[0], textureCoord->size());
        return this;
    }

    CoordType GenericRendererBuilder::getTextureCoordType() const {
        return textureCoordType;
    }

    CoordDimension GenericRendererBuilder::getTextureCoordDimension() const {
        return textureCoordDimension;
    }

    void *GenericRendererBuilder::getTextureCoord() const {
        return pTextureCoord;
    }

    unsigned int GenericRendererBuilder::getTextureCoordCount() const {
        return textureCoordCount;
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

    GenericRendererBuilder *GenericRendererBuilder::disableCullFace() {
        cullFaceEnabled = false;
        return this;
    }

    bool GenericRendererBuilder::isCullFaceEnabled() const {
        return cullFaceEnabled;
    }

    GenericRendererBuilder *GenericRendererBuilder::polygonMode(PolygonMode pPolygonMode) {
        this->pPolygonMode = pPolygonMode;
        return this;
    }

    PolygonMode GenericRendererBuilder::getPolygonMode() const {
        return pPolygonMode;
    }

    GenericRendererBuilder *GenericRendererBuilder::outlineSize(float pOutlineSize) {
        this->pOutlineSize = pOutlineSize;
        return this;
    }

    float GenericRendererBuilder::getOutlineSize() const {
        return pOutlineSize;
    }

    GenericRendererBuilder *GenericRendererBuilder::addTexture(Texture texture) {
        textures.push_back(texture);
        return this;
    }

    std::vector<Texture> GenericRendererBuilder::getTextures() const {
        return textures;
    }

    std::unique_ptr<GenericRenderer> GenericRendererBuilder::build() {
        return std::make_unique<GenericRenderer>(this);
    }

    void GenericRendererBuilder::setVertexData(CoordType vertexCoordType, CoordDimension vertexCoordDimension, void *vertexCoord, unsigned int vertexCoordCount) {
        this->vertexCoordType = vertexCoordType;
        this->vertexCoordDimension = vertexCoordDimension;
        this->pVertexCoord = vertexCoord;
        this->vertexCoordCount = vertexCoordCount;
    }

    void GenericRendererBuilder::setTextureData(CoordType textureCoordType, CoordDimension textureCoordDimension, void *textureCoord, unsigned int textureCoordCount) {
        this->textureCoordType = textureCoordType;
        this->textureCoordDimension = textureCoordDimension;
        this->pTextureCoord = textureCoord;
        this->textureCoordCount = textureCoordCount;
    }

}
