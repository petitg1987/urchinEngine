#include <stdexcept>
#include <memory>

#include "Map.h"

namespace urchin
{

	Map::Map(Renderer3d *renderer3d, PhysicsWorld *physicsWorld, SoundManager *soundManager, AIManager *aiManager) :
			renderer3d(renderer3d),
			physicsWorld(physicsWorld),
			soundManager(soundManager),
			aiManager(aiManager)
	{

	}

	Map::~Map()
	{
		for(SceneObject *sceneObject : sceneObjects)
		{
			delete sceneObject;
		}

		for(SceneLight *sceneLight : sceneLights)
		{
			delete sceneLight;
		}

		for(SceneTerrain *sceneTerrain : sceneTerrains)
		{
			delete sceneTerrain;
		}

		for(SceneSound *sceneSound : sceneSounds)
		{
			delete sceneSound;
		}

		delete sceneAI;
	}

	void Map::loadFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		loadSceneObjectsFrom(chunk, xmlParser);
		loadSceneLightsFrom(chunk, xmlParser);
		loadSceneTerrainFrom(chunk, xmlParser);
		loadSceneSoundsFrom(chunk, xmlParser);
		loadSceneAIFrom(chunk, xmlParser);
	}

	void Map::loadSceneObjectsFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> objectsListChunk = xmlParser.getUniqueChunk(true, OBJECTS_TAG, XmlAttribute(), std::move(chunk));
		std::vector<std::shared_ptr<XmlChunk>> objectsChunk = xmlParser.getChunks(OBJECT_TAG, XmlAttribute(), objectsListChunk);

		for (const auto &objectChunk : objectsChunk)
		{
			auto *sceneObject = new SceneObject();
			sceneObject->loadFrom(objectChunk, xmlParser);

			addSceneObject(sceneObject);
		}
	}

	void Map::loadSceneLightsFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> lightsListChunk = xmlParser.getUniqueChunk(true, LIGHTS_TAG, XmlAttribute(), std::move(chunk));
		std::vector<std::shared_ptr<XmlChunk>> lightsChunk = xmlParser.getChunks(LIGHT_TAG, XmlAttribute(), lightsListChunk);

		for (const auto &lightChunk : lightsChunk)
		{
			auto *sceneLight = new SceneLight();
			sceneLight->loadFrom(lightChunk, xmlParser);

			addSceneLight(sceneLight);
		}
	}

	void Map::loadSceneTerrainFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> terrainsListChunk = xmlParser.getUniqueChunk(true, TERRAINS_TAG, XmlAttribute(), std::move(chunk));
		std::vector<std::shared_ptr<XmlChunk>> terrainsChunk = xmlParser.getChunks(TERRAIN_TAG, XmlAttribute(), terrainsListChunk);

		for (const auto &terrainChunk : terrainsChunk)
		{
			auto *sceneTerrain = new SceneTerrain();
			sceneTerrain->loadFrom(terrainChunk, xmlParser);

			addSceneTerrain(sceneTerrain);
		}
	}

	void Map::loadSceneSoundsFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> soundElementsListChunk = xmlParser.getUniqueChunk(true, SOUND_ELEMENTS_TAG, XmlAttribute(), std::move(chunk));
		std::vector<std::shared_ptr<XmlChunk>> soundElementsChunk = xmlParser.getChunks(SOUND_ELEMENT_TAG, XmlAttribute(), soundElementsListChunk);

		for (const auto &soundElementChunk : soundElementsChunk)
		{
			auto *sceneSound = new SceneSound();
			sceneSound->loadFrom(soundElementChunk, xmlParser);

			addSceneSound(sceneSound);
		}
	}

	void Map::loadSceneAIFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> aiElementsListChunk = xmlParser.getUniqueChunk(true, AI_ELEMENTS_TAG, XmlAttribute(), std::move(chunk));

		auto *sceneAI = new SceneAI();
		sceneAI->loadFrom(aiElementsListChunk, xmlParser);

		setSceneAI(sceneAI);
	}

	void Map::writeOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		writeSceneObjectsOn(chunk, xmlWriter);
		writeSceneLightsOn(chunk, xmlWriter);
		writeSceneTerrainsOn(chunk, xmlWriter);
		writeSceneSoundsOn(chunk, xmlWriter);
		writeSceneAIOn(chunk, xmlWriter);
	}

	void Map::writeSceneObjectsOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> objectsListChunk = xmlWriter.createChunk(OBJECTS_TAG, XmlAttribute(), std::move(chunk));

		for (auto sceneObject : sceneObjects)
		{
			std::shared_ptr<XmlChunk> objectsChunk = xmlWriter.createChunk(OBJECT_TAG, XmlAttribute(), objectsListChunk);
			sceneObject->writeOn(objectsChunk, xmlWriter);
		}
	}

	void Map::writeSceneLightsOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> lightsListChunk = xmlWriter.createChunk(LIGHTS_TAG, XmlAttribute(), std::move(chunk));

		for (auto sceneLight : sceneLights)
		{
			std::shared_ptr<XmlChunk> lightsChunk = xmlWriter.createChunk(LIGHT_TAG, XmlAttribute(), lightsListChunk);
			sceneLight->writeOn(lightsChunk, xmlWriter);
		}
	}

	void Map::writeSceneTerrainsOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> terrainsListChunk = xmlWriter.createChunk(TERRAINS_TAG, XmlAttribute(), std::move(chunk));

		for (auto sceneTerrain : sceneTerrains)
		{
			std::shared_ptr<XmlChunk> lightsChunk = xmlWriter.createChunk(TERRAIN_TAG, XmlAttribute(), terrainsListChunk);
			sceneTerrain->writeOn(lightsChunk, xmlWriter);
		}
	}

	void Map::writeSceneSoundsOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> soundElementsListChunk = xmlWriter.createChunk(SOUND_ELEMENTS_TAG, XmlAttribute(), std::move(chunk));

		for (auto sceneSound : sceneSounds)
		{
			std::shared_ptr<XmlChunk> soundElementsChunk = xmlWriter.createChunk(SOUND_ELEMENT_TAG, XmlAttribute(), soundElementsListChunk);
			sceneSound->writeOn(soundElementsChunk, xmlWriter);
		}
	}

    void Map::writeSceneAIOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> aiElementsListChunk = xmlWriter.createChunk(AI_ELEMENTS_TAG, XmlAttribute(), std::move(chunk));

        sceneAI->writeOn(aiElementsListChunk, xmlWriter);
    }

	const std::list<SceneObject *> &Map::getSceneObjects() const
	{
		return sceneObjects;
	}

	SceneObject *Map::getSceneObject(const std::string &name) const
	{
		for (auto sceneObject : sceneObjects)
		{
			if(sceneObject->getName() == name)
			{
				return sceneObject;
			}
		}

		throw std::invalid_argument("Impossible to find a scene object having name: " + name);
	}

	void Map::addSceneObject(SceneObject *sceneObject)
	{
		sceneObject->setObjectManagers(renderer3d, physicsWorld, aiManager);
		sceneObjects.push_back(sceneObject);
	}

	void Map::removeSceneObject(SceneObject *sceneObject)
	{
		sceneObjects.remove(sceneObject);
		delete sceneObject;
	}

	const std::list<SceneLight *> &Map::getSceneLights() const
	{
		return sceneLights;
	}

	SceneLight *Map::getSceneLight(const std::string &name) const
	{
		for(auto sceneLight : sceneLights)
		{
			if(sceneLight->getName() == name)
			{
				return sceneLight;
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

	const std::list<SceneTerrain *> &Map::getSceneTerrains() const
	{
		return sceneTerrains;
	}

	SceneTerrain *Map::getSceneTerrain(const std::string &name) const
	{
		for(auto sceneTerrain : sceneTerrains)
		{
			if(sceneTerrain->getName() == name)
			{
				return sceneTerrain;
			}
		}

		throw std::invalid_argument("Impossible to find a scene terrain having name: " + name);
	}

	void Map::addSceneTerrain(SceneTerrain *sceneTerrain)
	{
		sceneTerrain->setTerrainManagers(renderer3d, physicsWorld);
		sceneTerrains.push_back(sceneTerrain);
	}

	void Map::removeSceneTerrain(SceneTerrain *sceneTerrain)
	{
		sceneTerrains.remove(sceneTerrain);
		delete sceneTerrain;
	}

	const std::list<SceneSound *> &Map::getSceneSounds() const
	{
		return sceneSounds;
	}

	SceneSound *Map::getSceneSound(const std::string &name) const
	{
		for (auto sceneSound : sceneSounds)
		{
			if(sceneSound->getName() == name)
			{
				return sceneSound;
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

	SceneAI *Map::getSceneAI() const
	{
		return sceneAI;
	}

	void Map::setSceneAI(SceneAI *sceneAI)
	{
		sceneAI->setAIManager(aiManager);
		this->sceneAI = sceneAI;
	}

	void Map::refreshMap()
	{
		refreshObjects();
		refreshSound();
	}

	void Map::refreshObjects()
	{
		for(SceneObject *sceneObject : sceneObjects)
		{
			RigidBody *rigidBody = sceneObject->getRigidBody();
			if(rigidBody!=nullptr && !rigidBody->isStatic() && rigidBody->isActive())
			{
				Transform<float> newTransform = rigidBody->getTransform(); //copy to avoid several calls on this slow method

				sceneObject->getModel()->setTransform(newTransform);
				sceneObject->getAIObject()->updateTransform(newTransform.getPosition(), newTransform.getOrientation());
			}
		}
	}

	void Map::refreshSound()
	{
		//update sound event
		if(renderer3d!=nullptr && renderer3d->getCamera()!=nullptr)
		{
			soundManager->process(renderer3d->getCamera()->getPosition());
		}else
		{
			soundManager->process();
		}
	}
}

