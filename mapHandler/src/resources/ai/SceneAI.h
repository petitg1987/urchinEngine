#ifndef ENGINE_SCENEAI_H
#define ENGINE_SCENEAI_H

#include <memory>

#include "UrchinAIEngine.h"

namespace urchin
{

    class SceneAI
    {
        //XML tags
        #define NAV_MESH_CONFIG_TAG "navMeshConfig"

        public:
            friend class Map;

            SceneAI();
            ~SceneAI();

            void setAIManager(AIManager *);

            std::shared_ptr<NavMeshConfig> getNavMeshConfig() const;
            void changeNavMeshConfig(std::shared_ptr<NavMeshConfig>);

        private:
            void loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
            void writeOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;

            void setNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig);

            AIManager *aiManager;
            std::shared_ptr<NavMeshConfig> navMeshConfig;
    };

}

#endif
