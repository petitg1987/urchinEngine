#ifndef URCHINENGINE_SCOPEPROFILER_H
#define URCHINENGINE_SCOPEPROFILER_H

#include <string>

namespace urchin
{

    class ScopeProfiler
    {
        public:
            ScopeProfiler(const std::string &, const std::string &);
            ~ScopeProfiler();

        private:
            std::string instanceName;
            std::string nodeName;
    };

}

#endif
