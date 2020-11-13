#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(ShapeType shapeType) :
            shapeType(shapeType),
            transparencyEnabled(false),
            depthTestEnabled(false),
            cullFaceEnabled(true),
            pPolygonMode(PolygonMode::FILL),
            pOutlineSize(1.0f) {

    }

    ShapeType GenericRendererBuilder::getShapeType() const {
        return shapeType;
    }

    GenericRendererBuilder *GenericRendererBuilder::addPointsCoord(std::vector<Point2<float>> *pointsCoord) {
        GenericRenderer::PointsCoord pc{};
        pc.coordType = CoordType::FLOAT;
        pc.coordDimension = CoordDimension::TWO_DIMENSION;
        pc.points = &(*pointsCoord)[0];
        pc.pointsCount = pointsCoord->size();

        pointsCoords.push_back(pc);
        return this;
    }

    GenericRendererBuilder *GenericRendererBuilder::addPointsCoord(std::vector<Point3<float>> *pointsCoord) {
        GenericRenderer::PointsCoord pc{};
        pc.coordType = CoordType::FLOAT;
        pc.coordDimension = CoordDimension::THREE_DIMENSION;
        pc.points = &(*pointsCoord)[0];
        pc.pointsCount = pointsCoord->size();

        pointsCoords.push_back(pc);
        return this;
    }

    std::vector<GenericRenderer::PointsCoord> GenericRendererBuilder::getPointsCoords() const {
        return pointsCoords;
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

    const std::vector<Texture> &GenericRendererBuilder::getTextures() const {
        return textures;
    }

    std::unique_ptr<GenericRenderer> GenericRendererBuilder::build() {
        return std::make_unique<GenericRenderer>(this);
    }

}
