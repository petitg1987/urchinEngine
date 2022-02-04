#pragma once

#include <string>
#include <Urchin3dEngine.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

namespace urchin {

    class TerrainEntity {
        public:
            friend class Map;

            TerrainEntity();
            ~TerrainEntity();

            std::string getName() const;
            void setName(std::string);

            Terrain* getTerrain() const;
            void setTerrain(std::shared_ptr<Terrain>);

            RigidBody* getRigidBody() const;
            void setupInteractiveBody(const std::shared_ptr<RigidBody>&);

            void refresh();

        private:
            void setup(Renderer3d&, PhysicsWorld&, AIEnvironment&);
            void setupRigidBody(const std::shared_ptr<RigidBody>&);
            void setupAIObject();

            void deleteRigidBody();
            void deleteAIObjects();

            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            AIEnvironment* aiEnvironment;

            std::string name;
            std::shared_ptr<Terrain> terrain;
            std::shared_ptr<RigidBody> rigidBody;
            std::shared_ptr<AITerrain> aiTerrain;
    };

}
