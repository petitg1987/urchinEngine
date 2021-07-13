#pragma once

#include <string>
#include <Urchin3dEngine.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

#include <resources/common/SceneEntity.h>

namespace urchin {

    /**
    * Represent an object on the scene (3d model and physics)
    */
    class SceneObject : public SceneEntity {
        public:
            friend class Map;

            SceneObject();
            ~SceneObject() override;

            void setObjectManagers(Renderer3d*, PhysicsWorld*, AIManager*);

            const std::string& getName() const;
            void setName(const std::string&);

            Model* getModel() const;
            void setModel(Model*);

            void setupInteractiveBody(RigidBody*);

            RigidBody* getRigidBody() const override;

        protected:
            void moveTo(const Point3<float>&, const Quaternion<float>&) override;

        private:
            void loadFrom(const DataChunk*, const DataParser&);
            void writeOn(DataChunk&, DataWriter&) const;

            void setupRigidBody(RigidBody*);
            void setupAIObject();

            void deleteRigidBody();
            void deleteAIObjects();

            static constexpr char MODEL_TAG[] = "model";
            static constexpr char PHYSICS_TAG[] = "physics";
            static constexpr char NAME_ATTR[] = "name";

            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            AIManager* aiManager;

            std::string name;
            Model* model;
            RigidBody* rigidBody;
            std::shared_ptr<AIObject> aiObject;
    };

}
