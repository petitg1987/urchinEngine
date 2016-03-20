#include <stdexcept>
#include <memory>

#include "Map.h"

namespace urchin
{

	Map::Map(Renderer3d *renderer3d, PhysicsWorld *physicsWorld, SoundManager *soundManager) :
			renderer3d(renderer3d),
			physicsWorld(physicsWorld),
			soundManager(soundManager)
	{

	}

	Map::~Map()
	{
		for(std::list<SceneObject *>::const_iterator it=sceneObjects.begin(); it!=sceneObjects.end(); ++it)
		{
			delete *it;
		}

		for(std::list<SceneLight *>::const_iterator it=sceneLights.begin(); it!=sceneLights.end(); ++it)
		{
			delete *it;
		}

		for(std::list<SceneSound *>::const_iterator it=sceneSounds.begin(); it!=sceneSounds.end(); ++it)
		{
			delete *it;
		}
	}

	void Map::loadFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		loadSceneObjectsFrom(chunk, xmlParser);
		loadSceneLightsFrom(chunk, xmlParser);
		loadSceneSoundsFrom(chunk, xmlParser);
	}

	void Map::loadSceneObjectsFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> objectsListChunk = xmlParser.getUniqueChunk(true, OBJECTS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> objectsChunk = xmlParser.getChunks(OBJECT_TAG, XmlAttribute(), objectsListChunk);

		for(unsigned int i=0; i<objectsChunk.size(); ++i)
		{
			SceneObject *sceneObject = new SceneObject();
			sceneObject->loadFrom(objectsChunk[i], xmlParser);

			addSceneObject(sceneObject);
		}
	}

	void Map::loadSceneLightsFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> lightsListChunk = xmlParser.getUniqueChunk(true, LIGHTS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> lightsChunk = xmlParser.getChunks(LIGHT_TAG, XmlAttribute(), lightsListChunk);

		for(unsigned int i=0; i<lightsChunk.size(); ++i)
		{
			SceneLight *sceneLight = new SceneLight();
			sceneLight->loadFrom(lightsChunk[i], xmlParser);

			addSceneLight(sceneLight);
		}
	}

	void Map::loadSceneSoundsFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> soundElementsListChunk = xmlParser.getUniqueChunk(true, SOUND_ELEMENTS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> soundElementsChunk = xmlParser.getChunks(SOUND_ELEMENT_TAG, XmlAttribute(), soundElementsListChunk);

		for(unsigned int i=0; i<soundElementsChunk.size(); ++i)
		{
			SceneSound *sceneSound = new SceneSound();
			sceneSound->loadFrom(soundElementsChunk[i], xmlParser);

			addSceneSound(sceneSound);
		}
	}

	void Map::writeOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		writeSceneObjectsOn(chunk, xmlWriter);
		writeSceneLightsOn(chunk, xmlWriter);
		writeSceneSoundsOn(chunk, xmlWriter);
	}

	void Map::writeSceneObjectsOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> objectsListChunk = xmlWriter.createChunk(OBJECTS_TAG, XmlAttribute(), chunk);

		for(std::list<SceneObject *>::const_iterator it=sceneObjects.begin(); it!=sceneObjects.end(); ++it)
		{
			std::shared_ptr<XmlChunk> objectsChunk = xmlWriter.createChunk(OBJECT_TAG, XmlAttribute(), objectsListChunk);
			(*it)->writeOn(objectsChunk, xmlWriter);
		}
	}

	void Map::writeSceneLightsOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> lightsListChunk = xmlWriter.createChunk(LIGHTS_TAG, XmlAttribute(), chunk);

		for(std::list<SceneLight *>::const_iterator it=sceneLights.begin(); it!=sceneLights.end(); ++it)
		{
			std::shared_ptr<XmlChunk> lightsChunk = xmlWriter.createChunk(LIGHT_TAG, XmlAttribute(), lightsListChunk);
			(*it)->writeOn(lightsChunk, xmlWriter);
		}
	}

	void Map::writeSceneSoundsOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> soundElementsListChunk = xmlWriter.createChunk(SOUND_ELEMENTS_TAG, XmlAttribute(), chunk);

		for(std::list<SceneSound *>::const_iterator it=sceneSounds.begin(); it!=sceneSounds.end(); ++it)
		{
			std::shared_ptr<XmlChunk> soundElementsChunk = xmlWriter.createChunk(SOUND_ELEMENT_TAG, XmlAttribute(), soundElementsListChunk);
			(*it)->writeOn(soundElementsChunk, xmlWriter);
		}
	}

	std::list<SceneObject *> Map::getSceneObjects() const
	{
		return sceneObjects;
	}

	SceneObject *Map::getSceneObject(const std::string &name) const
	{
		for(std::list<SceneObject *>::const_iterator it=sceneObjects.begin(); it!=sceneObjects.end(); ++it)
		{
			if((*it)->getName().compare(name)==0)
			{
				return *it;
			}
		}

		throw std::invalid_argument("Impossible to find a scene object having name: " + name);
	}

	void Map::addSceneObject(SceneObject *sceneObject)
	{
		sceneObject->setObjectManagers(renderer3d, physicsWorld);
		sceneObjects.push_back(sceneObject);
	}

	void Map::removeSceneObject(SceneObject *sceneObject)
	{
		sceneObjects.remove(sceneObject);
		delete sceneObject;
	}

	std::list<SceneLight *> Map::getSceneLights() const
	{
		return sceneLights;
	}

	SceneLight *Map::getSceneLight(const std::string &name) const
	{
		for(std::list<SceneLight *>::const_iterator it=sceneLights.begin(); it!=sceneLights.end(); ++it)
		{
			if((*it)->getName().compare(name)==0)
			{
				return *it;
			}
		}

		throw std::invalid_argument("Impossible to find a scene light having name: " + name);
	}

	void Map::addSceneLight(SceneLight *sceneLight)
	{
		sceneLight->setLightManager(renderer3d->getLightManager());
		sceneLights.push_back(sceneLight);
	}

	void Map::removeSceneLight(SceneLight *sceneLight)
	{
		sceneLights.remove(sceneLight);
		delete sceneLight;
	}

	std::list<SceneSound *> Map::getSceneSounds() const
	{
		return sceneSounds;
	}

	SceneSound *Map::getSceneSound(const std::string &name) const
	{
		for(std::list<SceneSound *>::const_iterator it=sceneSounds.begin(); it!=sceneSounds.end(); ++it)
		{
			if((*it)->getName().compare(name)==0)
			{
				return *it;
			}
		}

		throw std::invalid_argument("Impossible to find a scene sound having name: " + name);
	}

	void Map::addSceneSound(SceneSound *sceneSound)
	{
		sceneSound->setSoundManager(soundManager);
		sceneSounds.push_back(sceneSound);
	}

	void Map::removeSceneSound(SceneSound *sceneSound)
	{
		sceneSounds.remove(sceneSound);
		delete sceneSound;
	}

	void Map::refreshMap()
	{
		for(std::list<SceneObject *>::const_iterator it=sceneObjects.begin(); it!=sceneObjects.end(); ++it)
		{
			SceneObject *sceneObject = *it;
			RigidBody *rigidBody = sceneObject->getRigidBody();
			if(rigidBody!=nullptr && !rigidBody->isStatic() && rigidBody->isActive())
			{
				Model *model = sceneObject->getModel();
				model->setTransform(rigidBody->getTransform());
			}
		}
	}
}

