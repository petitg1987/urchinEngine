#include <iostream>
#include <sstream>
#include <cassert>
#include <thread>

#include "profiler/Profiler.h"
#include "config/ConfigService.h"
#include "logger/FileLogger.h"

namespace urchin {

    Profiler::Profiler(std::string instanceName) :
            isEnable(ConfigService::instance().getBoolValue("profiler." + instanceName + "Enable")),
            instanceName(std::move(instanceName)),
            currentNode(nullptr) {
        initializeOrReset();
    }

    void Profiler::initializeOrReset() {
        profilerRoot = std::make_unique<ProfilerNode>("root", nullptr);
        currentNode = profilerRoot.get();
    }

    Profiler& Profiler::graphic() {
        thread_local auto profiler3d = std::make_unique<Profiler>("3d");
        return *profiler3d;
    }

    Profiler& Profiler::physics() {
        thread_local auto profilerPhysics = std::make_unique<Profiler>("physics");
        return *profilerPhysics;
    }

    Profiler& Profiler::ai() {
        thread_local auto profilerAi = std::make_unique<Profiler>("ai");
        return *profilerAi;
    }

    Profiler& Profiler::sound() {
        thread_local auto profilerSound = std::make_unique<Profiler>("sound");
        return *profilerSound;
    }

    Profiler& Profiler::network() {
        thread_local auto profilerNetwork = std::make_unique<Profiler>("network");
        return *profilerNetwork;
    }

    void Profiler::startNewProfile(std::string_view nodeName) {
        if (isEnable) {
            assert(nodeName.length() <= 15); //ensure to use "small string optimization"

            if (currentNode->getName() == nodeName) {
                currentNode->startTimer();
            } else {
                ProfilerNode* profilerNode = currentNode->findChildren(nodeName);
                if (profilerNode == nullptr) {
                    auto newProfilerNode = std::make_unique<ProfilerNode>(std::string(nodeName), currentNode);
                    profilerNode = currentNode->addChild(std::move(newProfilerNode));
                }

                profilerNode->startTimer();
                currentNode = profilerNode;
            }
        }
    }

    void Profiler::stopProfile(std::string_view nodeName) {
        if (isEnable) {
            if (!nodeName.empty() && currentNode->getName() != nodeName) {
                throw std::runtime_error("Impossible to stop node '" + std::string(nodeName) + "' because current node is '" + currentNode->getName() + "'");
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
            if (currentNode != profilerRoot.get()) {
                throw std::runtime_error("Current node must be the root node to perform print. Current node: " + currentNode->getName());
            }

            std::stringstream logStream;
            logStream.precision(3);
            logStream << "Profiling result for " << instanceName << " on thread " << std::this_thread::get_id() << ":" << std::endl;
            profilerRoot->log(0, logStream, -1.0);

            auto profilerLogger = std::make_unique<FileLogger>("profiler.log");
            profilerLogger->logInfo(logStream.str());

            initializeOrReset();
        }
    }

}
