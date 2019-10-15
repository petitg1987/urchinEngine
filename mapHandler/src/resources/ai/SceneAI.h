#ifndef URCHINENGINE_SCENEAI_H
#define URCHINENGINE_SCENEAI_H

#include <memory>

#include "UrchinAIEngine.h"

namespace urchin
{

    class SceneAI
    {
        //XML tags
        #define NAV_MESH_AGENT_TAG "navMeshAgent"

        public:
            friend class Map;

            SceneAI();

            void setAIManager(AIManager *);

            std::shared_ptr<NavMeshAgent> getNavMeshAgent() const;
            void changeNavMeshAgent(const std::shared_ptr<NavMeshAgent>&);

        private:
            void loadFrom(const std::shared_ptr<XmlChunk> &, const XmlParser &);
            void writeOn(const std::shared_ptr<XmlChunk> &, XmlWriter &) const;

            void setNavMeshAgent(const std::shared_ptr<NavMeshAgent>&);

            AIManager *aiManager;
            std::shared_ptr<NavMeshAgent> navMeshAgent;
    };

}

#endif
