#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/pipeline/Pipeline.h>
#include <graphics/api/vulkan/render/pipeline/PipelineBuilder.h>

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
            std::size_t getPipelineId() const;
            PipelineType getPipelineType() const;

            virtual std::span<OffscreenRender*> getTexturesWriter() const = 0;

        protected:
            void setupPipelineBuilder(std::unique_ptr<PipelineBuilder>);
            PipelineBuilder& getPipelineBuilder() const;
            virtual void createPipeline();
            void destroyPipeline();
            Pipeline& getPipeline() const;

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

            std::unique_ptr<PipelineBuilder> pipelineBuilder;
            std::shared_ptr<Pipeline> pipeline;
    };

}
