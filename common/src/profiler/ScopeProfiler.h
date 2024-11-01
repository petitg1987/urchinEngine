#pragma once

#include <string>

#include <profiler/Profiler.h>

namespace urchin {

    class ScopeProfiler {
        public:
            ScopeProfiler(Profiler&, std::string);
            ~ScopeProfiler();

        private:
            Profiler& profiler;
            std::string nodeName;
    };

}
