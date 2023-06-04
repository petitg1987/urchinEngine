#pragma once

namespace urchin {

    class PipelineProcessor;

    struct PipelineProcessorComparator {
        bool operator()(const PipelineProcessor*, const PipelineProcessor*) const;
    };

}