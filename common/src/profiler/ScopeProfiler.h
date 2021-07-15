#pragma once

#include <string>
#include <memory>

#include <profiler/Profiler.h>

namespace urchin {

    class ScopeProfiler {
        public:
            ScopeProfiler(Profiler&, const std::string&);
            ~ScopeProfiler();

        private:
            Profiler& profiler;
            std::string nodeName;
    };

}
