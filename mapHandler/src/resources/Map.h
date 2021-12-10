#pragma once

#include <string>
#include <list>
#include <utility>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>
#include <load/LoadMapCallback.h>
#include <resources/model/SceneModel.h>
#include <resources/light/SceneLight.h>
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

            const std::list<std::unique_ptr<SceneModel>>& getSceneModels() const;
            SceneModel& getSceneModel(const std::string&) const;
            void findSceneModelsByTag(const std::string& tag, std::vector<SceneModel*>& models) const;
            void addSceneModel(std::unique_ptr<SceneModel>);
            void removeSceneModel(SceneModel&);

            const std::list<std::unique_ptr<SceneLight>>& getSceneLights() const;
            SceneLight& getSceneLight(const std::string&) const;
            void addSceneLight(std::unique_ptr<SceneLight>);
            void removeSceneLight(SceneLight&);

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
            void loadSceneModelsFrom(const UdaChunk*, const UdaParser&);
            void loadSceneLightsFrom(const UdaChunk*, const UdaParser&);
            void loadTerrainEntities(const UdaChunk*, const UdaParser&);
            void loadWaterEntities(const UdaChunk*, const UdaParser&);
            void loadSkyEntity(const UdaChunk*, const UdaParser&);
            void loadSoundEntities(const UdaChunk*, const UdaParser&);
            void loadAIConfig(const UdaChunk*, const UdaParser&);

            void writeOn(UdaChunk&, UdaWriter&) const;
            void writeSceneModelsOn(UdaChunk&, UdaWriter&) const;
            void writeSceneLightsOn(UdaChunk&, UdaWriter&) const;
            void writeTerrainEntities(UdaChunk&, UdaWriter&) const;
            void writeWaterEntities(UdaChunk&, UdaWriter&) const;
            void writeSkyEntity(UdaChunk&, UdaWriter&) const;
            void writeSoundEntities(UdaChunk&, UdaWriter&) const;
            void writeAIConfig(UdaChunk&, UdaWriter&) const;

            void refreshSound();

            static constexpr char MODELS_TAG[] = "models";
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

            TagHolder modelTagHolder;
            std::list<std::unique_ptr<SceneModel>> sceneModels;
            std::list<std::unique_ptr<SceneLight>> sceneLights;
            std::list<std::unique_ptr<TerrainEntity>> terrainEntities;
            std::list<std::unique_ptr<WaterEntity>> waterEntities;
            SkyEntity skyEntity;
            std::list<std::unique_ptr<SoundEntity>> soundEntities;
    };

}
