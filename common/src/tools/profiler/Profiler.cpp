#include <iostream>

#include "Profiler.h"
#include "tools/logger/Logger.h"
#include "tools/logger/FileLogger.h"

namespace urchin
{
    //static
    std::map<std::string, std::shared_ptr<Profiler>> Profiler::instances;

    Profiler::Profiler() :
            profilerRoot(new ProfilerNode("root", nullptr)),
            currentNode(profilerRoot)
    { //TODO allow to disable profiler

    }

    Profiler::~Profiler()
    {
        delete profilerRoot;
    }

    std::shared_ptr<Profiler> Profiler::getInstance(const std::string &instanceName)
    {
        auto instanceIt = instances.find(instanceName);
        if(instanceIt!=instances.end())
        {
            return instanceIt->second;
        }

        auto profiler = std::make_shared<Profiler>();
        instances.insert(std::pair<std::string, std::shared_ptr<Profiler>>(instanceName, profiler));
        return profiler;
    }

    void Profiler::startNewProfile(const std::string &nodeName)
    {
        if(currentNode->getName()==nodeName)
        {
            currentNode->startTimer();
        }else
        {
            ProfilerNode *profilerNode = currentNode->findChildren(nodeName);
            if(profilerNode==nullptr)
            {
                profilerNode = new ProfilerNode(nodeName, currentNode);
                currentNode->addChild(profilerNode);
            }

            profilerNode->startTimer();
            currentNode = profilerNode;
        }
    }

    void Profiler::stopProfile(const std::string &nodeName)
    {
        if(!nodeName.empty() && currentNode->getName()!=nodeName)
        {
            throw std::runtime_error("Impossible to stop node '" + nodeName + "' because current node is '" + currentNode->getName() + "'");
        }

        if(currentNode->getParent()==nullptr)
        {
            throw std::runtime_error("Current node is the root node: impossible to stop current profile");
        }

        bool isTimerStopped = currentNode->stopTimer();

        if(isTimerStopped)
        {
            currentNode = currentNode->getParent();
        }
    }

    void Profiler::log()
    {
        if(currentNode!=profilerRoot)
        {
            throw std::runtime_error("Current node must be the root node to perform print. Current node: " + currentNode->getName());
        }

        std::unique_ptr<Logger> oldLogger = Logger::defineLogger(std::make_unique<FileLogger>("perf.log"));
        std::stringstream logStream;
        logStream.precision(3);

        profilerRoot->log(0, logStream);

        Logger::logger().logInfo(logStream.str());
        Logger::defineLogger(std::move(oldLogger));
    }

}
