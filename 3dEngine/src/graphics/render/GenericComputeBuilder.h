#pragma once

#include "graphics/api/GraphicsApi.h"
#include "graphics/texture/TextureReader.h"
namespace urchin {

    class GenericComputeBuilder : public std::enable_shared_from_this<GenericComputeBuilder> {
        public:
            //Value of 128 is the minimum defined in the Vulkan spec.
            //Note: most of the graphics card can support 256 (see https://vulkan.gpuinfo.org/displaydevicelimit.php?name=maxComputeWorkGroupInvocations&platform=all)
            static constexpr uint32_t MAX_COMPUTE_WORK_GROUP_INVOCATIONS = 128;

            static std::shared_ptr<GenericComputeBuilder> create(std::string, RenderTarget&, const Shader&, const Vector2<int>&);

            const std::string& getName() const;
            RenderTarget& getRenderTarget() const;
            const Shader& getShader() const;
            const Vector2<int>& getThreadLocalSize() const;

            std::shared_ptr<GenericComputeBuilder> addUniformData(uint32_t, std::size_t, const void*);
            const std::map<uint32_t, ShaderDataContainer>& getUniformData() const;

            std::shared_ptr<GenericComputeBuilder> addUniformTextureReader(uint32_t, const std::shared_ptr<TextureReader>&);
            const std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>>& getUniformTextureReaders() const;

            std::shared_ptr<GenericComputeBuilder> addUniformTextureOutput(uint32_t, const std::shared_ptr<Texture>&);
            const std::map<uint32_t, std::shared_ptr<Texture>>& getUniformTextureOutputs() const;

            std::unique_ptr<GenericCompute> build();

        private:
            GenericComputeBuilder(std::string, RenderTarget&, const Shader&, const Vector2<int>&);

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;
            Vector2<int> threadLocalSize;

            std::map<uint32_t, ShaderDataContainer> uniformData;
            std::map<uint32_t, std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;
            std::map<uint32_t, std::shared_ptr<Texture>> uniformTextureOutputs;
    };

}
