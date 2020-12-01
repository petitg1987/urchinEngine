#include <iostream>
#include <cassert>
#include <memory>

#include "Profiler.h"
#include "tools/ConfigService.h"
#include "tools/logger/FileLogger.h"

namespace urchin {

    Profiler::Profiler(const std::string& instanceName) :
            instanceName(instanceName),
            profilerRoot(new ProfilerNode("root", nullptr)),
            currentNode(profilerRoot) {
        std::string enableKey = "profiler." + instanceName + "Enable";
        isEnable = ConfigService::instance()->getBoolValue(enableKey);
    }

    Profiler::~Profiler() {
        delete profilerRoot;
    }

    const std::unique_ptr<Profiler>& Profiler::graphic() {
        static auto profiler3d = std::make_unique<Profiler>("3d");
        return profiler3d;
    }

    const std::unique_ptr<Profiler>& Profiler::physics() {
        static auto profilerPhysics = std::make_unique<Profiler>("physics");
        return profilerPhysics;
    }

    const std::unique_ptr<Profiler>& Profiler::ai() {
        static auto profilerAi = std::make_unique<Profiler>("ai");
        return profilerAi;
    }

    const std::unique_ptr<Profiler>& Profiler::sound() {
        static auto profilerSound = std::make_unique<Profiler>("sound");
        return profilerSound;
    }

    void Profiler::startNewProfile(const std::string& nodeName) {
        if (isEnable) {
            assert(nodeName.length() <= 15); //ensure to use "small string optimization"

            if (currentNode->getName() == nodeName) {
                currentNode->startTimer();
            } else {
                ProfilerNode* profilerNode = currentNode->findChildren(nodeName);
                if (profilerNode == nullptr) {
                    profilerNode = new ProfilerNode(nodeName, currentNode);
                    currentNode->addChild(profilerNode);
                }

                profilerNode->startTimer();
                currentNode = profilerNode;
            }
        }
    }

    void Profiler::stopProfile(const std::string& nodeName) {
        if (isEnable) {
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
        if (isEnable) {
            if (currentNode != profilerRoot) {
                throw std::runtime_error("Current node must be the root node to perform print. Current node: " + currentNode->getName());
            }

            std::stringstream logStream;
            logStream.precision(3);
            logStream << "Profiling result (" << instanceName << "):" << std::endl;
            profilerRoot->log(0, logStream, -1.0);

            auto profilerLogger = std::make_unique<FileLogger>("profiler.log");
            profilerLogger->logInfo(logStream.str());
        }
    }

}
