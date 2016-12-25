#ifndef ENGINE_MAP_H
#define ENGINE_MAP_H

#include <string>
#include <list>
#include <utility>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"
#include "resources/object/SceneObject.h"
#include "resources/light/SceneLight.h"
#include "resources/sound/SceneSound.h"

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
		#define SOUND_ELEMENTS_TAG "soundElements"
		#define SOUND_ELEMENT_TAG "soundElement"

		public:
			friend class MapHandler;

			Map(Renderer3d *, PhysicsWorld *, SoundManager *);
			~Map();

			const std::list<SceneObject *> &getSceneObjects() const;
			SceneObject *getSceneObject(const std::string &) const;
			void addSceneObject(SceneObject *);
			void removeSceneObject(SceneObject *);

			const std::list<SceneLight *> &getSceneLights() const;
			SceneLight *getSceneLight(const std::string &) const;
			void addSceneLight(SceneLight *);
			void removeSceneLight(SceneLight *);

			const std::list<SceneSound *> &getSceneSounds() const;
			SceneSound *getSceneSound(const std::string &) const;
			void addSceneSound(SceneSound *);
			void removeSceneSound(SceneSound *);

			void refreshMap();

		private:
			void loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneObjectsFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneLightsFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
			void loadSceneSoundsFrom(std::shared_ptr<XmlChunk>, const XmlParser &);

			void writeOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneObjectsOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneLightsOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;
			void writeSceneSoundsOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;

			Renderer3d *renderer3d;
			PhysicsWorld *physicsWorld;
			SoundManager *soundManager;

			std::list<SceneObject *> sceneObjects;
			std::list<SceneLight *> sceneLights;
			std::list<SceneSound *> sceneSounds;
	};

}

#endif





