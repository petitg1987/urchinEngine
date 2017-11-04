#include <stdexcept>

#include "SceneTerrain.h"
#include "resources/terrain/TerrainReaderWriter.h"

namespace urchin
{
    SceneTerrain::SceneTerrain() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            terrain(nullptr),
            rigidBody(nullptr)
    {

    }

    SceneTerrain::~SceneTerrain()
    {
        renderer3d->removeTerrain(terrain);
        if(physicsWorld!=nullptr)
        {
            physicsWorld->removeBody(rigidBody);
        }else
        {
            delete this->rigidBody;
        }
    }

    void SceneTerrain::setTerrainManagers(Renderer3d *renderer3d, PhysicsWorld *physicsWorld)
    {
        if(this->renderer3d!=nullptr)
        {
            throw std::invalid_argument("Cannot add the scene light on two different renderer.");
        }
        if(renderer3d==nullptr)
        {
            throw std::invalid_argument("Cannot specify a null renderer manager for a scene terrain.");
        }

        this->renderer3d = renderer3d;
        this->physicsWorld = physicsWorld;

        renderer3d->addTerrain(terrain);
        if(physicsWorld!=nullptr)
        {
            physicsWorld->addBody(rigidBody);
        }
    }

    void SceneTerrain::loadFrom(std::shared_ptr<XmlChunk> chunk, const XmlParser &xmlParser)
    {
        this->name = chunk->getAttributeValue(NAME_ATTR);

        setTerrain(TerrainReaderWriter().loadFrom(chunk, xmlParser));

        auto collisionTerrainShape = std::make_shared<urchin::CollisionHeightfieldShape>(terrain->getMesh()->getVertices(),
                                                                                         terrain->getMesh()->getXSize(),
                                                                                         terrain->getMesh()->getZSize());
        RigidBody *terrainRigidBody = new RigidBody(this->name, Transform<float>(terrain->getPosition()), collisionTerrainShape);
        setRigidBody(terrainRigidBody);
    }

    void SceneTerrain::writeOn(std::shared_ptr<XmlChunk> chunk, XmlWriter &xmlWriter) const
    {
        chunk->setAttribute(XmlAttribute(NAME_ATTR, this->name));

        TerrainReaderWriter().writeOn(chunk, terrain, xmlWriter);
    }

    std::string SceneTerrain::getName() const
    {
        return name;
    }

    void SceneTerrain::setName(const std::string &name)
    {
        this->name = name;
    }

    Terrain *SceneTerrain::getTerrain() const
    {
        return terrain;
    }

    void SceneTerrain::setTerrain(Terrain *terrain)
    {
        if(terrain==nullptr)
        {
            throw std::invalid_argument("Cannot set a null terrain on scene terrain.");
        }

        if(renderer3d!=nullptr)
        {
            renderer3d->removeTerrain(this->terrain);
            renderer3d->addTerrain(terrain);
        }else
        {
            delete this->terrain;
        }

        this->terrain = terrain;
    }

    RigidBody *SceneTerrain::getRigidBody() const
    {
        return rigidBody;
    }

    void SceneTerrain::setRigidBody(RigidBody *rigidBody)
    {
        if(physicsWorld!=nullptr)
        {
            physicsWorld->removeBody(this->rigidBody);
            physicsWorld->addBody(rigidBody);
        }else
        {
            delete this->rigidBody;
        }

        this->rigidBody = rigidBody;
    }
}
