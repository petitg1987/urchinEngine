#include "Profiler.h"

namespace urchin
{
    //static
    std::map<std::string, std::shared_ptr<Profiler>> Profiler::instances;

    Profiler::Profiler() : //TODO create ScopeProfile class and use it
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
        ProfilerNode *profilerNode = currentNode->findChildren(nodeName);
        if(profilerNode!=nullptr)
        {
            profilerNode->startTimer();
        }else
        {
            profilerNode = new ProfilerNode(nodeName, currentNode);
            currentNode->addChild(profilerNode);
            profilerNode->startTimer();
        }

        currentNode = profilerNode;
    }

    void Profiler::stopCurrentProfile()
    {
        currentNode->stopTimer();

        currentNode = currentNode->getParent();
    }

    void Profiler::print()
    {
        if(currentNode!=profilerRoot)
        {
            throw std::runtime_error("Current node must be the root node to perform print: " + currentNode->getName());
        }

        profilerRoot->print(0);
    }

}
