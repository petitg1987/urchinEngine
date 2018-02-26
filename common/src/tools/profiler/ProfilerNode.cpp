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
            isStarted(false)
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

    void ProfilerNode::startTimer()
    {
        if(isStarted)
        {
            throw std::runtime_error("Profiler already started: " + name);
        }

        startTime = std::chrono::high_resolution_clock::now();
        isStarted = true;
    }

    void ProfilerNode::stopTimer()
    {
        if(!isStarted)
        {
            throw std::runtime_error("Profiler not started: " + name);
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationMs = endTime - startTime;

        averageTimes.push_back(durationMs.count());
        isStarted = false;
    }

    void ProfilerNode::print(unsigned int level)
    {
        if(level > 0)
        {
            double average = std::accumulate(averageTimes.begin(), averageTimes.end(), 0.0) / averageTimes.size();
            std::cout << std::setw(level) << std::left << " - " << name << " (" << average << " ms)" << std::endl;
        }else
        {
            std::cout<<"Root profile:"<<std::endl;
        }

        for(const auto &child : children)
        {
            child->print(level + 4);
        }
    }
}
