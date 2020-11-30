#ifndef URCHINENGINE_PROFILERNODE_H
#define URCHINENGINE_PROFILERNODE_H

#include <chrono>
#include <string>
#include <vector>

namespace urchin {

    class ProfilerNode {
        public:
            ProfilerNode(std::string , ProfilerNode*);
            ~ProfilerNode();

            const std::string& getName() const;

            ProfilerNode* getParent() const;

            std::vector<ProfilerNode*> getChildren() const;
            ProfilerNode* findChildren(const std::string&) const;
            void addChild(ProfilerNode*);

            bool isStarted();
            void startTimer();
            bool stopTimer();

            void log(unsigned int, std::stringstream&, double);

        private:
            double computeTotalTimes() const;
            int getNbValidTimes() const;

            std::string name;
            ProfilerNode* parent;
            std::vector<ProfilerNode*> children;

            unsigned int startCount;
            std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
            std::vector<double> times;
    };

}

#endif
