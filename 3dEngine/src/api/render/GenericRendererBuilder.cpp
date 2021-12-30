#include <utility>

#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(std::string name, RenderTarget& renderTarget, const Shader& shader, ShapeType shapeType) :
            name(std::move(name)),
            renderTarget(renderTarget),
            shader(shader),
            shapeType(shapeType),
            depthTestEnabled(false),
            depthWriteEnabled(false),
            cullFaceEnabled(true),
            pPolygonMode(PolygonMode::FILL),
            scissorEnabled(false) {

    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::create(std::string name, RenderTarget& renderTarget, const Shader& shader, ShapeType shapeType) {
        return std::shared_ptr<GenericRendererBuilder>(new GenericRendererBuilder(std::move(name), renderTarget, shader, shapeType));
    }

    const std::string& GenericRendererBuilder::getName() const {
        return name;
    }

    RenderTarget& GenericRendererBuilder::getRenderTarget() const {
        return renderTarget;
    }

    const Shader& GenericRendererBuilder::getShader() const {
        return shader;
    }

    ShapeType GenericRendererBuilder::getShapeType() const {
        return shapeType;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Point2<float>>& dataPtr) {
        assert(data.empty() || dataPtr.size() == data[0].getDataCount());
        data.emplace_back(DataType::FLOAT, DataDimension::TWO_DIMENSION, dataPtr.size(), dataPtr.data());
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Point3<float>>& dataPtr) {
        assert(data.empty() || dataPtr.size() == data[0].getDataCount());
        data.emplace_back(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data());
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Vector3<float>>& dataPtr) {
        assert(data.empty() || dataPtr.size() == data[0].getDataCount());
        data.emplace_back(DataType::FLOAT, DataDimension::THREE_DIMENSION, dataPtr.size(), dataPtr.data());
        return shared_from_this();
    }

    const std::vector<DataContainer>& GenericRendererBuilder::getData() const {
        return data;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::instanceData(std::size_t instanceCount, std::size_t dataSize, const float* dataPtr) {
        DataDimension dataDimension;
        if (dataSize / sizeof(float) == 16) {
            dataDimension = DataDimension::DIM_16;
        } else if (dataSize / sizeof(float) == 32) {
            dataDimension = DataDimension::DIM_32;
        } else {
            throw std::runtime_error(""); //TODO improve msg + if/else
        }

        mInstanceData = std::make_shared<DataContainer>(DataType::FLOAT, dataDimension, instanceCount, dataPtr);
        return shared_from_this();
    }

    const std::shared_ptr<DataContainer>& GenericRendererBuilder::getInstanceData() const {
        return mInstanceData;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::indices(const std::vector<uint32_t>& indices) {
        mIndices = std::make_shared<IndexContainer>(indices.size(), indices.data());
        return shared_from_this();
    }

    const std::shared_ptr<IndexContainer>& GenericRendererBuilder::getIndices() const {
        return mIndices;
    }

    /**
     * @param dataPtr Shader data pointer. Data must be memory aligned with "alignas(N)" expression using std140 alignment:
     *   - N=4 for scalar
     *   - N=8 for Vector2
     *   - N=16 for Point3, Point4, Vector3, Vector4, Matrix3 (not working ?), Matrix4
     *   - N=16 for embedded struct
     *   - N=16 for an array (important: the array elements are rounded up to 16 bytes. Therefore, an array of float (4 bytes) in C++ won't match an array of float in the shader.)
     */
    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addUniformData(std::size_t dataSize, const void* dataPtr) {
        if (!uniformTextureReaders.empty()) {
            throw std::runtime_error("Adding uniform data after uniform texture is discouraged. Uniform binding ID start first with all data and then with all textures.");
        }
        uniformData.emplace_back(dataSize, dataPtr);
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

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::enableTransparency(const std::vector<BlendFunction>& blendFunctions) {
        assert(!blendFunctions.empty());
        this->blendFunctions = blendFunctions;
        return shared_from_this();
    }

    const std::vector<BlendFunction>& GenericRendererBuilder::getBlendFunctions() const {
        return blendFunctions;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::enableDepthTest() {
        this->depthTestEnabled = true;
        return shared_from_this();
    }

    bool GenericRendererBuilder::isDepthTestEnabled() const {
        return depthTestEnabled;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::enableDepthWrite() {
        this->depthWriteEnabled = true;
        return shared_from_this();
    }

    bool GenericRendererBuilder::isDepthWriteEnabled() const {
        return depthWriteEnabled;
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

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::enableScissor(const Vector2<int>& scissorOffset, const Vector2<unsigned int>& scissorSize) {
        scissorEnabled = true;
        this->scissorOffset = scissorOffset;
        this->scissorSize = scissorSize;
        return shared_from_this();
    }

    bool GenericRendererBuilder::isScissorEnabled() const {
        return scissorEnabled;
    }

    const Vector2<int>& GenericRendererBuilder::getScissorOffset() const {
        return scissorOffset;
    }

    const Vector2<unsigned int>& GenericRendererBuilder::getScissorSize() const {
        return scissorSize;
    }

    std::unique_ptr<GenericRenderer> GenericRendererBuilder::build() {
        auto renderer = std::make_unique<GenericRenderer>(*this);
        renderTarget.addRenderer(renderer.get());
        return renderer;
    }

}
