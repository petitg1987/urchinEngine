#include "SceneAI.h"
#include "resources/ai/NavMeshConfigWriter.h"

namespace urchin
{

    SceneAI::SceneAI() :
            aiManager(nullptr)
    {

    }

    void SceneAI::setAIManager(AIManager *aiManager)
    {
        this->aiManager = aiManager;

        if(aiManager)
        {
            aiManager->getNavMeshGenerator()->setNavMeshConfig(navMeshConfig);
        }
    }

    std::shared_ptr<NavMeshConfig> SceneAI::getNavMeshConfig() const
    {
        return navMeshConfig;
    }

    void SceneAI::changeNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig)
    {
        setNavMeshConfig(std::move(navMeshConfig));
    }

    void SceneAI::loadFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
    {
        std::shared_ptr<XmlChunk> navMeshConfigChunk = xmlParser.getUniqueChunk(true, NAV_MESH_CONFIG_TAG, XmlAttribute(), chunk);

        setNavMeshConfig(NavMeshConfigWriter().loadFrom(navMeshConfigChunk, xmlParser));
    }

    void SceneAI::writeOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> navMeshConfigChunk = xmlWriter.createChunk(NAV_MESH_CONFIG_TAG, XmlAttribute(), chunk);

        NavMeshConfigWriter().writeOn(navMeshConfigChunk, navMeshConfig, xmlWriter);
    }

    void SceneAI::setNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig)
    {
        this->navMeshConfig = navMeshConfig;

        if(aiManager)
        {
            aiManager->getNavMeshGenerator()->setNavMeshConfig(navMeshConfig);
        }
    }
}
