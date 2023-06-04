#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>

namespace urchin {

    class PipelineProcessor { //TODO implement some method common between both impl
        public:
            friend class RenderTarget;

            virtual ~PipelineProcessor() = default;

            virtual bool isCompute() const = 0;
            virtual const std::string& getName() const = 0;
            virtual const RenderTarget& getRenderTarget() const = 0;
            virtual bool needCommandBufferRefresh(std::size_t) const = 0;

            virtual bool isEnabled() const = 0;
            virtual void enableRenderer(unsigned int) = 0;
            virtual void disableRenderer() = 0;

            virtual unsigned int getRenderingOrder() const = 0;
            virtual bool isDepthTestEnabled() const = 0;
            virtual std::size_t getPipelineId() const = 0;

            virtual std::span<OffscreenRender*> getTexturesWriter() const = 0;

        private:
            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            virtual void updateGraphicData(uint32_t) = 0;
            virtual std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) = 0;
    };

}
