#pragma once

#include <string>
#include <Urchin3dEngine.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

namespace urchin {

    class ObjectEntity : public TaggableResource {
        public:
            friend class Map;

            ObjectEntity();
            ~ObjectEntity() override;

            void setName(std::string);
            const std::string& getName() const;

            void setModel(const std::shared_ptr<Model>&);
            Model* getModel() const;
            const std::shared_ptr<Model>& getModelPtr() const;

            void setupInteractiveBody(std::shared_ptr<RigidBody>);
            RigidBody* getRigidBody() const;
            const std::shared_ptr<RigidBody>& getRigidBodyPtr() const;

            std::unique_ptr<ObjectEntity> clone(std::string) const;
            void updatePosition(const Point3<float>&) const;

            void refresh() const;

        private:
            void setup(Renderer3d&, PhysicsWorld&, AIEnvironment&);
            void setupRigidBody(std::shared_ptr<RigidBody>);
            void setupAIObject();

            void deleteRigidBody();
            void deleteAIObjects();

            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            AIEnvironment* aiEnvironment;

            std::string name;
            std::shared_ptr<Model> model;
            std::shared_ptr<RigidBody> rigidBody;
            std::shared_ptr<AIObject> aiObject;
    };

}
