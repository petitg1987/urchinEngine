#pragma once

#include <string>
#include <list>
#include <utility>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>
#include <load/LoadMapCallback.h>
#include <resources/model/ObjectEntity.h>
#include <resources/light/LightEntity.h>
#include <resources/terrain/TerrainEntity.h>
#include <resources/water/WaterEntity.h>
#include <resources/sky/SkyEntity.h>
#include <resources/sound/SoundEntity.h>

namespace urchin {

    /**
    * Represent a map loaded in memory
    */
    class Map {
        public:
            friend class MapHandler;

            Map(Renderer3d*, PhysicsWorld*, SoundEnvironment*, AIEnvironment*);

            Renderer3d* getRenderer3d() const;
            PhysicsWorld* getPhysicsWorld() const;
            SoundEnvironment* getSoundEnvironment() const;
            AIEnvironment* getAIEnvironment() const;

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
            void updateSkyEntity(std::unique_ptr<Skybox> skybox);

            const std::list<std::unique_ptr<SoundEntity>>& getSoundEntities() const;
            SoundEntity& getSoundEntity(const std::string&) const;
            void addSoundEntity(std::unique_ptr<SoundEntity>);
            void removeSoundEntity(SoundEntity&);

            void refresh();

            void pause();
            void unpause();

        private:
            void loadFrom(const UdaChunk*, const UdaParser&, LoadMapCallback&);
            void loadObjectEntities(const UdaChunk*, const UdaParser&);
            void loadLightEntities(const UdaChunk*, const UdaParser&);
            void loadTerrainEntities(const UdaChunk*, const UdaParser&);
            void loadWaterEntities(const UdaChunk*, const UdaParser&);
            void loadSkyEntity(const UdaChunk*, const UdaParser&);
            void loadSoundEntities(const UdaChunk*, const UdaParser&);
            void loadAIConfig(const UdaChunk*, const UdaParser&);

            void writeOn(UdaChunk&, UdaWriter&) const;
            void writeObjectEntities(UdaChunk&, UdaWriter&) const;
            void writeLightEntities(UdaChunk&, UdaWriter&) const;
            void writeTerrainEntities(UdaChunk&, UdaWriter&) const;
            void writeWaterEntities(UdaChunk&, UdaWriter&) const;
            void writeSkyEntity(UdaChunk&, UdaWriter&) const;
            void writeSoundEntities(UdaChunk&, UdaWriter&) const;
            void writeAIConfig(UdaChunk&, UdaWriter&) const;

            static constexpr char MODELS_TAG[] = "models"; //TODO rename
            static constexpr char MODEL_TAG[] = "model";
            static constexpr char LIGHTS_TAG[] = "lights";
            static constexpr char LIGHT_TAG[] = "light";
            static constexpr char TERRAINS_TAG[] = "terrains";
            static constexpr char TERRAIN_TAG[] = "terrain";
            static constexpr char WATERS_TAG[] = "waters";
            static constexpr char WATER_TAG[] = "water";
            static constexpr char SKY_TAG[] = "sky";
            static constexpr char SOUND_ELEMENTS_TAG[] = "soundElements";
            static constexpr char SOUND_ELEMENT_TAG[] = "soundElement";
            static constexpr char AI_ELEMENTS_TAG[] = "aiElements";

            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            SoundEnvironment* soundEnvironment;
            AIEnvironment* aiEnvironment;

            TagHolder objectEntitiesTagHolder;
            std::list<std::unique_ptr<ObjectEntity>> objectEntities;
            std::list<std::unique_ptr<LightEntity>> lightEntities;
            std::list<std::unique_ptr<TerrainEntity>> terrainEntities;
            std::list<std::unique_ptr<WaterEntity>> waterEntities;
            SkyEntity skyEntity;
            std::list<std::unique_ptr<SoundEntity>> soundEntities;
    };

}
