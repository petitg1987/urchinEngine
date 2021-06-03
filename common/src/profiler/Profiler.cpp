#include <iostream>
#include <cassert>
#include <memory>
#include <thread>

#include <profiler/Profiler.h>
#include <config/ConfigService.h>
#include <logger/FileLogger.h>

namespace urchin {

    Profiler::Profiler(const std::string& instanceName) :
            isEnable(ConfigService::instance()->getBoolValue("profiler." + instanceName + "Enable")),
            instanceName(instanceName),
            profilerRoot(new ProfilerNode("root", nullptr)),
            currentNode(profilerRoot) {

    }

    Profiler::~Profiler() {
        delete profilerRoot;
    }

    const std::unique_ptr<Profiler>& Profiler::graphic() {
        static thread_local auto profiler3d = std::make_unique<Profiler>("3d");
        return profiler3d;
    }

    const std::unique_ptr<Profiler>& Profiler::physics() {
        static thread_local auto profilerPhysics = std::make_unique<Profiler>("physics");
        return profilerPhysics;
    }

    const std::unique_ptr<Profiler>& Profiler::ai() {
        static thread_local auto profilerAi = std::make_unique<Profiler>("ai");
        return profilerAi;
    }

    const std::unique_ptr<Profiler>& Profiler::sound() {
        static thread_local auto profilerSound = std::make_unique<Profiler>("sound");
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
            logStream << "Profiling result for " << instanceName << " on thread " << std::this_thread::get_id() << ":" << std::endl;
            profilerRoot->log(0, logStream, -1.0);

            auto profilerLogger = std::make_unique<FileLogger>("profiler.log");
            profilerLogger->logInfo(logStream.str());
        }
    }

}
