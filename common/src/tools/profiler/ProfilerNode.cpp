#include <stdexcept>
#include <iostream>
#include <numeric>
#include <iomanip>

#include "ProfilerNode.h"

namespace urchin
{
    ProfilerNode::ProfilerNode(const std::string &name, ProfilerNode *parent) :
            name(name),
            parent(parent),
            isStarted(false),
            ignoreStop(0)
    {

    }

    const std::string &ProfilerNode::getName() const
    {
        return name;
    }

    ProfilerNode *ProfilerNode::getParent() const
    {
        return parent;
    }

    std::vector<ProfilerNode *> ProfilerNode::getChildren() const
    {
        return children;
    }

    ProfilerNode *ProfilerNode::findChildren(const std::string &name) const
    {
        for(const auto &child : children)
        {
            if(child->getName() == name)
            {
                return child;
            }
        }

        return nullptr;
    }

    void ProfilerNode::addChild(ProfilerNode *child)
    {
        children.push_back(child);
    }

    bool ProfilerNode::startTimer(bool accumulateTime)
    {
        bool isTimerStarted = false;

        if(isStarted)
        { //recursive call (ignore)
            ignoreStop++;
        }else
        {
            startTime = std::chrono::high_resolution_clock::now();
            isStarted = true;
            needAccumulateTime = accumulateTime;

            isTimerStarted = true;
        }

        return isTimerStarted;
    }

    bool ProfilerNode::stopTimer()
    {
        bool isTimerStopped = false;

        if(!isStarted)
        {
            throw std::runtime_error("Profiler not started: " + name);
        }else if(ignoreStop > 0)
        {
            ignoreStop--;
        }else
        {
            auto endTime = std::chrono::high_resolution_clock::now();
            double durationMs = static_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

            if (needAccumulateTime)
            {
                averageTimes[averageTimes.size() - 1] += durationMs;
            } else
            {
                averageTimes.push_back(durationMs);
            }

            isStarted = false;
            isTimerStopped = true;
        }

        return isTimerStopped;
    }

    double ProfilerNode::computeAverageTime() const
    {
        return std::accumulate(averageTimes.begin(), averageTimes.end(), 0.0) / averageTimes.size();
    }

    void ProfilerNode::print(unsigned int level)
    {
        if(ignoreStop!=0) //TODO: do it better (startCount ?)
        {
            throw std::runtime_error("errrrr"); //TODO ...
        }

        if(level > 0)
        {
            double averageTime = computeAverageTime();
            double childAverageTime = 0.0;
            for(const auto &child : children)
            {
                childAverageTime += child->computeAverageTime();
            }
            double unTrackedTime = averageTime - childAverageTime; //TODO wrong computation

            std::cout << std::setw(level) << std::left << " - " << name
                      << " (time: " << computeAverageTime() <<"ms"
                      << ", call: " << averageTimes.size()
                      << ", un-tracked: " << (children.empty() ? "N/A" : std::to_string(unTrackedTime))
                      << ")" << std::endl;
        }else
        {
            std::cout<<"root profile:"<<std::endl;
        }

        for(const auto &child : children)
        {
            child->print(level + 4);
        }
    }
}
