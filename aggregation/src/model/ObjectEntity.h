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

            const std::string& getName() const;
            void setName(std::string);

            Model* getModel() const;
            void setModel(const std::shared_ptr<Model>&);

            void setupInteractiveBody(const std::shared_ptr<RigidBody>&);
            RigidBody* getRigidBody() const;

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
            std::shared_ptr<Model> model;
            std::shared_ptr<RigidBody> rigidBody;
            std::shared_ptr<AIObject> aiObject;
    };

}
