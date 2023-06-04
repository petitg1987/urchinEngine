#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/texture/TextureReader.h>

namespace urchin {

    class GenericComputeBuilder;

    class GenericCompute {
        public:
            explicit GenericCompute(const GenericComputeBuilder&);
            ~GenericCompute();

            const std::string& getName() const;
            const RenderTarget& getRenderTarget() const;

        private:
            void initialize();
            void cleanup();

            void createPipeline();
            void destroyPipeline();

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
    };

}
