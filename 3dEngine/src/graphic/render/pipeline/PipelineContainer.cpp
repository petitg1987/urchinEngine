#include <graphic/render/pipeline/PipelineContainer.h>

namespace urchin {

    PipelineContainer::~PipelineContainer() {
        cleanPipelines();
        for (auto& pipeline : pipelines) {
            Logger::instance().logError("Pipeline not released: " + pipeline.second->name);
        }
    }

    std::shared_ptr<Pipeline> PipelineContainer::getPipeline(std::size_t pipelineHash) const {
        auto itFind = pipelines.find(pipelineHash);
        if (itFind != pipelines.end()) {
            return itFind->second;
        }
        return std::shared_ptr<Pipeline>(nullptr);
    }

    void PipelineContainer::addPipeline(std::size_t pipelineHash, const std::shared_ptr<Pipeline>& pipeline) {
        #ifndef NDEBUG
            assert(pipelines.find(pipelineHash) == pipelines.end());
        #endif
        pipelines.emplace(pipelineHash, pipeline);
    }

    void PipelineContainer::cleanPipelines() {
        for (auto it = pipelines.begin(); it != pipelines.end();) {
            if (it->second.use_count() <= 1) { //pipeline not used anymore
                it = pipelines.erase(it);
            } else {
                ++it;
            }
        }
    }

}