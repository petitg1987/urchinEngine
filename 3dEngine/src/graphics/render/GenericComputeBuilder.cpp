#include <utility>

#include "graphics/render/GenericComputeBuilder.h"

namespace urchin {

    GenericComputeBuilder::GenericComputeBuilder(std::string name, RenderTarget& renderTarget, const Shader& shader, const Vector2<int>& threadLocalSize) :
            name(std::move(name)),
            renderTarget(renderTarget),
            shader(shader),
            threadLocalSize(threadLocalSize) {
        if ((uint32_t)(threadLocalSize.X * threadLocalSize.Y) > MAX_COMPUTE_WORK_GROUP_INVOCATIONS) {
            throw std::runtime_error("Compute work group invocation exceed the limit of " + std::to_string(MAX_COMPUTE_WORK_GROUP_INVOCATIONS));
        }
    }

    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::create(std::string name, RenderTarget& renderTarget, const Shader& shader, const Vector2<int>& threadLocalSize) {
        return std::shared_ptr<GenericComputeBuilder>(new GenericComputeBuilder(std::move(name), renderTarget, shader, threadLocalSize));
    }

    const std::string& GenericComputeBuilder::getName() const {
        return name;
    }

    RenderTarget& GenericComputeBuilder::getRenderTarget() const {
        return renderTarget;
    }

    const Shader& GenericComputeBuilder::getShader() const {
        return shader;
    }

    const Vector2<int>& GenericComputeBuilder::getThreadLocalSize() const {
        return threadLocalSize;
    }

    /**
     * @param dataPtr Shader data pointer. Data must be memory aligned with "alignas(N)" expression using std140 alignment:
     *   - N=4 for scalar
     *   - N=8 for Vector2
     *   - N=16 for Point3, Point4, Vector3, Vector4, Matrix3 (not working ?), Matrix4
     *   - N=16 for embedded struct
     *   - N=16 for an array (important: the array elements are rounded up to 16 bytes. Therefore, an array of float (4 bytes) in C++ won't match an array of float in the shader.)
     */
    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::addUniformData(uint32_t binding, std::size_t dataSize, const void* dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(!uniformData.contains(binding));
            assert(!uniformTextureReaders.contains(binding));
            assert(!uniformTextureOutputs.contains(binding));
            assert(!storageBufferData.contains(binding));
        #endif

        uniformData.insert({binding, {dataSize, dataPtr}});
        return shared_from_this();
    }

    const std::map<uint32_t, ShaderDataContainer>& GenericComputeBuilder::getUniformData() const {
        return uniformData;
    }

    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::addUniformTextureReader(uint32_t binding, const std::shared_ptr<TextureReader>& textureReader) {
        #ifdef URCHIN_DEBUG
            assert(!uniformData.contains(binding));
            assert(!uniformTextureReaders.contains(binding));
            assert(!uniformTextureOutputs.contains(binding));
            assert(!storageBufferData.contains(binding));
        #endif

        uniformTextureReaders.insert({binding, {textureReader}});
        return shared_from_this();
    }

    const std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>>& GenericComputeBuilder::getUniformTextureReaders() const {
        return uniformTextureReaders;
    }

    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::addUniformTextureOutput(uint32_t binding, const std::shared_ptr<Texture>& textureOutput) {
        #ifdef URCHIN_DEBUG
            assert(!uniformData.contains(binding));
            assert(!uniformTextureReaders.contains(binding));
            assert(!uniformTextureOutputs.contains(binding));
            assert(!storageBufferData.contains(binding));
        #endif

        uniformTextureOutputs.insert({binding, textureOutput});
        return shared_from_this();
    }

    const std::map<uint32_t, std::shared_ptr<Texture>>& GenericComputeBuilder::getUniformTextureOutputs() const {
        return uniformTextureOutputs;
    }

    std::shared_ptr<GenericComputeBuilder> GenericComputeBuilder::addStorageBufferData(uint32_t binding, std::size_t dataSize, const void* dataPtr) {
        #ifdef URCHIN_DEBUG
            assert(!uniformData.contains(binding));
            assert(!uniformTextureReaders.contains(binding));
            assert(!uniformTextureOutputs.contains(binding));
            assert(!storageBufferData.contains(binding));
        #endif

        storageBufferData.insert({binding, {dataSize, dataPtr}});
        return shared_from_this();
    }

    const std::map<uint32_t, ShaderDataContainer>& GenericComputeBuilder::getStorageBufferData() const {
        return storageBufferData;
    }

    std::unique_ptr<GenericCompute> GenericComputeBuilder::build() {
        auto renderer = std::make_unique<GenericCompute>(*this);
        renderTarget.addProcessor(renderer.get());
        return renderer;
    }

}