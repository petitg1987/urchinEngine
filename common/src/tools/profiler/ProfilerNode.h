#ifndef URCHINENGINE_PROFILERNODE_H
#define URCHINENGINE_PROFILERNODE_H

#include <chrono>
#include <string>
#include <vector>

namespace urchin
{

    class ProfilerNode
    {
        public:
            ProfilerNode(const std::string &, ProfilerNode *);

            const std::string &getName() const;

            ProfilerNode *getParent() const;

            std::vector<ProfilerNode *> getChildren() const;
            ProfilerNode *findChildren(const std::string &) const;
            void addChild(ProfilerNode *);

            void startTimer();
            void stopTimer();

            void print(unsigned int);

        private:
            std::string name;
            ProfilerNode *parent;
            std::vector<ProfilerNode *> children;

            bool isStarted;
            std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
            std::vector<double> averageTimes;
    };

}

#endif
