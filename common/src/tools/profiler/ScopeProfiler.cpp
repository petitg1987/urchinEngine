#include "ScopeProfiler.h"

namespace urchin {
    ScopeProfiler::ScopeProfiler(const std::unique_ptr<Profiler>& profiler, const std::string& nodeName) :
            profiler(profiler),
            nodeName(nodeName) {
        profiler->startNewProfile(nodeName);
    }

    ScopeProfiler::~ScopeProfiler() {
        profiler->stopProfile(nodeName);
    }
}
