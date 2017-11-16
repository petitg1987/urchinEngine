#ifndef URCHINENGINE_SCENETERRAIN_H
#define URCHINENGINE_SCENETERRAIN_H

#include <string>

#include "Urchin3dEngine.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinAIEngine.h"

namespace urchin
{

    class SceneTerrain
    {
        //XML attributes
        #define NAME_ATTR "name"

        public:
            friend class Map;

            SceneTerrain();
            ~SceneTerrain();

            void setTerrainManagers(Renderer3d *, PhysicsWorld *physicsWorld, AIManager *);

            std::string getName() const;
            void setName(const std::string &);

            Terrain *getTerrain() const;
            void setTerrain(Terrain *);

            RigidBody *getRigidBody() const;

        private:
            void loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &);
            void writeOn(std::shared_ptr<XmlChunk>, XmlWriter &) const;

            void setupInteractiveBody(RigidBody *);
            void setupRigidBody(RigidBody *);
            void setupAIObject(RigidBody *);

            void deleteRigidBody();
            void deleteAIObjects();

            Renderer3d *renderer3d;
            PhysicsWorld *physicsWorld;
            AIManager *aiManager;

            std::string name;
            Terrain *terrain;
            RigidBody *rigidBody;
            std::shared_ptr<AIObject> aiObject;
    };

}

#endif
