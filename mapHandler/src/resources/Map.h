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
#include <resources/terrain/SceneTerrain.h>
#include <resources/water/SceneWater.h>
#include <resources/sky/SceneSky.h>
#include <resources/sound/SceneSound.h>

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

            const std::list<std::unique_ptr<SceneTerrain>>& getSceneTerrains() const;
            SceneTerrain& getSceneTerrain(const std::string&) const;
            void addSceneTerrain(std::unique_ptr<SceneTerrain>);
            void removeSceneTerrain(SceneTerrain&);

            const std::list<std::unique_ptr<SceneWater>>& getSceneWaters() const;
            SceneWater& getSceneWater(const std::string&) const;
            void addSceneWater(std::unique_ptr<SceneWater>);
            void removeSceneWater(SceneWater&);

            const SceneSky& getSceneSky() const;
            void updateSceneSky(std::unique_ptr<Skybox> skybox);

            const std::list<std::unique_ptr<SceneSound>>& getSceneSounds() const;
            SceneSound& getSceneSound(const std::string&) const;
            void addSceneSound(std::unique_ptr<SceneSound>);
            void removeSceneSound(SceneSound&);

            void refreshMap();

            void pause();
            void unpause();

        private:
            void loadFrom(const UdaChunk*, const UdaParser&, LoadMapCallback&);
            void loadSceneModelsFrom(const UdaChunk*, const UdaParser&);
            void loadSceneLightsFrom(const UdaChunk*, const UdaParser&);
            void loadSceneTerrainFrom(const UdaChunk*, const UdaParser&);
            void loadSceneWaterFrom(const UdaChunk*, const UdaParser&);
            void loadSceneSkyFrom(const UdaChunk*, const UdaParser&);
            void loadSceneSoundsFrom(const UdaChunk*, const UdaParser&);
            void loadAIConfig(const UdaChunk*, const UdaParser&);

            void writeOn(UdaChunk&, UdaWriter&) const;
            void writeSceneModelsOn(UdaChunk&, UdaWriter&) const;
            void writeSceneLightsOn(UdaChunk&, UdaWriter&) const;
            void writeSceneTerrainsOn(UdaChunk&, UdaWriter&) const;
            void writeSceneWatersOn(UdaChunk&, UdaWriter&) const;
            void writeSceneSkyOn(UdaChunk&, UdaWriter&) const;
            void writeSceneSoundsOn(UdaChunk&, UdaWriter&) const;
            void writeSceneAIOn(UdaChunk&, UdaWriter&) const;

            void refreshEntities();
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
            std::list<std::unique_ptr<SceneTerrain>> sceneTerrains;
            std::list<std::unique_ptr<SceneWater>> sceneWaters;
            SceneSky sceneSky;
            std::list<std::unique_ptr<SceneSound>> sceneSounds;
    };

}
