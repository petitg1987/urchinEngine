#ifndef URCHINENGINE_PROFILER_H
#define URCHINENGINE_PROFILER_H

#include <memory>
#include <map>
#include <stack>

#include "tools/profiler/ProfilerNode.h"

namespace urchin {

    class Profiler {
        public:
            explicit Profiler(const std::string&);
            ~Profiler();

            static std::shared_ptr<Profiler> getInstance(const std::string&);

            void startNewProfile(const std::string&);
            void stopProfile(const std::string &nodeName = "");

            void log();

        private:
            static std::map<std::string, std::shared_ptr<Profiler>> instances;

            bool isEnable;
            std::string instanceName;

            ProfilerNode *profilerRoot;
            ProfilerNode *currentNode;
    };

}

#endif
