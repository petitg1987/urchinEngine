#include <utility>

#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    GenericRendererBuilder::GenericRendererBuilder(std::string name, RenderTarget& renderTarget, const Shader& shader, ShapeType shapeType) :
            name(std::move(name)),
            renderTarget(renderTarget),
            shader(shader),
            shapeType(shapeType),
            customScissor(false),
            scissorOffset(Vector2<int>(0, 0)),
            scissorSize(Vector2<int>((int)renderTarget.getWidth(), (int)renderTarget.getHeight())),
            depthTestEnabled(false),
            depthWriteEnabled(false),
            cullFaceEnabled(true),
            layerIndexDataInShaderEnabled(false) {

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
        data.emplace_back(DataType::FLOAT, VariableType::VEC2, dataPtr.size(), dataPtr.data());
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Point3<float>>& dataPtr) {
        assert(data.empty() || dataPtr.size() == data[0].getDataCount());
        data.emplace_back(DataType::FLOAT, VariableType::VEC3, dataPtr.size(), dataPtr.data());
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addData(const std::vector<Vector3<float>>& dataPtr) {
        assert(data.empty() || dataPtr.size() == data[0].getDataCount());
        data.emplace_back(DataType::FLOAT, VariableType::VEC3, dataPtr.size(), dataPtr.data());
        return shared_from_this();
    }

    const std::vector<DataContainer>& GenericRendererBuilder::getData() const {
        return data;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::instanceData(std::size_t instanceCount, VariableType variableType, const float* dataPtr) {
        mInstanceData = std::make_shared<DataContainer>(DataType::FLOAT, variableType, instanceCount, dataPtr);
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
    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addUniformData(uint32_t uniformBinding, std::size_t dataSize, const void* dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(!uniformData.contains(uniformBinding));
            assert(!uniformTextureReaders.contains(uniformBinding));
        #endif

        uniformData.insert({uniformBinding, {dataSize, dataPtr}});
        return shared_from_this();
    }

    const std::map<uint32_t, ShaderDataContainer>& GenericRendererBuilder::getUniformData() const {
        return uniformData;
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addUniformTextureReader(uint32_t uniformBinding, const std::shared_ptr<TextureReader>& textureReader) {
        #ifdef URCHIN_DEBUG
            assert(!uniformData.contains(uniformBinding));
            assert(!uniformTextureReaders.contains(uniformBinding));
        #endif

        uniformTextureReaders.insert({uniformBinding, {textureReader}});
        return shared_from_this();
    }

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::addUniformTextureReaderArray(uint32_t uniformBinding, const std::vector<std::shared_ptr<TextureReader>>& textureReadersArray) {
        #ifdef URCHIN_DEBUG
            assert(!uniformData.contains(uniformBinding));
            assert(!uniformTextureReaders.contains(uniformBinding));
        #endif

        uniformTextureReaders.insert({uniformBinding, textureReadersArray});
        return shared_from_this();
    }

    const std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>>& GenericRendererBuilder::getUniformTextureReaders() const {
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

    void GenericRendererBuilder::setScissor(const Vector2<int>& scissorOffset, const Vector2<int>& scissorSize) {
        this->customScissor = true;
        this->scissorOffset = scissorOffset;
        this->scissorSize = scissorSize;
    }

    bool GenericRendererBuilder::hasCustomScissor() const {
        return customScissor;
    }

    const Vector2<int>& GenericRendererBuilder::getScissorOffset() const {
        return scissorOffset;
    }

    const Vector2<int>& GenericRendererBuilder::getScissorSize() const {
        return scissorSize;
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

    std::shared_ptr<GenericRendererBuilder> GenericRendererBuilder::enableLayerIndexDataInShader() {
        layerIndexDataInShaderEnabled = true;
        return shared_from_this();
    }

    bool GenericRendererBuilder::isLayerIndexDataInShaderEnabled() const {
        return layerIndexDataInShaderEnabled;
    }

    std::unique_ptr<GenericRenderer> GenericRendererBuilder::build() {
        auto renderer = std::make_unique<GenericRenderer>(*this);
        renderTarget.addProcessor(renderer.get());
        return renderer;
    }

}
