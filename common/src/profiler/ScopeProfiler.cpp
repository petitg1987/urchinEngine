#include <profiler/ScopeProfiler.h>

namespace urchin {
    ScopeProfiler::ScopeProfiler(Profiler& profiler, const std::string& nodeName) :
            profiler(profiler),
            nodeName(nodeName) {
        profiler.startNewProfile(nodeName);
    }

    ScopeProfiler::~ScopeProfiler() {
        profiler.stopProfile(nodeName);
    }
}
