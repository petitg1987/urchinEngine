#include <stdexcept>
#include <memory>

#include "Map.h"

namespace urchin {

    Map::Map(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, SoundManager* soundManager, AIManager* aiManager) :
            renderer3d(renderer3d),
            physicsWorld(physicsWorld),
            soundManager(soundManager),
            aiManager(aiManager),
            sceneSky(std::make_unique<SceneSky>(renderer3d)),
            sceneAI(std::make_unique<SceneAI>(aiManager)) {

    }

    Map::~Map() {
        for (SceneLight* sceneLight : sceneLights) {
            delete sceneLight;
        }

        for (SceneTerrain* sceneTerrain : sceneTerrains) {
            delete sceneTerrain;
        }

        for (SceneWater* sceneWater : sceneWaters) {
            delete sceneWater;
        }

        for (SceneSound* sceneSound : sceneSounds) {
            delete sceneSound;
        }
    }

    void Map::loadFrom(const UdaChunk* rootChunk, const UdaParser& udaParser, LoadCallback& loadCallback) {
        if (renderer3d && !renderer3d->isPaused()) { //to avoid move camera before being able to see the map
            throw std::runtime_error("Renderer 3d should be paused while loading map.");
        }

        if (physicsWorld && !physicsWorld->isPaused()) { //to avoid miss of collision between objects just loaded and on objects not loaded yet
            throw std::runtime_error("Physics world should be paused while loading map.");
        }

        if (aiManager && !aiManager->isPaused()) { //to avoid compute path based on a world with missing objects
            throw std::runtime_error("AI manager should be paused while loading map.");
        }

        loadSceneObjectsFrom(rootChunk, udaParser);
        loadCallback.execute(LoadCallback::OBJECTS);

        loadSceneLightsFrom(rootChunk, udaParser);
        loadCallback.execute(LoadCallback::LIGHTS);

        loadSceneTerrainFrom(rootChunk, udaParser);
        loadSceneWaterFrom(rootChunk, udaParser);
        loadSceneSkyFrom(rootChunk, udaParser);
        loadCallback.execute(LoadCallback::LANDSCAPE);

        loadSceneSoundsFrom(rootChunk, udaParser);
        loadCallback.execute(LoadCallback::SOUNDS);

        loadSceneAIFrom(rootChunk, udaParser);
        loadCallback.execute(LoadCallback::AI);
    }

    void Map::loadSceneObjectsFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto objectsListChunk = udaParser.getUniqueChunk(true, OBJECTS_TAG, UdaAttribute(), chunk);
        auto objectsChunk = udaParser.getChunks(OBJECT_TAG, UdaAttribute(), objectsListChunk);

        for (const auto& objectChunk : objectsChunk) {
            auto sceneObject = std::make_unique<SceneObject>();
            sceneObject->loadFrom(objectChunk, udaParser);

            addSceneObject(std::move(sceneObject));
        }
    }

    void Map::loadSceneLightsFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto lightsListChunk = udaParser.getUniqueChunk(true, LIGHTS_TAG, UdaAttribute(), chunk);
        auto lightsChunk = udaParser.getChunks(LIGHT_TAG, UdaAttribute(), lightsListChunk);

        for (const auto& lightChunk : lightsChunk) {
            auto* sceneLight = new SceneLight();
            sceneLight->loadFrom(lightChunk, udaParser);

            addSceneLight(sceneLight);
        }
    }

    void Map::loadSceneTerrainFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto terrainsListChunk = udaParser.getUniqueChunk(true, TERRAINS_TAG, UdaAttribute(), chunk);
        auto terrainsChunk = udaParser.getChunks(TERRAIN_TAG, UdaAttribute(), terrainsListChunk);

        for (const auto& terrainChunk : terrainsChunk) {
            auto* sceneTerrain = new SceneTerrain();
            sceneTerrain->loadFrom(terrainChunk, udaParser);

            addSceneTerrain(sceneTerrain);
        }
    }

    void Map::loadSceneWaterFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto watersListChunk = udaParser.getUniqueChunk(true, WATERS_TAG, UdaAttribute(), chunk);
        auto watersChunk = udaParser.getChunks(WATER_TAG, UdaAttribute(), watersListChunk);

        for (const auto& waterChunk : watersChunk) {
            auto* sceneWater = new SceneWater();
            sceneWater->loadFrom(waterChunk, udaParser);

            addSceneWater(sceneWater);
        }
    }

    void Map::loadSceneSkyFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto skyChunk = udaParser.getUniqueChunk(true, SKY_TAG, UdaAttribute(), chunk);

        sceneSky->loadFrom(skyChunk, udaParser);
    }

    void Map::loadSceneSoundsFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto soundElementsListChunk = udaParser.getUniqueChunk(true, SOUND_ELEMENTS_TAG, UdaAttribute(), chunk);
        auto soundElementsChunk = udaParser.getChunks(SOUND_ELEMENT_TAG, UdaAttribute(), soundElementsListChunk);

        for (const auto& soundElementChunk : soundElementsChunk) {
            auto* sceneSound = new SceneSound();
            sceneSound->loadFrom(soundElementChunk, udaParser);

            addSceneSound(sceneSound);
        }
    }

    void Map::loadSceneAIFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        auto aiElementsListChunk = udaParser.getUniqueChunk(true, AI_ELEMENTS_TAG, UdaAttribute(), chunk);

        sceneAI->loadFrom(aiElementsListChunk, udaParser);
    }

    void Map::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        writeSceneObjectsOn(chunk, udaWriter);
        writeSceneLightsOn(chunk, udaWriter);
        writeSceneTerrainsOn(chunk, udaWriter);
        writeSceneWatersOn(chunk, udaWriter);
        writeSceneSkyOn(chunk, udaWriter);
        writeSceneSoundsOn(chunk, udaWriter);
        writeSceneAIOn(chunk, udaWriter);
    }

    void Map::writeSceneObjectsOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& objectsListChunk = udaWriter.createChunk(OBJECTS_TAG, UdaAttribute(), &chunk);

        for (auto& sceneObject : sceneObjects) {
            auto& objectsChunk = udaWriter.createChunk(OBJECT_TAG, UdaAttribute(), &objectsListChunk);
            sceneObject->writeOn(objectsChunk, udaWriter);
        }
    }

    void Map::writeSceneLightsOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& lightsListChunk = udaWriter.createChunk(LIGHTS_TAG, UdaAttribute(), &chunk);

        for (auto sceneLight : sceneLights) {
            auto& lightsChunk = udaWriter.createChunk(LIGHT_TAG, UdaAttribute(), &lightsListChunk);
            sceneLight->writeOn(lightsChunk, udaWriter);
        }
    }

    void Map::writeSceneTerrainsOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& terrainsListChunk = udaWriter.createChunk(TERRAINS_TAG, UdaAttribute(), &chunk);

        for (auto sceneTerrain : sceneTerrains) {
            auto& terrainsChunk = udaWriter.createChunk(TERRAIN_TAG, UdaAttribute(), &terrainsListChunk);
            sceneTerrain->writeOn(terrainsChunk, udaWriter);
        }
    }

    void Map::writeSceneWatersOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& watersListChunk = udaWriter.createChunk(WATERS_TAG, UdaAttribute(), &chunk);

        for (auto sceneWater : sceneWaters) {
            auto& watersChunk = udaWriter.createChunk(WATER_TAG, UdaAttribute(), &watersListChunk);
            sceneWater->writeOn(watersChunk, udaWriter);
        }
    }

    void Map::writeSceneSkyOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& skyChunk = udaWriter.createChunk(SKY_TAG, UdaAttribute(), &chunk);

        sceneSky->writeOn(skyChunk, udaWriter);
    }

    void Map::writeSceneSoundsOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& soundElementsListChunk = udaWriter.createChunk(SOUND_ELEMENTS_TAG, UdaAttribute(), &chunk);

        for (auto sceneSound : sceneSounds) {
            auto& soundElementsChunk = udaWriter.createChunk(SOUND_ELEMENT_TAG, UdaAttribute(), &soundElementsListChunk);
            sceneSound->writeOn(soundElementsChunk, udaWriter);
        }
    }

    void Map::writeSceneAIOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        auto& aiElementsListChunk = udaWriter.createChunk(AI_ELEMENTS_TAG, UdaAttribute(), &chunk);

        sceneAI->writeOn(aiElementsListChunk, udaWriter);
    }

    const std::list<std::unique_ptr<SceneObject>>& Map::getSceneObjects() const {
        return sceneObjects;
    }

    SceneObject& Map::getSceneObject(const std::string& name) const {
        for (auto& sceneObject : sceneObjects) {
            if (sceneObject->getName() == name) {
                return *sceneObject;
            }
        }

        throw std::invalid_argument("Impossible to find a scene object having name: " + name);
    }

    void Map::addSceneObject(std::unique_ptr<SceneObject> sceneObject) {
        sceneObject->setObjectManagers(renderer3d, physicsWorld, aiManager);
        sceneObjects.push_back(std::move(sceneObject));
    }

    void Map::removeSceneObject(SceneObject& sceneObject) {
        sceneObjects.remove_if([&sceneObject](const auto& o){return o.get()==&sceneObject;});
    }

    const std::list<SceneLight*>& Map::getSceneLights() const {
        return sceneLights;
    }

    SceneLight* Map::getSceneLight(const std::string& name) const {
        for (auto sceneLight : sceneLights) {
            if (sceneLight->getName() == name) {
                return sceneLight;
            }
        }

        throw std::invalid_argument("Impossible to find a scene light having name: " + name);
    }

    void Map::addSceneLight(SceneLight* sceneLight) {
        sceneLight->setLightManager(renderer3d->getLightManager());
        sceneLights.push_back(sceneLight);
    }

    void Map::removeSceneLight(SceneLight* sceneLight) {
        sceneLights.remove(sceneLight);
        delete sceneLight;
    }

    const std::list<SceneTerrain*>& Map::getSceneTerrains() const {
        return sceneTerrains;
    }

    SceneTerrain* Map::getSceneTerrain(const std::string& name) const {
        for (auto sceneTerrain : sceneTerrains) {
            if (sceneTerrain->getName() == name) {
                return sceneTerrain;
            }
        }

        throw std::invalid_argument("Impossible to find a scene terrain having name: " + name);
    }

    void Map::addSceneTerrain(SceneTerrain* sceneTerrain) {
        sceneTerrain->setTerrainManagers(renderer3d, physicsWorld, aiManager);
        sceneTerrains.push_back(sceneTerrain);
    }

    void Map::removeSceneTerrain(SceneTerrain* sceneTerrain) {
        sceneTerrains.remove(sceneTerrain);
        delete sceneTerrain;
    }

    const std::list<SceneWater*>& Map::getSceneWaters() const {
        return sceneWaters;
    }

    SceneWater* Map::getSceneWater(const std::string& name) const {
        for (auto sceneWater : sceneWaters) {
            if (sceneWater->getName() == name) {
                return sceneWater;
            }
        }

        throw std::invalid_argument("Impossible to find a scene water having name: " + name);
    }

    void Map::addSceneWater(SceneWater* sceneWater) {
        sceneWater->setWaterManagers(renderer3d);
        sceneWaters.push_back(sceneWater);
    }

    void Map::removeSceneWater(SceneWater* sceneWater) {
        sceneWaters.remove(sceneWater);
        delete sceneWater;
    }

    const SceneSky& Map::getSceneSky() const {
        return *sceneSky;
    }

    void Map::updateSceneSky(std::unique_ptr<Skybox> skybox) {
        sceneSky->changeSkybox(std::move(skybox));
    }

    const std::list<SceneSound*>& Map::getSceneSounds() const {
        return sceneSounds;
    }

    SceneSound* Map::getSceneSound(const std::string& name) const {
        for (auto sceneSound : sceneSounds) {
            if (sceneSound->getName() == name) {
                return sceneSound;
            }
        }

        throw std::invalid_argument("Impossible to find a scene sound having name: " + name);
    }

    void Map::addSceneSound(SceneSound* sceneSound) {
        sceneSound->setSoundManager(soundManager);
        sceneSounds.push_back(sceneSound);
    }

    void Map::removeSceneSound(SceneSound* sceneSound) {
        sceneSounds.remove(sceneSound);
        delete sceneSound;
    }

    const SceneAI& Map::getSceneAI() const {
        return *sceneAI;
    }

    void Map::updateSceneAI(std::unique_ptr<NavMeshAgent> navMeshAgent) {
        sceneAI->changeNavMeshAgent(std::move(navMeshAgent));
    }

    void Map::pause() {
        if (renderer3d) {
            renderer3d->pause();
        }

        if (physicsWorld) {
            physicsWorld->pause();
        }

        if (aiManager) {
            aiManager->pause();
        }

        if (soundManager) {
            soundManager->pause();
        }
    }

    void Map::unpause() {
        if (renderer3d) {
            renderer3d->unpause();
        }

        if (physicsWorld) {
            physicsWorld->unpause();
        }

        if (aiManager) {
            aiManager->unpause();
        }

        if (soundManager) {
            soundManager->unpause();
        }
    }

    void Map::refreshMap() {
        physicsWorld->checkNoExceptionRaised();
        aiManager->checkNoExceptionRaised();
        soundManager->checkNoExceptionRaised();

        refreshEntities();
        refreshSound();
    }

    void Map::refreshEntities() {
        for (auto& sceneObject : sceneObjects) {
            sceneObject->refresh();
        }

        for (SceneTerrain* sceneTerrain : sceneTerrains) {
            sceneTerrain->refresh();
        }
    }

    void Map::refreshSound() {
        //update sound event
        if (renderer3d && renderer3d->getCamera()) {
            soundManager->process(renderer3d->getCamera()->getPosition());
        } else {
            soundManager->process();
        }
    }
}

