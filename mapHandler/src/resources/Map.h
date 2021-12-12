#pragma once

#include <string>
#include <list>
#include <utility>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>
#include <load/LoadMapCallback.h>
#include <resources/object/ObjectEntity.h>
#include <resources/light/LightEntity.h>
#include <resources/terrain/TerrainEntity.h>
#include <resources/water/WaterEntity.h>
#include <resources/sky/SkyEntity.h>
#include <resources/sound/SoundEntity.h>

namespace urchin {

    class Map {
        public:
            Map(Renderer3d*, PhysicsWorld*, SoundEnvironment*, AIEnvironment*);

            Renderer3d* getRenderer3d() const;
            PhysicsWorld* getPhysicsWorld() const;
            SoundEnvironment* getSoundEnvironment() const;
            AIEnvironment* getAIEnvironment() const;

            void setRelativeWorkingDirectory(const std::string&);
            const std::string& getRelativeWorkingDirectory() const;

            const std::list<std::unique_ptr<ObjectEntity>>& getObjectEntities() const;
            ObjectEntity& getObjectEntity(const std::string&) const;
            void findObjectEntitiesByTag(const std::string& tag, std::vector<ObjectEntity*>& models) const;
            void addObjectEntity(std::unique_ptr<ObjectEntity>);
            void removeObjectEntity(ObjectEntity&);

            const std::list<std::unique_ptr<LightEntity>>& getLightEntities() const;
            LightEntity& getLightEntity(const std::string&) const;
            void addLightEntity(std::unique_ptr<LightEntity>);
            void removeLightEntity(LightEntity&);

            const std::list<std::unique_ptr<TerrainEntity>>& getTerrainEntities() const;
            TerrainEntity& getTerrainEntity(const std::string&) const;
            void addTerrainEntity(std::unique_ptr<TerrainEntity>);
            void removeTerrainEntity(TerrainEntity&);

            const std::list<std::unique_ptr<WaterEntity>>& getWaterEntities() const;
            WaterEntity& getWaterEntity(const std::string&) const;
            void addWaterEntity(std::unique_ptr<WaterEntity>);
            void removeWaterEntity(WaterEntity&);

            const SkyEntity& getSkyEntity() const;
            void setSkyEntity(std::unique_ptr<SkyEntity>);

            const std::list<std::unique_ptr<SoundEntity>>& getSoundEntities() const;
            SoundEntity& getSoundEntity(const std::string&) const;
            void addSoundEntity(std::unique_ptr<SoundEntity>);
            void removeSoundEntity(SoundEntity&);

            void refresh();
            void pause();
            void unpause();

        private:
            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            SoundEnvironment* soundEnvironment;
            AIEnvironment* aiEnvironment;

            std::string relativeWorkingDirectory;

            TagHolder objectEntitiesTagHolder;
            std::list<std::unique_ptr<ObjectEntity>> objectEntities;
            std::list<std::unique_ptr<LightEntity>> lightEntities;
            std::list<std::unique_ptr<TerrainEntity>> terrainEntities;
            std::list<std::unique_ptr<WaterEntity>> waterEntities;
            std::unique_ptr<SkyEntity> skyEntity;
            std::list<std::unique_ptr<SoundEntity>> soundEntities;
    };

}
