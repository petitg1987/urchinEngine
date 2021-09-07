#pragma once

#include <map>
#include <memory>
#include <UrchinCommon.h>

#include <graphic/render/pipeline/Pipeline.h>

namespace urchin {

    class PipelineContainer : public Singleton<PipelineContainer> {
        public:
            friend class Singleton<PipelineContainer>;

            ~PipelineContainer() override;

            std::shared_ptr<Pipeline> getPipeline(std::size_t) const;
            void addPipeline(std::size_t, const std::shared_ptr<Pipeline>&);
            void cleanPipelines();

        private:
            std::map<std::size_t, std::shared_ptr<Pipeline>> pipelines;
    };

}
