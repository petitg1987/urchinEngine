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
            startCount(0)
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

    bool ProfilerNode::isStarted()
    {
        return startCount > 0;
    }

    void ProfilerNode::startTimer()
    {
        if(!isStarted())
        { //not recursive call
            startTime = std::chrono::high_resolution_clock::now();
        }

        startCount++;
    }

    bool ProfilerNode::stopTimer()
    {
        if(!isStarted())
        {
            throw std::runtime_error("Profiler not started: " + name);
        }

        bool isStopped = false;
        if(startCount==1)
        {
            auto endTime = std::chrono::high_resolution_clock::now();
            double durationMs = static_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

            times.push_back(durationMs);
            isStopped = true;
        }

        startCount--;

        return isStopped;
    }

    double ProfilerNode::computeTotalTimes() const
    {
        return std::accumulate(times.begin(), times.end(), 0.0);
    }

    double ProfilerNode::computeAverageTime() const
    {
        return computeTotalTimes() / times.size();
    }

    void ProfilerNode::log(unsigned int shiftLevel, std::stringstream &logStream)
    {
        if(startCount!=0)
        {
            throw std::runtime_error("Impossible to print node " + getName() + " because there is " + std::to_string(startCount) + " missing stop call");
        }

        if(shiftLevel > 0)
        {
            double averageTime = computeAverageTime();

            logStream << std::setw(shiftLevel) << " - " << name
                      << " (time: " << computeAverageTime() <<"ms"
                      << ", call: " << times.size();
            if(!children.empty())
            {
                double childTotalTime = 0.0;
                for(const auto &child : children)
                {
                    childTotalTime += child->computeTotalTimes();
                }
                double unTrackedTime = averageTime - (childTotalTime / times.size());

                logStream << ", un-tracked: " << std::to_string(unTrackedTime) << "ms";
            }
            logStream << ")" <<std::endl;
        }else
        {
            logStream << "Profiling result:" << std::endl;
        }

        for(const auto &child : children)
        {
            child->log(shiftLevel + 4, logStream);
        }
    }
}
