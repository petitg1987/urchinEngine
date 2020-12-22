#ifndef URCHINENGINE_SCOPEPROFILER_H
#define URCHINENGINE_SCOPEPROFILER_H

#include <string>
#include <memory>

#include "profiler/Profiler.h"

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

#endif
