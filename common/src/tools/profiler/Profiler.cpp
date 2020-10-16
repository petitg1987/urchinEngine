#include <iostream>
#include <cassert>

#include "Profiler.h"
#include "tools/ConfigService.h"
#include "tools/logger/Logger.h"
#include "tools/logger/FileLogger.h"

namespace urchin {
    //static
    std::map<std::string, std::shared_ptr<Profiler>> Profiler::instances;

    Profiler::Profiler(const std::string &instanceName) :
            instanceName(instanceName),
            profilerRoot(new ProfilerNode("root", nullptr)),
            currentNode(profilerRoot) {
        std::string enableKey = "profiler." + instanceName + "Enable";
        isEnable = ConfigService::instance()->getBoolValue(enableKey);
    }

    Profiler::~Profiler() {
        delete profilerRoot;
    }

    std::shared_ptr<Profiler> Profiler::getInstance(const std::string &instanceName) {
        auto instanceIt = instances.find(instanceName);
        if(instanceIt!=instances.end()) {
            return instanceIt->second;
        }

        auto profiler = std::make_shared<Profiler>(instanceName);
        instances.insert(std::pair<std::string, std::shared_ptr<Profiler>>(instanceName, profiler));
        return profiler;
    }

    void Profiler::startNewProfile(const std::string &nodeName) {
        if(isEnable) {
            assert(nodeName.length() <= 15); //ensure to use "small string optimization"

            if (currentNode->getName() == nodeName) {
                currentNode->startTimer();
            } else {
                ProfilerNode *profilerNode = currentNode->findChildren(nodeName);
                if (profilerNode == nullptr) {
                    profilerNode = new ProfilerNode(nodeName, currentNode);
                    currentNode->addChild(profilerNode);
                }

                profilerNode->startTimer();
                currentNode = profilerNode;
            }
        }
    }

    void Profiler::stopProfile(const std::string &nodeName) {
        if(isEnable) {
            if (!nodeName.empty() && currentNode->getName() != nodeName) {
                throw std::runtime_error("Impossible to stop node '" + nodeName + "' because current node is '" + currentNode->getName() + "'");
            }

            if (currentNode->getParent() == nullptr) {
                throw std::runtime_error("Current node is the root node: impossible to stop current profile");
            }

            bool isTimerStopped = currentNode->stopTimer();

            if (isTimerStopped) {
                currentNode = currentNode->getParent();
            }
        }
    }

    void Profiler::log() {
        if(isEnable) {
            if (currentNode != profilerRoot) {
                throw std::runtime_error("Current node must be the root node to perform print. Current node: " + currentNode->getName());
            }

            std::unique_ptr<Logger> oldLogger = Logger::defineLogger(std::make_unique<FileLogger>("profiler.log"));
            std::stringstream logStream;
            logStream.precision(3);

            logStream << "Profiling result (" << instanceName << "):" << std::endl;
            profilerRoot->log(0, logStream, -1.0);

            Logger::logger().logInfo(logStream.str());
            Logger::defineLogger(std::move(oldLogger));
        }
    }

}
