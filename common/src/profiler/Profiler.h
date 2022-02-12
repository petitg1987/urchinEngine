#pragma once

#include <memory>
#include <map>
#include <stack>

#include <profiler/ProfilerNode.h>

namespace urchin {

    class Profiler {
        public:
            explicit Profiler(std::string);

            static Profiler& graphic();
            static Profiler& physics();
            static Profiler& ai();
            static Profiler& sound();

            void startNewProfile(const std::string&);
            void stopProfile(std::string_view nodeName = "");

            void log();

        private:
            void initializeOrReset();

            bool isEnable;
            std::string instanceName;

            std::unique_ptr<ProfilerNode> profilerRoot;
            ProfilerNode* currentNode;
    };

}
