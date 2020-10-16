#include "ScopeProfiler.h"
#include "tools/profiler/Profiler.h"

namespace urchin {
    ScopeProfiler::ScopeProfiler(const std::string &instanceName, const std::string &nodeName) :
            instanceName(instanceName),
            nodeName(nodeName) {
        Profiler::getInstance(instanceName)->startNewProfile(nodeName);
    }

    ScopeProfiler::~ScopeProfiler() {
        Profiler::getInstance(instanceName)->stopProfile(nodeName);
    }
}
