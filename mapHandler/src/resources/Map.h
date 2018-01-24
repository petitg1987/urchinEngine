#ifndef URCHINENGINE_MAP_H
#define URCHINENGINE_MAP_H

#include <string>
#include <list>
#include <utility>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"
#include "UrchinAIEngine.h"
#include "load/LoadCallback.h"
#include "resources/object/SceneObject.h"
#include "resources/light/SceneLight.h"
#include "resources/terrain/SceneTerrain.h"
#include "resources/water/SceneWater.h"
#include "resources/sound/SceneSound.h"
#include "resources/ai/SceneAI.h"
#include "resources/common/SceneEntity.h"

namespace urchin
{

	/**
	* Represent a map loaded in memory
	*/
	class Map
	{
		//XML tags
		#define OBJECTS_TAG "objects"
		#define OBJECT_TAG "object"
		#define LIGHTS_TAG "lights"
		#define LIGHT_TAG "light"
		#define TERRAINS_TAG "terrains"
		#define TERRAIN_TAG "terrain"
		#define WATERS_TAG "waters"
		#define WATER_TAG "water"
		#define SOUND_ELEMENTS_TAG "soundElements"
		#define SOUND_ELEMENT_TAG "soundElement"
		#define AI_ELEMENTS_TAG "aiElements"

		public:
			friend class MapHandler;

			Map(Renderer3d *, PhysicsWorld *, SoundManager *, AIManager *);
			~Map();

			const std::list<SceneObject *> &getSceneObjects() const;
			SceneObject *getSceneObject(const std::string &) const;
			void addSceneObject(SceneObject *);
			void removeSceneObject(SceneObject *);

			const std::list<SceneLight *> &getSceneLights() const;
			SceneLight *getSceneLight(const std::string &) const;
			void addSceneLight(SceneLight *);
			void removeSceneLight(SceneLight *);

			const std::list<SceneTerrain *> &getSceneTerrains() const;
			SceneTerrain *getSceneTerrain(const std::string &) const;
			void addSceneTerrain(SceneTerrain *);
			void removeSceneTerrain(SceneTerrain *);

			const std::list<SceneWater *> &getSceneWaters() const;
			SceneWater *getSceneWater(const std::string &) const;
			void addSceneWater(SceneWater *);
			void removeSceneWater(SceneWater *);

			const std::list<SceneSound *> &getSceneSounds() const;
			SceneSound *getSceneSound(const std::string &) const;
			void addSceneSound(SceneSound *);
			void removeSceneSound(SceneSound *);

			SceneAI *getSceneAI() const;
			void setSceneAI(SceneAI *);

			void refreshMap();

		private:
			void loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &, LoadCallback &);
			void loadSceneObjectsFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneLightsFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneTerrainFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneWaterFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneSoundsFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneAIFrom(std::shared_ptr<XmlChunk>, const XmlParser &);

			void writeOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneObjectsOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneLightsOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneTerrainsOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneWatersOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneSoundsOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneAIOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;

			void refreshEntities();
			void refreshEntity(SceneEntity *);
            void refreshSound();

			Renderer3d *renderer3d;
			PhysicsWorld *physicsWorld;
			SoundManager *soundManager;
			AIManager *aiManager;

			std::list<SceneObject *> sceneObjects;
			std::list<SceneLight *> sceneLights;
			std::list<SceneTerrain *> sceneTerrains;
			std::list<SceneWater *> sceneWaters;
			std::list<SceneSound *> sceneSounds;
			SceneAI *sceneAI;
	};

}

#endif





