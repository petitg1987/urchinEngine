#include <stdexcept>

#include "SceneTerrain.h"
#include <resources/terrain/TerrainReaderWriter.h>
#include <resources/common/AIEntityBuilder.h>

namespace urchin {
    SceneTerrain::SceneTerrain() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            aiManager(nullptr),
            terrain(nullptr),
            rigidBody(nullptr) {

    }

    SceneTerrain::~SceneTerrain() {
        renderer3d->getTerrainManager()->removeTerrain(terrain);
        deleteRigidBody();
        deleteAIObjects();
    }

    void SceneTerrain::setTerrainManagers(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, AIManager* aiManager) {
        if (this->renderer3d) {
            throw std::invalid_argument("Cannot add the scene terrain on two different renderer.");
        }
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer manager for a scene terrain.");
        }

        this->renderer3d = renderer3d;
        this->physicsWorld = physicsWorld;
        this->aiManager = aiManager;

        renderer3d->getTerrainManager()->addTerrain(terrain);

        if (physicsWorld) {
            physicsWorld->addBody(rigidBody);
        }

        if (aiManager && aiTerrain) {
            aiManager->addEntity(aiTerrain);
        }
    }

    void SceneTerrain::loadFrom(const XmlChunk* chunk, const XmlParser& xmlParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setTerrain(TerrainReaderWriter().loadFrom(chunk, xmlParser));

        auto collisionTerrainShape = std::make_unique<urchin::CollisionHeightfieldShape>(terrain->getMesh()->getVertices(),
                                                                                         terrain->getMesh()->getXSize(),
                                                                                         terrain->getMesh()->getZSize());
        auto* terrainRigidBody = new RigidBody(this->name, PhysicsTransform(terrain->getPosition()), std::move(collisionTerrainShape));
        setupInteractiveBody(terrainRigidBody);
    }

    void SceneTerrain::writeOn(XmlChunk* chunk, XmlWriter& xmlWriter) const {
        chunk->setAttribute(XmlAttribute(NAME_ATTR, this->name));

        TerrainReaderWriter().writeOn(chunk, terrain, xmlWriter);
    }

    std::string SceneTerrain::getName() const {
        return name;
    }

    void SceneTerrain::setName(const std::string& name) {
        this->name = name;
    }

    Terrain* SceneTerrain::getTerrain() const {
        return terrain;
    }

    void SceneTerrain::setTerrain(Terrain* terrain) {
        if (!terrain) {
            throw std::invalid_argument("Cannot set a null terrain on scene terrain.");
        }

        if (renderer3d) {
            renderer3d->getTerrainManager()->removeTerrain(this->terrain);
            renderer3d->getTerrainManager()->addTerrain(terrain);
        } else {
            delete this->terrain;
        }

        this->terrain = terrain;
    }

    RigidBody* SceneTerrain::getRigidBody() const {
        return rigidBody;
    }

    void SceneTerrain::moveTo(const Point3<float>& position, const Quaternion<float>& orientation) {
        terrain->setPosition(position);
        aiTerrain->updateTransform(position, orientation);
    }

    void SceneTerrain::setupInteractiveBody(RigidBody* rigidBody) {
        setupRigidBody(rigidBody);
        setupAIObject(rigidBody);
    }

    void SceneTerrain::setupRigidBody(RigidBody* rigidBody) {
        deleteRigidBody();

        this->rigidBody = rigidBody;
        if (physicsWorld && rigidBody) {
            physicsWorld->addBody(rigidBody);
        }
    }

    void SceneTerrain::setupAIObject(RigidBody* rigidBody) {
        deleteAIObjects();

        if (!rigidBody) {
            this->aiTerrain = nullptr;
        } else {
            std::string aiObjectName = "@" + rigidBody->getId(); //prefix to avoid collision name with objects
            this->aiTerrain = AIEntityBuilder::buildAITerrain(aiObjectName, rigidBody->getShape(), rigidBody->getTransform().toTransform());
            if (aiManager) {
                aiManager->addEntity(aiTerrain);
            }
        }
    }

    void SceneTerrain::deleteRigidBody() {
        if (physicsWorld && rigidBody) {
            physicsWorld->removeBody(rigidBody);
        } else {
            delete rigidBody;
        }

        rigidBody = nullptr;
    }

    void SceneTerrain::deleteAIObjects() {
        if (aiManager && aiTerrain) {
            aiManager->removeEntity(aiTerrain);
        }
    }
}
