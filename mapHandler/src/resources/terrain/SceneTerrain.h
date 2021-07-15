#pragma once

#include <string>
#include <Urchin3dEngine.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

#include <resources/common/SceneEntity.h>

namespace urchin {

    class SceneTerrain : public SceneEntity {
        //XML attributes
        static constexpr char NAME_ATTR[] = "name";

        public:
            friend class Map;

            SceneTerrain();
            ~SceneTerrain() override;

            void setTerrainManagers(Renderer3d*, PhysicsWorld*, AIManager*);

            std::string getName() const;
            void setName(const std::string&);

            Terrain* getTerrain() const;
            void setTerrain(std::shared_ptr<Terrain>);

            RigidBody* getRigidBody() const override;

        protected:
            void moveTo(const Point3<float>&, const Quaternion<float>&) override;

        private:
            void loadFrom(const UdaChunk*, const UdaParser&);
            void writeOn(UdaChunk&, UdaWriter&) const;

            void setupInteractiveBody(const std::shared_ptr<RigidBody>&);
            void setupRigidBody(const std::shared_ptr<RigidBody>&);
            void setupAIObject();

            void deleteRigidBody();
            void deleteAIObjects();

            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            AIManager* aiManager;

            std::string name;
            std::shared_ptr<Terrain> terrain;
            std::shared_ptr<RigidBody> rigidBody;
            std::shared_ptr<AITerrain> aiTerrain;
    };

}
