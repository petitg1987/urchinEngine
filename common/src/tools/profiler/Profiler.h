#ifndef URCHINENGINE_PROFILER_H
#define URCHINENGINE_PROFILER_H

#include <memory>
#include <map>
#include <stack>

#include "tools/profiler/ProfilerNode.h"

namespace urchin
{

    class Profiler
    {
        public:
            Profiler();
            ~Profiler();

            static std::shared_ptr<Profiler> getInstance(const std::string &);

            void startNewProfile(const std::string &);
            void stopProfile(const std::string &nodeName = "");

            void log();

        private:
            static std::map<std::string, std::shared_ptr<Profiler>> instances;

            ProfilerNode *profilerRoot;
            ProfilerNode *currentNode;
    };

}

#endif
