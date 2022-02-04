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
            startCount(0),
            numberOfCall(0),
            totalTime(0.0) {

    }

    const std::string& ProfilerNode::getName() const {
        return name;
    }

    ProfilerNode* ProfilerNode::getParent() const {
        return parent;
    }

    ProfilerNode* ProfilerNode::findChildren(std::string_view name) const {
        for (const auto& child : children) {
            if (child->getName() == name) {
                return child.get();
            }
        }

        return nullptr;
    }

    void ProfilerNode::addChild(std::unique_ptr<ProfilerNode> child) {
        children.push_back(std::move(child));
    }

    bool ProfilerNode::isStarted() const {
        return startCount > 0;
    }

    void ProfilerNode::startTimer() {
        if (!isStarted()) { //not recursive call
            startTime = std::chrono::steady_clock::now();
        }

        startCount++;
    }

    bool ProfilerNode::stopTimer() {
        if (!isStarted()) {
            throw std::runtime_error("Profiler not started: " + name);
        }

        bool isStopped = false;
        if (startCount == 1) {
            auto endTime = std::chrono::steady_clock::now();
            double durationMs = static_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count();

            numberOfCall++;
            totalTime += durationMs;
            isStopped = true;
        }

        startCount--;

        return isStopped;
    }

    void ProfilerNode::log(unsigned int level, std::stringstream& logStream, double levelOneTotalTime) const {
        if (startCount != 0) {
            throw std::runtime_error("Impossible to print node " + getName() + " because there is " + std::to_string(startCount) + " missing stop call");
        }

        if (level == 1) {
            levelOneTotalTime = totalTime;
        }

        if (level > 0 && numberOfCall > 0) {
            double averageTime = totalTime / numberOfCall;
            double percentageTime = (totalTime / levelOneTotalTime) * 100.0;

            logStream << std::setw((int)level * 4 + 2) << " - " << name;
            logStream << " (average: " << averageTime <<"ms";
            logStream << ", total: " << totalTime / 1000.0 << "sec/" << percentageTime << "%";
            logStream << ", call: " << numberOfCall;

            if (!children.empty()) {
                double childTotalTime = 0.0;
                for (const auto& child : children) {
                    childTotalTime += child->totalTime;
                }
                double unTrackedTime = averageTime - (childTotalTime / numberOfCall);
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
