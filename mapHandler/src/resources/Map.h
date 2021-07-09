#pragma once

#include <string>
#include <list>
#include <utility>

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>
#include <load/LoadCallback.h>
#include <resources/object/SceneObject.h>
#include <resources/light/SceneLight.h>
#include <resources/terrain/SceneTerrain.h>
#include <resources/water/SceneWater.h>
#include <resources/sky/SceneSky.h>
#include <resources/sound/SceneSound.h>
#include <resources/ai/SceneAI.h>
#include <resources/common/SceneEntity.h>

namespace urchin {

    /**
    * Represent a map loaded in memory
    */
    class Map {
        public:
            friend class MapHandler;

            Map(Renderer3d*, PhysicsWorld*, SoundManager*, AIManager*);
            ~Map();

            const std::list<SceneObject*>& getSceneObjects() const;
            SceneObject* getSceneObject(const std::string&) const;
            void addSceneObject(SceneObject*);
            void removeSceneObject(SceneObject*);

            const std::list<SceneLight*>& getSceneLights() const;
            SceneLight* getSceneLight(const std::string&) const;
            void addSceneLight(SceneLight*);
            void removeSceneLight(SceneLight*);

            const std::list<SceneTerrain*>& getSceneTerrains() const;
            SceneTerrain* getSceneTerrain(const std::string&) const;
            void addSceneTerrain(SceneTerrain*);
            void removeSceneTerrain(SceneTerrain*);

            const std::list<SceneWater*>& getSceneWaters() const;
            SceneWater* getSceneWater(const std::string&) const;
            void addSceneWater(SceneWater*);
            void removeSceneWater(SceneWater*);

            const SceneSky* getSceneSky() const;
            void updateSceneSky(std::unique_ptr<Skybox> skybox);

            const std::list<SceneSound*>& getSceneSounds() const;
            SceneSound* getSceneSound(const std::string&) const;
            void addSceneSound(SceneSound*);
            void removeSceneSound(SceneSound*);

            const SceneAI* getSceneAI() const;
            void updateSceneAI(const std::shared_ptr<NavMeshAgent>&);

            void refreshMap();

            void pause();
            void unpause();

        private:
            void loadFrom(const XmlChunk*, const XmlParser&, LoadCallback&);
            void loadSceneObjectsFrom(const XmlChunk*, const XmlParser&);
            void loadSceneLightsFrom(const XmlChunk*, const XmlParser&);
            void loadSceneTerrainFrom(const XmlChunk*, const XmlParser&);
            void loadSceneWaterFrom(const XmlChunk*, const XmlParser&);
            void loadSceneSkyFrom(const XmlChunk*, const XmlParser&);
            void loadSceneSoundsFrom(const XmlChunk*, const XmlParser&);
            void loadSceneAIFrom(const XmlChunk*, const XmlParser&);

            void writeOn(XmlChunk*, XmlWriter&) const;
            void writeSceneObjectsOn(XmlChunk*, XmlWriter&) const;
            void writeSceneLightsOn(XmlChunk*, XmlWriter&) const;
            void writeSceneTerrainsOn(XmlChunk*, XmlWriter&) const;
            void writeSceneWatersOn(XmlChunk*, XmlWriter&) const;
            void writeSceneSkyOn(XmlChunk*, XmlWriter&) const;
            void writeSceneSoundsOn(XmlChunk*, XmlWriter&) const;
            void writeSceneAIOn(XmlChunk*, XmlWriter&) const;

            void refreshEntities();
            void refreshSound();

            static constexpr char OBJECTS_TAG[] = "objects";
            static constexpr char OBJECT_TAG[] = "object";
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
            SoundManager* soundManager;
            AIManager* aiManager;

            std::list<SceneObject*> sceneObjects;
            std::list<SceneLight*> sceneLights;
            std::list<SceneTerrain*> sceneTerrains;
            std::list<SceneWater*> sceneWaters;
            SceneSky* sceneSky;
            std::list<SceneSound*> sceneSounds;
            SceneAI* sceneAI;
    };

}
