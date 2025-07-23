#pragma once

#include <map>
#include <memory>

#include "graphics/api/vulkan/render/pipeline/Pipeline.h"

namespace urchin {

    class PipelineContainer {
        public:
            static PipelineContainer& instance();
            ~PipelineContainer();

            std::shared_ptr<Pipeline> getPipeline(std::size_t) const;
            void addPipeline(const std::shared_ptr<Pipeline>&);
            void cleanPipelines();

        private:
            std::map<std::size_t, std::shared_ptr<Pipeline>> pipelines;
    };

}
