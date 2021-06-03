#include <stdexcept>
#include <iostream>
#include <numeric>
#include <iomanip>
#include <utility>

#include <profiler/ProfilerNode.h>

namespace urchin {
    ProfilerNode::ProfilerNode(std::string name, ProfilerNode* parent) :
            name(std::move(name)),
            parent(parent),
            startCount(0) {

    }

    ProfilerNode::~ProfilerNode() {
        for (ProfilerNode* child : children) {
            delete child;
        }
    }

    const std::string& ProfilerNode::getName() const {
        return name;
    }

    ProfilerNode* ProfilerNode::getParent() const {
        return parent;
    }

    std::vector<ProfilerNode*> ProfilerNode::getChildren() const {
        return children;
    }

    ProfilerNode* ProfilerNode::findChildren(const std::string& name) const {
        for (const auto& child : children) {
            if (child->getName() == name) {
                return child;
            }
        }

        return nullptr;
    }

    void ProfilerNode::addChild(ProfilerNode* child) {
        children.push_back(child);
    }

    bool ProfilerNode::isStarted() const {
        return startCount > 0;
    }

    void ProfilerNode::startTimer() {
        if (!isStarted()) { //not recursive call
            startTime = std::chrono::high_resolution_clock::now();
        }

        startCount++;
    }

    bool ProfilerNode::stopTimer() {
        if (!isStarted()) {
            throw std::runtime_error("Profiler not started: " + name);
        }

        bool isStopped = false;
        if (startCount == 1) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double durationMs = static_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

            times.push_back(durationMs);
            isStopped = true;
        }

        startCount--;

        return isStopped;
    }

    double ProfilerNode::computeTotalTimes() const { //remove first element (avoid counting time for potential initialization process)
        return std::accumulate(times.begin() + 1, times.end(), 0.0);
    }

    int ProfilerNode::getNbValidTimes() const { //remove first element (avoid counting time for potential initialization process)
        return (int)times.size() - 1;
    }

    void ProfilerNode::log(unsigned int level, std::stringstream& logStream, double levelOneTotalTime) {
        if (startCount != 0) {
            throw std::runtime_error("Impossible to print node " + getName() + " because there is " + std::to_string(startCount) + " missing stop call");
        }

        if (level == 1) {
            levelOneTotalTime = computeTotalTimes();
        }

        if (level > 0) {
            double totalTime = computeTotalTimes();
            double averageTime = totalTime / getNbValidTimes();
            double percentageTime = (totalTime / levelOneTotalTime) * 100.0;

            logStream << std::setw((int)level * 4) << " - " << name;
            logStream << " (average: " << averageTime <<"ms";
            logStream << ", total: " << totalTime / 1000.0 << "sec/" << percentageTime << "%";
            logStream << ", call: " << getNbValidTimes();

            if (!children.empty()) {
                double childTotalTime = 0.0;
                for (const auto& child : children) {
                    childTotalTime += child->computeTotalTimes();
                }
                double unTrackedTime = averageTime - (childTotalTime / getNbValidTimes());
                double unTrackedPercentageTime = (unTrackedTime / averageTime) * 100.0;

                logStream << ", un-tracked: " << std::to_string(unTrackedTime) << "ms/" << unTrackedPercentageTime << "%";
            }

            logStream << ")" << std::endl;
        }

        for (const auto& child : children) {
            child->log(level + 1, logStream, levelOneTotalTime);
        }
    }
}
