#pragma once

#include <string>
#include <list>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>

#include "model/ObjectEntity.h"
#include "model/TerrainEntity.h"
#include "model/WaterEntity.h"
#include "model/SkyEntity.h"
#include "model/NavMeshAgentEntity.h"

namespace urchin {

    class Map {
        public:
            Map(Renderer3d*, PhysicsWorld*, SoundEnvironment*, AIEnvironment*);

            Renderer3d* getRenderer3d() const;
            PhysicsWorld* getPhysicsWorld() const;
            SoundEnvironment* getSoundEnvironment() const;
            AIEnvironment* getAIEnvironment() const;

            void setRelativeWorkingDirectory(std::string);
            const std::string& getRelativeWorkingDirectory() const;
            void addUserData(const std::string&, const std::string&);
            const std::map<std::string, std::string>& getUserData() const;
            std::string getUserData(const std::string&) const;

            const std::list<std::unique_ptr<ObjectEntity>>& getObjectEntities() const;
            ObjectEntity& getObjectEntity(std::string_view) const;
            void findObjectEntitiesByTag(std::string_view, std::vector<ObjectEntity*>&) const;
            std::vector<ObjectEntity*> findObjectEntitiesByTag(std::string_view) const;
            ObjectEntity* findObjectEntityByTag(std::string_view) const;
            ObjectEntity& addObjectEntity(std::unique_ptr<ObjectEntity>);
            void removeObjectEntity(ObjectEntity&);

            const std::list<std::unique_ptr<TerrainEntity>>& getTerrainEntities() const;
            TerrainEntity& getTerrainEntity(std::string_view) const;
            TerrainEntity& addTerrainEntity(std::unique_ptr<TerrainEntity>);
            void removeTerrainEntity(TerrainEntity&);

            const std::list<std::unique_ptr<WaterEntity>>& getWaterEntities() const;
            WaterEntity& getWaterEntity(std::string_view) const;
            WaterEntity& addWaterEntity(std::unique_ptr<WaterEntity>);
            void removeWaterEntity(WaterEntity&);

            const SkyEntity& getSkyEntity() const;
            void setSkyEntity(std::unique_ptr<SkyEntity>);

            const NavMeshAgentEntity& getNavMeshAgentEntity() const;
            void setNavMeshAgentEntity(std::unique_ptr<NavMeshAgentEntity>);

            void refresh() const;
            void pause() const;
            void unpause() const;

        private:
            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            SoundEnvironment* soundEnvironment;
            AIEnvironment* aiEnvironment;

            std::string relativeWorkingDirectory;
            std::map<std::string, std::string> userData;

            TagHolder objectEntitiesTagHolder;
            mutable std::vector<ObjectEntity*> tmpObjectEntities;
            std::list<std::unique_ptr<ObjectEntity>> objectEntities;
            std::list<std::unique_ptr<TerrainEntity>> terrainEntities;
            std::list<std::unique_ptr<WaterEntity>> waterEntities;
            std::unique_ptr<SkyEntity> skyEntity;
            std::unique_ptr<NavMeshAgentEntity> navMeshAgentEntity;
    };

}
