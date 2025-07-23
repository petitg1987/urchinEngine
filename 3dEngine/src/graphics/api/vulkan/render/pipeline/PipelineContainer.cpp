#include <ranges>

#include "graphics/api/vulkan/render/pipeline/PipelineContainer.h"

namespace urchin {

    PipelineContainer& PipelineContainer::instance() {
        static PipelineContainer instance;
        return instance;
    }

    PipelineContainer::~PipelineContainer() {
        cleanPipelines();
        for (const std::shared_ptr<Pipeline>& pipeline : std::views::values(pipelines)) {
            Logger::instance().logError("Pipeline not released: " + pipeline->getName());
        }
    }

    std::shared_ptr<Pipeline> PipelineContainer::getPipeline(std::size_t pipelineHash) const {
        auto itFind = pipelines.find(pipelineHash);
        if (itFind != pipelines.end()) {
            return itFind->second;
        }
        return std::shared_ptr<Pipeline>(nullptr);
    }

    void PipelineContainer::addPipeline(const std::shared_ptr<Pipeline>& pipeline) {
        #ifdef URCHIN_DEBUG
            assert(!pipelines.contains(pipeline->getId()));
        #endif
        pipelines.try_emplace(pipeline->getId(), pipeline);
    }

    void PipelineContainer::cleanPipelines() {
        std::erase_if(pipelines, [](const auto& pipeline) {
            return pipeline.second.use_count() <= 1; //pipeline not used anymore
        });
    }

}