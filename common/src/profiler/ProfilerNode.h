#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace urchin {

    class ProfilerNode {
        public:
            ProfilerNode(std::string, ProfilerNode*);

            const std::string& getName() const;

            ProfilerNode* getParent() const;

            ProfilerNode* findChildren(std::string_view) const;
            ProfilerNode* addChild(std::unique_ptr<ProfilerNode>);

            bool isStarted() const;
            void startTimer();
            bool stopTimer();

            void log(unsigned int, std::stringstream&, double) const;

        private:
            std::string name;
            ProfilerNode* parent;
            std::vector<std::unique_ptr<ProfilerNode>> children;

            unsigned int startCount;
            std::chrono::steady_clock::time_point startTime;
            unsigned int numberOfCall;
            double totalTime;
    };

}
