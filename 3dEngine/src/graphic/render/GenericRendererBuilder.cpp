#include <utility>

#include "GenericRendererBuilder.h"

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(std::string name, std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Shader> shader, ShapeType shapeType) :
            name(std::move(name)),
            renderTarget(std::move(renderTarget)),
            shader(std::move(shader)),
            shapeType(shapeType),
            transparencyEnabled(false),
            depthOperationsEnabled(false),
            cullFaceEnabled(true),
            pPolygonMode(PolygonMode::FILL),
            pLineWidth(1.0f) {

    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::create(std::string name, std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Shader> shader, ShapeType shapeType) {
        return std::shared_ptr<GenericRendererBuilder>(new GenericRendererBuilder(std::move(name), std::move(renderTarget), std::move(shader), shapeType));
    }

    const std::string& GenericRendererBuilder::getName() const {
        return name;
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

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Point2<float>>& dataPtr) {
        data.emplace_back(DataContainer(DataType::FLOAT, DataDimension::TWO_DIMENSION, dataPtr.size(), dataPtr.data()));
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Point3<float>>& dataPtr) {
        data.emplace_back(DataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data()));
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Vector3<float>>& dataPtr) {
        data.emplace_back(DataContainer(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data()));
        return shared_from_this();
    }

    const std::vector<DataContainer>& GenericRendererBuilder::getData() const {
        return data;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::indices(const std::vector<uint32_t>& indices) {
        bool hasPrimitiveRestartIndex = false;
        for (uint32_t index : indices) {
            if (index == GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE) {
                hasPrimitiveRestartIndex = true;
                break;
            }
        }

        pIndices = std::make_shared<IndexContainer>(indices.size(), indices.data(), hasPrimitiveRestartIndex);
        return shared_from_this();
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
    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addUniformData(std::size_t dataSize, const void* dataPtr) {
        if (!uniformTextureReaders.empty()) {
            throw std::runtime_error("Adding uniform data after uniform texture is discouraged. Uniform binding ID start first with all data and then with all textures.");
        }
        uniformData.emplace_back(ShaderDataContainer(dataSize, dataPtr));
        return shared_from_this();
    }

    const std::vector<ShaderDataContainer> &GenericRendererBuilder::getUniformData() const {
        return uniformData;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addUniformTextureReader(const std::shared_ptr<TextureReader>& textureReader) {
        uniformTextureReaders.push_back({textureReader});
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addUniformTextureReaderArray(const std::vector<std::shared_ptr<TextureReader>>& textureReadersArray) {
        uniformTextureReaders.push_back(textureReadersArray);
        return shared_from_this();
    }

    const std::vector<std::vector<std::shared_ptr<TextureReader>>>& GenericRendererBuilder::getUniformTextureReaders() const {
        return uniformTextureReaders;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::enableTransparency() {
        this->transparencyEnabled = true;
        return shared_from_this();
    }

    bool GenericRendererBuilder::isTransparencyEnabled() const {
        return transparencyEnabled;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::enableDepthOperations() {
        this->depthOperationsEnabled = true;
        return shared_from_this();
    }

    bool GenericRendererBuilder::isDepthOperationsEnabled() const {
        return depthOperationsEnabled;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::disableCullFace() {
        cullFaceEnabled = false;
        return shared_from_this();
    }

    bool GenericRendererBuilder::isCullFaceEnabled() const {
        return cullFaceEnabled;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::polygonMode(PolygonMode pPolygonMode) {
        this->pPolygonMode = pPolygonMode;
        return shared_from_this();
    }

    PolygonMode GenericRendererBuilder::getPolygonMode() const {
        return pPolygonMode;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::lineWidth(float pLineWidth) {
        this->pLineWidth = pLineWidth;
        return shared_from_this();
    }

    float GenericRendererBuilder::getLineWidth() const {
        return pLineWidth;
    }

    std::shared_ptr<GenericRenderer> GenericRendererBuilder::build() {
        auto renderer = std::make_shared<GenericRenderer>(this);
        renderer->addOnRenderTarget();

        return renderer;
    }

}
