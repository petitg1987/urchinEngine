#pragma once

#include <string>
#include <memory>

#include <profiler/Profiler.h>

namespace urchin {

    class ScopeProfiler {
        public:
            ScopeProfiler(const std::unique_ptr<Profiler>&, const std::string&);
            ~ScopeProfiler();

        private:
            const std::unique_ptr<Profiler>& profiler;
            std::string nodeName;
    };

}
