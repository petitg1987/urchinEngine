#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/PipelineProcessor.h>

namespace urchin {

    class GenericComputeBuilder;
    class TextureReader;

    class GenericCompute : public PipelineProcessor {
        public:
            explicit GenericCompute(const GenericComputeBuilder&);
            ~GenericCompute();

            const std::string& getName() const override;
            const RenderTarget& getRenderTarget() const override;
            bool needCommandBufferRefresh(std::size_t) const override;

            bool isEnabled() const override;
            void enableRenderer(unsigned int) override;
            void disableRenderer() override;

            unsigned int getRenderingOrder() const override;
            bool isDepthTestEnabled() const override;
            std::size_t getPipelineId() const override;

            std::span<OffscreenRender*> getTexturesWriter() const override;

        private:
            void initialize() override;
            void cleanup() override;

            void createPipeline();
            void destroyPipeline();

            void updateGraphicData(uint32_t) override;
            std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) override;

            bool isInitialized;
            bool bIsEnabled;
            unsigned int renderingOrder;

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;

            std::vector<std::vector<std::shared_ptr<TextureReader>>> uniformTextureReaders;

            VkDescriptorSetLayout computeDescriptorSetLayout;
            VkPipelineLayout computePipelineLayout;
            VkPipeline computePipeline; //TODO use Pipeline object ?

            bool drawCommandsDirty;
    };

}
