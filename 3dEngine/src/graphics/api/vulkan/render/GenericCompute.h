#pragma once

#include "graphics/api/vulkan/render/PipelineProcessor.h"

namespace urchin {

    class GenericComputeBuilder;
    class TextureReader;

    class GenericCompute final : public PipelineProcessor {
        public:
            explicit GenericCompute(const GenericComputeBuilder&);
            ~GenericCompute() override;

            bool isGraphicsProcessor() const override;
            bool isComputeProcessor() const override;
            bool isDepthTestEnabled() const override;

        private:
            void initialize() override;
            void cleanup() override;

            bool isApplicableOnLayer(std::size_t) const override;
            std::size_t doUpdateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t, std::size_t) override;

            bool isInitialized;

            Vector2<int> threadLocalSize;
    };

}
