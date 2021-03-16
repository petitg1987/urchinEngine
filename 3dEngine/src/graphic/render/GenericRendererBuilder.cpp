#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(std::shared_ptr<RenderTarget> renderTarget, ShapeType shapeType) :
            renderTarget(std::move(renderTarget)),
            shapeType(shapeType),
            transparencyEnabled(false),
            depthTestEnabled(false),
            cullFaceEnabled(true),
            pPolygonMode(PolygonMode::FILL),
            pOutlineSize(1.0f) {

    }

    const std::shared_ptr<RenderTarget>& GenericRendererBuilder::getRenderTarget() const {
        return renderTarget;
    }

    ShapeType GenericRendererBuilder::getShapeType() const {
        return shapeType;
    }

    GenericRendererBuilder* GenericRendererBuilder::addData(const std::vector<Point2<float>>* dataPtr) {
        GenericRenderer::Data dataValue{DataType::FLOAT, DataDimension::TWO_DIMENSION, &(*dataPtr)[0], dataPtr->size()};
        data.push_back(dataValue);
        return this;
    }

    GenericRendererBuilder* GenericRendererBuilder::addData(const std::vector<Point3<float>>* dataPtr) {
        GenericRenderer::Data dataValue{DataType::FLOAT, DataDimension::THREE_DIMENSION, &(*dataPtr)[0], dataPtr->size()};
        data.push_back(dataValue);
        return this;
    }

    GenericRendererBuilder* GenericRendererBuilder::addData(const std::vector<Vector3<float>>* dataPtr) {
        addData(reinterpret_cast<const std::vector<Point3<float>>*>(dataPtr));
        return this;
    }

    const std::vector<GenericRenderer::Data>& GenericRendererBuilder::getData() const {
        return data;
    }

    GenericRendererBuilder* GenericRendererBuilder::indices(const std::vector<unsigned int>* indices) {
        pIndices.ptr = &(*indices)[0];
        pIndices.indicesCount = indices->size();
        pIndices.hasPrimitiveRestartIndex = false;

        for (unsigned int index : (*indices)) {
            if (index == GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE) {
                pIndices.hasPrimitiveRestartIndex = true;
                break;
            }
        }

        return this;
    }

    const GenericRenderer::Indices& GenericRendererBuilder::getIndices() const {
        return pIndices;
    }

    GenericRendererBuilder* GenericRendererBuilder::enableTransparency() {
        this->transparencyEnabled = true;
        return this;
    }

    bool GenericRendererBuilder::isTransparencyEnabled() const {
        return transparencyEnabled;
    }

    GenericRendererBuilder* GenericRendererBuilder::enableDepthTest() {
        this->depthTestEnabled = true;
        return this;
    }

    bool GenericRendererBuilder::isDepthTestEnabled() const {
        return depthTestEnabled;
    }

    GenericRendererBuilder* GenericRendererBuilder::disableCullFace() {
        cullFaceEnabled = false;
        return this;
    }

    bool GenericRendererBuilder::isCullFaceEnabled() const {
        return cullFaceEnabled;
    }

    GenericRendererBuilder* GenericRendererBuilder::polygonMode(PolygonMode pPolygonMode) {
        this->pPolygonMode = pPolygonMode;
        return this;
    }

    PolygonMode GenericRendererBuilder::getPolygonMode() const {
        return pPolygonMode;
    }

    GenericRendererBuilder* GenericRendererBuilder::outlineSize(float pOutlineSize) {
        this->pOutlineSize = pOutlineSize;
        return this;
    }

    float GenericRendererBuilder::getOutlineSize() const {
        return pOutlineSize;
    }

    GenericRendererBuilder* GenericRendererBuilder::addTextureReader(const TextureReader& texture) {
        textures.push_back(texture);
        return this;
    }

    const std::vector<TextureReader>& GenericRendererBuilder::getTextureReaders() const {
        return textures;
    }

    std::unique_ptr<GenericRenderer> GenericRendererBuilder::build() {
        return std::make_unique<GenericRenderer>(this);
    }

}
