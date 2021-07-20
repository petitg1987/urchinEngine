#include <stdexcept>

#include "SceneTerrain.h"
#include <resources/terrain/TerrainReaderWriter.h>
#include <resources/common/AIEntityBuilder.h>

namespace urchin {
    SceneTerrain::SceneTerrain() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            aiEnvironment(nullptr),
            terrain(nullptr),
            rigidBody(nullptr) {

    }

    SceneTerrain::~SceneTerrain() {
        if (terrain) {
            renderer3d->getTerrainContainer().removeTerrain(*terrain);
        }
        deleteRigidBody();
        deleteAIObjects();
    }

    void SceneTerrain::setup(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, AIEnvironment* aiEnvironment) {
        if (this->renderer3d) {
            throw std::invalid_argument("Cannot add the scene terrain on two different renderer.");
        }
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer manager for a scene terrain.");
        }

        this->renderer3d = renderer3d;
        this->physicsWorld = physicsWorld;
        this->aiEnvironment = aiEnvironment;

        renderer3d->getTerrainContainer().addTerrain(terrain);

        if (physicsWorld) {
            physicsWorld->addBody(rigidBody);
        }

        if (aiEnvironment && aiTerrain) {
            aiEnvironment->addEntity(aiTerrain);
        }
    }

    void SceneTerrain::loadFrom(const UdaChunk* chunk, const UdaParser& udaParser) {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setTerrain(TerrainReaderWriter().loadFrom(chunk, udaParser));

        auto collisionTerrainShape = std::make_unique<urchin::CollisionHeightfieldShape>(terrain->getMesh()->getVertices(),
                                                                                         terrain->getMesh()->getXSize(),
                                                                                         terrain->getMesh()->getZSize());
        auto terrainRigidBody = std::make_unique<RigidBody>(this->name, PhysicsTransform(terrain->getPosition()), std::move(collisionTerrainShape));
        setupInteractiveBody(std::move(terrainRigidBody));
    }

    void SceneTerrain::writeOn(UdaChunk& chunk, UdaWriter& udaWriter) const {
        chunk.addAttribute(UdaAttribute(NAME_ATTR, this->name));

        TerrainReaderWriter().writeOn(chunk, *terrain, udaWriter);
    }

    std::string SceneTerrain::getName() const {
        return name;
    }

    void SceneTerrain::setName(const std::string& name) {
        this->name = name;
    }

    Terrain* SceneTerrain::getTerrain() const {
        return terrain.get();
    }

    void SceneTerrain::setTerrain(std::shared_ptr<Terrain> terrain) {
        if (!terrain) {
            throw std::invalid_argument("Cannot set a null terrain on scene terrain.");
        }

        if (renderer3d) {
            if (this->terrain) {
                renderer3d->getTerrainContainer().removeTerrain(*this->terrain);
            }
            renderer3d->getTerrainContainer().addTerrain(terrain);
        }

        this->terrain = std::move(terrain);
    }

    RigidBody* SceneTerrain::getRigidBody() const {
        return rigidBody.get();
    }

    void SceneTerrain::moveTo(const Point3<float>& position, const Quaternion<float>& orientation) {
        terrain->setPosition(position);
        aiTerrain->updateTransform(position, orientation);
    }

    void SceneTerrain::setupInteractiveBody(const std::shared_ptr<RigidBody>& rigidBody) {
        setupRigidBody(rigidBody);
        setupAIObject();
    }

    void SceneTerrain::setupRigidBody(const std::shared_ptr<RigidBody>& rigidBody) {
        deleteRigidBody();

        this->rigidBody = rigidBody;
        if (physicsWorld && rigidBody) {
            physicsWorld->addBody(rigidBody);
        }
    }

    void SceneTerrain::setupAIObject() {
        deleteAIObjects();

        if (!rigidBody) {
            this->aiTerrain = nullptr;
        } else {
            std::string aiObjectName = "@" + rigidBody->getId(); //prefix to avoid collision name with objects
            this->aiTerrain = AIEntityBuilder::buildAITerrain(aiObjectName, rigidBody->getShape(), rigidBody->getTransform().toTransform());
            if (aiEnvironment) {
                aiEnvironment->addEntity(aiTerrain);
            }
        }
    }

    void SceneTerrain::deleteRigidBody() {
        if (physicsWorld && rigidBody) {
            physicsWorld->removeBody(*rigidBody);
        }
        rigidBody = nullptr;
    }

    void SceneTerrain::deleteAIObjects() {
        if (aiEnvironment && aiTerrain) {
            aiEnvironment->removeEntity(aiTerrain);
        }
    }
}
