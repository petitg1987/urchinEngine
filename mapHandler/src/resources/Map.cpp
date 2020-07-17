#include <stdexcept>
#include <memory>

#include "Map.h"

namespace urchin
{

	Map::Map(Renderer3d *renderer3d, PhysicsWorld *physicsWorld, SoundManager *soundManager, AIManager *aiManager) :
			renderer3d(renderer3d),
			physicsWorld(physicsWorld),
			soundManager(soundManager),
			aiManager(aiManager),
			sceneSky(new SceneSky(renderer3d)),
            sceneAI(new SceneAI(aiManager))
	{

	}

	Map::~Map()
	{
        delete sceneAI;
        delete sceneSky;

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

		for(SceneWater *sceneWater : sceneWaters)
		{
			delete sceneWater;
		}

		for(SceneSound *sceneSound : sceneSounds)
		{
			delete sceneSound;
		}
	}

	void Map::loadFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser, LoadCallback &loadCallback)
	{
        if(renderer3d && !renderer3d->isPaused())
        { //to avoid move camera before being able to see the map
            throw std::runtime_error("Renderer 3d should be paused while loading map.");
        }

		if(physicsWorld && !physicsWorld->isPaused())
		{ //to avoid miss of collision between objects just loaded and on objects not loaded yet
			throw std::runtime_error("Physics world should be paused while loading map.");
		}

        if(aiManager && !aiManager->isPaused())
        { //to avoid compute path based on a world with missing objects
            throw std::runtime_error("AI manager should be paused while loading map.");
        }

		loadSceneObjectsFrom(chunk, xmlParser);
		loadCallback.execute(LoadCallback::OBJECTS);

		loadSceneLightsFrom(chunk, xmlParser);
		loadCallback.execute(LoadCallback::LIGHTS);

		loadSceneTerrainFrom(chunk, xmlParser);
		loadSceneWaterFrom(chunk, xmlParser);
        loadSceneSkyFrom(chunk, xmlParser);
		loadCallback.execute(LoadCallback::LANDSCAPE);

		loadSceneSoundsFrom(chunk, xmlParser);
		loadCallback.execute(LoadCallback::SOUNDS);

		loadSceneAIFrom(chunk, xmlParser);
		loadCallback.execute(LoadCallback::AI);
	}

	void Map::loadSceneObjectsFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> objectsListChunk = xmlParser.getUniqueChunk(true, OBJECTS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> objectsChunk = xmlParser.getChunks(OBJECT_TAG, XmlAttribute(), objectsListChunk);

		for (const auto &objectChunk : objectsChunk)
		{
			auto *sceneObject = new SceneObject();
			sceneObject->loadFrom(objectChunk, xmlParser);

			addSceneObject(sceneObject);
		}
	}

	void Map::loadSceneLightsFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> lightsListChunk = xmlParser.getUniqueChunk(true, LIGHTS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> lightsChunk = xmlParser.getChunks(LIGHT_TAG, XmlAttribute(), lightsListChunk);

		for (const auto &lightChunk : lightsChunk)
		{
			auto *sceneLight = new SceneLight();
			sceneLight->loadFrom(lightChunk, xmlParser);

			addSceneLight(sceneLight);
		}
	}

	void Map::loadSceneTerrainFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> terrainsListChunk = xmlParser.getUniqueChunk(true, TERRAINS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> terrainsChunk = xmlParser.getChunks(TERRAIN_TAG, XmlAttribute(), terrainsListChunk);

		for (const auto &terrainChunk : terrainsChunk)
		{
			auto *sceneTerrain = new SceneTerrain();
			sceneTerrain->loadFrom(terrainChunk, xmlParser);

			addSceneTerrain(sceneTerrain);
		}
	}

	void Map::loadSceneWaterFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> watersListChunk = xmlParser.getUniqueChunk(true, WATERS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> watersChunk = xmlParser.getChunks(WATER_TAG, XmlAttribute(), watersListChunk);

		for (const auto &waterChunk : watersChunk)
		{
			auto *sceneWater = new SceneWater();
			sceneWater->loadFrom(waterChunk, xmlParser);

			addSceneWater(sceneWater);
		}
	}

    void Map::loadSceneSkyFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
    {
        std::shared_ptr<XmlChunk> skyChunk = xmlParser.getUniqueChunk(true, SKY_TAG, XmlAttribute(), chunk);

        sceneSky->loadFrom(skyChunk, xmlParser);
    }

	void Map::loadSceneSoundsFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> soundElementsListChunk = xmlParser.getUniqueChunk(true, SOUND_ELEMENTS_TAG, XmlAttribute(), chunk);
		std::vector<std::shared_ptr<XmlChunk>> soundElementsChunk = xmlParser.getChunks(SOUND_ELEMENT_TAG, XmlAttribute(), soundElementsListChunk);

		for (const auto &soundElementChunk : soundElementsChunk)
		{
			auto *sceneSound = new SceneSound();
			sceneSound->loadFrom(soundElementChunk, xmlParser);

			addSceneSound(sceneSound);
		}
	}

	void Map::loadSceneAIFrom(const std::shared_ptr<XmlChunk> &chunk, const XmlParser &xmlParser)
	{
		std::shared_ptr<XmlChunk> aiElementsListChunk = xmlParser.getUniqueChunk(true, AI_ELEMENTS_TAG, XmlAttribute(), chunk);

        sceneAI->loadFrom(aiElementsListChunk, xmlParser);
	}

	void Map::writeOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
	{
		writeSceneObjectsOn(chunk, xmlWriter);
		writeSceneLightsOn(chunk, xmlWriter);
		writeSceneTerrainsOn(chunk, xmlWriter);
		writeSceneWatersOn(chunk, xmlWriter);
        writeSceneSkyOn(chunk, xmlWriter);
		writeSceneSoundsOn(chunk, xmlWriter);
		writeSceneAIOn(chunk, xmlWriter);
	}

	void Map::writeSceneObjectsOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> objectsListChunk = xmlWriter.createChunk(OBJECTS_TAG, XmlAttribute(), chunk);

		for (auto sceneObject : sceneObjects)
		{
			std::shared_ptr<XmlChunk> objectsChunk = xmlWriter.createChunk(OBJECT_TAG, XmlAttribute(), objectsListChunk);
			sceneObject->writeOn(objectsChunk, xmlWriter);
		}
	}

	void Map::writeSceneLightsOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> lightsListChunk = xmlWriter.createChunk(LIGHTS_TAG, XmlAttribute(), chunk);

		for (auto sceneLight : sceneLights)
		{
			std::shared_ptr<XmlChunk> lightsChunk = xmlWriter.createChunk(LIGHT_TAG, XmlAttribute(), lightsListChunk);
			sceneLight->writeOn(lightsChunk, xmlWriter);
		}
	}

	void Map::writeSceneTerrainsOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> terrainsListChunk = xmlWriter.createChunk(TERRAINS_TAG, XmlAttribute(), chunk);

		for (auto sceneTerrain : sceneTerrains)
		{
			std::shared_ptr<XmlChunk> terrainsChunk = xmlWriter.createChunk(TERRAIN_TAG, XmlAttribute(), terrainsListChunk);
			sceneTerrain->writeOn(terrainsChunk, xmlWriter);
		}
	}

	void Map::writeSceneWatersOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> watersListChunk = xmlWriter.createChunk(WATERS_TAG, XmlAttribute(), chunk);

		for (auto sceneWater : sceneWaters)
		{
			std::shared_ptr<XmlChunk> watersChunk = xmlWriter.createChunk(WATER_TAG, XmlAttribute(), watersListChunk);
			sceneWater->writeOn(watersChunk, xmlWriter);
		}
	}

    void Map::writeSceneSkyOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> skyChunk = xmlWriter.createChunk(SKY_TAG, XmlAttribute(), chunk);

        sceneSky->writeOn(skyChunk, xmlWriter);
    }

	void Map::writeSceneSoundsOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> soundElementsListChunk = xmlWriter.createChunk(SOUND_ELEMENTS_TAG, XmlAttribute(), chunk);

		for (auto sceneSound : sceneSounds)
		{
			std::shared_ptr<XmlChunk> soundElementsChunk = xmlWriter.createChunk(SOUND_ELEMENT_TAG, XmlAttribute(), soundElementsListChunk);
			sceneSound->writeOn(soundElementsChunk, xmlWriter);
		}
	}

    void Map::writeSceneAIOn(const std::shared_ptr<XmlChunk> &chunk, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> aiElementsListChunk = xmlWriter.createChunk(AI_ELEMENTS_TAG, XmlAttribute(), chunk);

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
		sceneTerrain->setTerrainManagers(renderer3d, physicsWorld, aiManager);
		sceneTerrains.push_back(sceneTerrain);
	}

	void Map::removeSceneTerrain(SceneTerrain *sceneTerrain)
	{
		sceneTerrains.remove(sceneTerrain);
		delete sceneTerrain;
	}

	const std::list<SceneWater *> &Map::getSceneWaters() const
	{
		return sceneWaters;
	}

	SceneWater *Map::getSceneWater(const std::string &name) const
	{
		for(auto sceneWater : sceneWaters)
		{
			if(sceneWater->getName() == name)
			{
				return sceneWater;
			}
		}

		throw std::invalid_argument("Impossible to find a scene water having name: " + name);
	}

	void Map::addSceneWater(SceneWater *sceneWater)
	{
		sceneWater->setWaterManagers(renderer3d);
		sceneWaters.push_back(sceneWater);
	}

	void Map::removeSceneWater(SceneWater *sceneWater)
	{
		sceneWaters.remove(sceneWater);
		delete sceneWater;
	}

    const SceneSky *Map::getSceneSky() const
    {
	    return sceneSky;
    }

    void Map::updateSceneSky(std::unique_ptr<Skybox> skybox)
    {
        sceneSky->changeSkybox(std::move(skybox));
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

	const SceneAI *Map::getSceneAI() const
	{
		return sceneAI;
	}

    void Map::updateSceneAI(const std::shared_ptr<NavMeshAgent> &navMeshAgent)
    {
	    sceneAI->changeNavMeshAgent(navMeshAgent);
    }

    void Map::pause()
    {
        if(renderer3d)
        {
            renderer3d->pause();
        }

        if(physicsWorld)
        {
            physicsWorld->pause();
        }

        if(aiManager)
        {
            aiManager->pause();
        }

        if(soundManager)
        {
            soundManager->pause();
        }
    }

    void Map::unpause()
    {
        if(renderer3d)
        {
            renderer3d->unpause();
        }

        if(physicsWorld)
        {
            physicsWorld->unpause();
        }

        if(aiManager)
        {
            aiManager->unpause();
        }

        if(soundManager)
        {
            soundManager->unpause();
        }
    }

	void Map::refreshMap()
	{
		physicsWorld->controlExecution();
		aiManager->controlExecution();
		soundManager->controlExecution();

        refreshEntities();
		refreshSound();
	}

	void Map::refreshEntities()
	{
		for(SceneObject *sceneObject : sceneObjects)
		{
			sceneObject->refresh();
		}

		for(SceneTerrain *sceneTerrain : sceneTerrains)
		{
			sceneTerrain->refresh();
		}
	}

	void Map::refreshSound()
	{
		//update sound event
		if(renderer3d && renderer3d->getCamera())
		{
			soundManager->process(renderer3d->getCamera()->getPosition());
		}else
		{
			soundManager->process();
		}
	}
}

