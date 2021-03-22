#include <utility>

#include "GenericRendererBuilder.h"

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Shader> shader, ShapeType shapeType) :
            renderTarget(std::move(renderTarget)),
            shader(std::move(shader)),
            shapeType(shapeType),
            transparencyEnabled(false),
            depthOperationsEnabled(false),
            cullFaceEnabled(true),
            pPolygonMode(PolygonMode::FILL),
            pOutlineSize(1.0f) {

    }

    const std::shared_ptr<RenderTarget>& GenericRendererBuilder::getRenderTarget() const {
        return renderTarget;
    }

    const std::shared_ptr<Shader>& GenericRendererBuilder::getShader() const {
        return shader;
    }

    ShapeType GenericRendererBuilder::getShapeType() const {
        return shapeType;
    }

    GenericRendererBuilder* GenericRendererBuilder::addData(const std::vector<Point2<float>>& dataPtr) {
        data.emplace_back(DataContainer(DataType::FLOAT, DataDimension::TWO_DIMENSION, dataPtr.size(), dataPtr.data()));
        return this;
    }

    GenericRendererBuilder* GenericRendererBuilder::addData(const std::vector<Point3<float>>& dataPtr) {
        data.emplace_back(DataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data()));
        return this;
    }

    GenericRendererBuilder* GenericRendererBuilder::addData(const std::vector<Vector3<float>>& dataPtr) {
        data.emplace_back(DataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data()));
        return this;
    }

    const std::vector<DataContainer>& GenericRendererBuilder::getData() const {
        return data;
    }

    GenericRendererBuilder* GenericRendererBuilder::indices(const std::vector<uint32_t>& indices) {
        bool hasPrimitiveRestartIndex = false;
        for (uint32_t index : indices) {
            if (index == GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE) {
                hasPrimitiveRestartIndex = true;
                break;
            }
        }

        pIndices = std::make_shared<IndexContainer>(indices.size(), indices.data(), hasPrimitiveRestartIndex);
        return this;
    }

    const std::shared_ptr<IndexContainer>& GenericRendererBuilder::getIndices() const {
        return pIndices;
    }

    /**
     * @param dataPtr Shader data pointer. Data must be memory aligned with "alignas(N)" expression using std140 alignment:
     *   - N=4 for scalar
     *   - N=8 for Vector2
     *   - N=16 for Point3, Point4, Vector3, Vector4, Matrix3, Matrix4
     *   - N=16 for embedded struct
     *   - N=16 for an array (important: the array elements are rounded up to 16 bytes. Therefore, an array of float (4 bytes) in C++ won't match an array of float in the shader.)
     */
    GenericRendererBuilder* GenericRendererBuilder::addShaderData(std::size_t dataSize, const void* dataPtr) {
        shaderData.emplace_back(ShaderDataContainer(dataSize, dataPtr));
        return this;
    }

    const std::vector<ShaderDataContainer> &GenericRendererBuilder::getShaderData() const {
        return shaderData;
    }

    GenericRendererBuilder* GenericRendererBuilder::addTextureReader(const std::shared_ptr<TextureReader>& textureReader) {
        textureReaders.push_back({textureReader});
        return this;
    }

    GenericRendererBuilder* GenericRendererBuilder::addTextureReaderArray(const std::vector<std::shared_ptr<TextureReader>>& textureReadersArray) {
        textureReaders.push_back(textureReadersArray);
        return this;
    }

    const std::vector<std::vector<std::shared_ptr<TextureReader>>>& GenericRendererBuilder::getTextureReaders() const {
        return textureReaders;
    }

    GenericRendererBuilder* GenericRendererBuilder::enableTransparency() {
        this->transparencyEnabled = true;
        return this;
    }

    bool GenericRendererBuilder::isTransparencyEnabled() const {
        return transparencyEnabled;
    }

    GenericRendererBuilder* GenericRendererBuilder::enableDepthOperations() {
        this->depthOperationsEnabled = true;
        return this;
    }

    bool GenericRendererBuilder::isDepthOperationsEnabled() const {
        return depthOperationsEnabled;
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

    std::unique_ptr<GenericRenderer> GenericRendererBuilder::build() {
        return std::make_unique<GenericRenderer>(this);
    }

}
