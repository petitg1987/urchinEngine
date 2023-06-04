#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/pipeline/Pipeline.h>

namespace urchin {

    class PipelineProcessor { //TODO implement some method common between both impl
        public:
            friend class RenderTarget;

            PipelineProcessor(std::string, RenderTarget&, const Shader&);
            virtual ~PipelineProcessor();

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
            virtual PipelineType getPipelineType() const = 0;

            virtual std::span<OffscreenRender*> getTexturesWriter() const = 0;

        private:
            virtual void initialize() = 0;
            virtual void cleanup() = 0;

            virtual void updateGraphicData(uint32_t) = 0;
            virtual std::size_t updateCommandBuffer(VkCommandBuffer, std::size_t, std::size_t) = 0;

            std::string name;
            RenderTarget& renderTarget;
            const Shader& shader;

            bool bIsEnabled;
            unsigned int renderingOrder;
    };

}
