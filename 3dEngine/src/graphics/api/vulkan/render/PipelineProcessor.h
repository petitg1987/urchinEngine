#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>

namespace urchin {

    class PipelineProcessor { //TODO implement some method common between both impl
        public:
            friend class RenderTarget;

            PipelineProcessor(std::string, RenderTarget&, const Shader&);
            virtual ~PipelineProcessor() = default;

            virtual bool isCompute() const = 0;
            const std::string& getName() const;
            const RenderTarget& getRenderTarget() const;
            const Shader& getShader() const;
            virtual bool needCommandBufferRefresh(std::size_t) const = 0;

            bool isEnabled() const;
            void enableRenderer(unsigned int);
            void disableRenderer();
            unsigned int getRenderingOrder() const;

            virtual bool isDepthTestEnabled() const = 0;
            virtual std::size_t getPipelineId() const = 0;

            virtual std::span<OffscreenRender*> getTexturesWriter() const = 0;

        protected:
            RenderTarget& renderTarget();

        private:
            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            virtual void updateGraphicData(uint32_t) = 0;
            virtual std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) = 0;

            std::string mName;
            RenderTarget& mRenderTarget;
            const Shader& mShader;

            bool bIsEnabled;
            unsigned int renderingOrder;
    };

}
