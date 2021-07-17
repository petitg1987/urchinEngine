#include <stdexcept>
#include <memory>

#include "Map.h"

namespace urchin {

    Map::Map(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, SoundEnvironment* soundEnvironment, AIEnvironment* aiEnvironment) :
            renderer3d(renderer3d),
            physicsWorld(physicsWorld),
            soundEnvironment(soundEnvironment),
            aiEnvironment(aiEnvironment),
            sceneSky(std::make_unique<SceneSky>(renderer3d)),
            sceneAI(std::make_unique<SceneAI>(aiEnvironment)) {

    }

    void Map::loadFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser, LoadCallback& loadCallback) {
        if (renderer3d && !renderer3d->isPaused()) { //to avoid move camera before being able to see the map
            throw std::runtime_error("Renderer 3d should be paused while loading map.");
        }

        if (physicsWorld && !physicsWorld->isPaused()) { //to avoid miss of collision between objects just loaded and on objects not loaded yet
            throw std::runtime_error("Physics world should be paused while loading map.");
        }

        if (aiEnvironment && !aiEnvironment->isPaused()) { //to avoid compute path based on a world with missing objects
            throw std::runtime_error("AI environment should be paused while loading map.");
        }

        loadSceneObjectsFrom(sceneChunk, udaParser);
        loadCallback.execute(LoadCallback::OBJECTS);

        loadSceneLightsFrom(sceneChunk, udaParser);
        loadCallback.execute(LoadCallback::LIGHTS);

        loadSceneTerrainFrom(sceneChunk, udaParser);
        loadSceneWaterFrom(sceneChunk, udaParser);
        loadSceneSkyFrom(sceneChunk, udaParser);
        loadCallback.execute(LoadCallback::LANDSCAPE);

        loadSceneSoundsFrom(sceneChunk, udaParser);
        loadCallback.execute(LoadCallback::SOUNDS);

        loadSceneAIFrom(sceneChunk, udaParser);
        loadCallback.execute(LoadCallback::AI);
    }

    void Map::loadSceneObjectsFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto objectsListChunk = udaParser.getUniqueChunk(true, OBJECTS_TAG, UdaAttribute(), sceneChunk);
        auto objectsChunk = udaParser.getChunks(OBJECT_TAG, UdaAttribute(), objectsListChunk);

        for (const auto& objectChunk : objectsChunk) {
            auto sceneObject = std::make_unique<SceneObject>();
            sceneObject->loadFrom(objectChunk, udaParser);

            addSceneObject(std::move(sceneObject));
        }
    }

    void Map::loadSceneLightsFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto lightsListChunk = udaParser.getUniqueChunk(true, LIGHTS_TAG, UdaAttribute(), sceneChunk);
        auto lightsChunk = udaParser.getChunks(LIGHT_TAG, UdaAttribute(), lightsListChunk);

        for (const auto& lightChunk : lightsChunk) {
            auto sceneLight = std::make_unique<SceneLight>();
            sceneLight->loadFrom(lightChunk, udaParser);

            addSceneLight(std::move(sceneLight));
        }
    }

    void Map::loadSceneTerrainFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto terrainsListChunk = udaParser.getUniqueChunk(true, TERRAINS_TAG, UdaAttribute(), sceneChunk);
        auto terrainsChunk = udaParser.getChunks(TERRAIN_TAG, UdaAttribute(), terrainsListChunk);

        for (const auto& terrainChunk : terrainsChunk) {
            auto sceneTerrain = std::make_unique<SceneTerrain>();
            sceneTerrain->loadFrom(terrainChunk, udaParser);

            addSceneTerrain(std::move(sceneTerrain));
        }
    }

    void Map::loadSceneWaterFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto watersListChunk = udaParser.getUniqueChunk(true, WATERS_TAG, UdaAttribute(), sceneChunk);
        auto watersChunk = udaParser.getChunks(WATER_TAG, UdaAttribute(), watersListChunk);

        for (const auto& waterChunk : watersChunk) {
            auto sceneWater = std::make_unique<SceneWater>();
            sceneWater->loadFrom(waterChunk, udaParser);

            addSceneWater(std::move(sceneWater));
        }
    }

    void Map::loadSceneSkyFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto skyChunk = udaParser.getUniqueChunk(true, SKY_TAG, UdaAttribute(), sceneChunk);

        sceneSky->loadFrom(skyChunk, udaParser);
    }

    void Map::loadSceneSoundsFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto soundElementsListChunk = udaParser.getUniqueChunk(true, SOUND_ELEMENTS_TAG, UdaAttribute(), sceneChunk);
        auto soundElementsChunk = udaParser.getChunks(SOUND_ELEMENT_TAG, UdaAttribute(), soundElementsListChunk);

        for (const auto& soundElementChunk : soundElementsChunk) {
            auto sceneSound = std::make_unique<SceneSound>();
            sceneSound->loadFrom(soundElementChunk, udaParser);

            addSceneSound(std::move(sceneSound));
        }
    }

    void Map::loadSceneAIFrom(const UdaChunk* sceneChunk, const UdaParser& udaParser) {
        auto aiElementsListChunk = udaParser.getUniqueChunk(true, AI_ELEMENTS_TAG, UdaAttribute(), sceneChunk);

        sceneAI->loadFrom(aiElementsListChunk, udaParser);
    }

    void Map::writeOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        writeSceneObjectsOn(sceneChunk, udaWriter);
        writeSceneLightsOn(sceneChunk, udaWriter);
        writeSceneTerrainsOn(sceneChunk, udaWriter);
        writeSceneWatersOn(sceneChunk, udaWriter);
        writeSceneSkyOn(sceneChunk, udaWriter);
        writeSceneSoundsOn(sceneChunk, udaWriter);
        writeSceneAIOn(sceneChunk, udaWriter);
    }

    void Map::writeSceneObjectsOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& objectsListChunk = udaWriter.createChunk(OBJECTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& sceneObject : sceneObjects) {
            auto& objectsChunk = udaWriter.createChunk(OBJECT_TAG, UdaAttribute(), &objectsListChunk);
            sceneObject->writeOn(objectsChunk, udaWriter);
        }
    }

    void Map::writeSceneLightsOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& lightsListChunk = udaWriter.createChunk(LIGHTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& sceneLight : sceneLights) {
            auto& lightsChunk = udaWriter.createChunk(LIGHT_TAG, UdaAttribute(), &lightsListChunk);
            sceneLight->writeOn(lightsChunk, udaWriter);
        }
    }

    void Map::writeSceneTerrainsOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& terrainsListChunk = udaWriter.createChunk(TERRAINS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& sceneTerrain : sceneTerrains) {
            auto& terrainsChunk = udaWriter.createChunk(TERRAIN_TAG, UdaAttribute(), &terrainsListChunk);
            sceneTerrain->writeOn(terrainsChunk, udaWriter);
        }
    }

    void Map::writeSceneWatersOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& watersListChunk = udaWriter.createChunk(WATERS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& sceneWater : sceneWaters) {
            auto& watersChunk = udaWriter.createChunk(WATER_TAG, UdaAttribute(), &watersListChunk);
            sceneWater->writeOn(watersChunk, udaWriter);
        }
    }

    void Map::writeSceneSkyOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& skyChunk = udaWriter.createChunk(SKY_TAG, UdaAttribute(), &sceneChunk);

        sceneSky->writeOn(skyChunk, udaWriter);
    }

    void Map::writeSceneSoundsOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& soundElementsListChunk = udaWriter.createChunk(SOUND_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

        for (auto& sceneSound : sceneSounds) {
            auto& soundElementsChunk = udaWriter.createChunk(SOUND_ELEMENT_TAG, UdaAttribute(), &soundElementsListChunk);
            sceneSound->writeOn(soundElementsChunk, udaWriter);
        }
    }

    void Map::writeSceneAIOn(UdaChunk& sceneChunk, UdaWriter& udaWriter) const {
        auto& aiElementsListChunk = udaWriter.createChunk(AI_ELEMENTS_TAG, UdaAttribute(), &sceneChunk);

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
        sceneObject->setObjectManagers(renderer3d, physicsWorld, aiEnvironment);
        sceneObjects.push_back(std::move(sceneObject));
    }

    void Map::removeSceneObject(SceneObject& sceneObject) {
        sceneObjects.remove_if([&sceneObject](const auto& o){return o.get()==&sceneObject;});
    }

    const std::list<std::unique_ptr<SceneLight>>& Map::getSceneLights() const {
        return sceneLights;
    }

    SceneLight& Map::getSceneLight(const std::string& name) const {
        for (auto& sceneLight : sceneLights) {
            if (sceneLight->getName() == name) {
                return *sceneLight;
            }
        }
        throw std::invalid_argument("Impossible to find a scene light having name: " + name);
    }

    void Map::addSceneLight(std::unique_ptr<SceneLight> sceneLight) {
        sceneLight->setLightManager(renderer3d->getLightManager());
        sceneLights.push_back(std::move(sceneLight));
    }

    void Map::removeSceneLight(SceneLight& sceneLight) {
        sceneLights.remove_if([&sceneLight](const auto& o){return o.get()==&sceneLight;});
    }

    const std::list<std::unique_ptr<SceneTerrain>>& Map::getSceneTerrains() const {
        return sceneTerrains;
    }

    SceneTerrain& Map::getSceneTerrain(const std::string& name) const {
        for (auto& sceneTerrain : sceneTerrains) {
            if (sceneTerrain->getName() == name) {
                return *sceneTerrain;
            }
        }
        throw std::invalid_argument("Impossible to find a scene terrain having name: " + name);
    }

    void Map::addSceneTerrain(std::unique_ptr<SceneTerrain> sceneTerrain) {
        sceneTerrain->setTerrainManagers(renderer3d, physicsWorld, aiEnvironment);
        sceneTerrains.push_back(std::move(sceneTerrain));
    }

    void Map::removeSceneTerrain(SceneTerrain& sceneTerrain) {
        sceneTerrains.remove_if([&sceneTerrain](const auto& o){return o.get()==&sceneTerrain;});
    }

    const std::list<std::unique_ptr<SceneWater>>& Map::getSceneWaters() const {
        return sceneWaters;
    }

    SceneWater& Map::getSceneWater(const std::string& name) const {
        for (auto& sceneWater : sceneWaters) {
            if (sceneWater->getName() == name) {
                return *sceneWater;
            }
        }
        throw std::invalid_argument("Impossible to find a scene water having name: " + name);
    }

    void Map::addSceneWater(std::unique_ptr<SceneWater> sceneWater) {
        sceneWater->setWaterManagers(renderer3d);
        sceneWaters.push_back(std::move(sceneWater));
    }

    void Map::removeSceneWater(SceneWater& sceneWater) {
        sceneWaters.remove_if([&sceneWater](const auto& o){return o.get()==&sceneWater;});
    }

    const SceneSky& Map::getSceneSky() const {
        return *sceneSky;
    }

    void Map::updateSceneSky(std::unique_ptr<Skybox> skybox) {
        sceneSky->changeSkybox(std::move(skybox));
    }

    const std::list<std::unique_ptr<SceneSound>>& Map::getSceneSounds() const {
        return sceneSounds;
    }

    SceneSound& Map::getSceneSound(const std::string& name) const {
        for (auto& sceneSound : sceneSounds) {
            if (sceneSound->getName() == name) {
                return *sceneSound;
            }
        }
        throw std::invalid_argument("Impossible to find a scene sound having name: " + name);
    }

    void Map::addSceneSound(std::unique_ptr<SceneSound> sceneSound) {
        sceneSound->setSoundEnvironment(soundEnvironment);
        sceneSounds.push_back(std::move(sceneSound));
    }

    void Map::removeSceneSound(SceneSound& sceneSound) {
        sceneSounds.remove_if([&sceneSound](const auto& o){return o.get()==&sceneSound;});
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

        if (aiEnvironment) {
            aiEnvironment->pause();
        }

        if (soundEnvironment) {
            soundEnvironment->pause();
        }
    }

    void Map::unpause() {
        if (renderer3d) {
            renderer3d->unpause();
        }

        if (physicsWorld) {
            physicsWorld->unpause();
        }

        if (aiEnvironment) {
            aiEnvironment->unpause();
        }

        if (soundEnvironment) {
            soundEnvironment->unpause();
        }
    }

    void Map::refreshMap() {
        physicsWorld->checkNoExceptionRaised();
        aiEnvironment->checkNoExceptionRaised();
        soundEnvironment->checkNoExceptionRaised();

        refreshEntities();
        refreshSound();
    }

    void Map::refreshEntities() {
        for (auto& sceneObject : sceneObjects) {
            sceneObject->refresh();
        }

        for (auto& sceneTerrain : sceneTerrains) {
            sceneTerrain->refresh();
        }
    }

    void Map::refreshSound() {
        //update sound event
        if (renderer3d && renderer3d->getCamera()) {
            soundEnvironment->process(renderer3d->getCamera()->getPosition());
        } else {
            soundEnvironment->process();
        }
    }
}

