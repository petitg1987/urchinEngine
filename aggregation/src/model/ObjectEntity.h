#pragma once

#include <string>
#include <Urchin3dEngine.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>

namespace urchin {

    class ObjectEntity final : public TaggableResource {
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

            Light* getLight() const;
            std::shared_ptr<Light>& getLightPtr();
            void setLight(std::shared_ptr<Light>);

            SoundComponent* getSoundComponent() const;
            void setSoundComponent(const std::shared_ptr<SoundComponent>&);

            std::unique_ptr<ObjectEntity> clone(std::string) const;
            void updateTransform(const Transform<float>&) const;
            void updatePosition(const Point3<float>&) const;
            void updateOrientation(const Quaternion<float>&) const;

            void refresh() const;

        private:
            void setup(Renderer3d*, PhysicsWorld*, SoundEnvironment*, AIEnvironment*);
            void setupRigidBody(std::shared_ptr<RigidBody>);
            void setupAIObject();

            void deleteRigidBody();
            void deleteAIObjects() const;

            Renderer3d* renderer3d;
            PhysicsWorld* physicsWorld;
            SoundEnvironment* soundEnvironment;
            AIEnvironment* aiEnvironment;

            std::string name;
            std::shared_ptr<Model> model;
            std::shared_ptr<RigidBody> rigidBody;
            std::shared_ptr<Light> light;
            std::shared_ptr<SoundComponent> soundComponent;
            std::shared_ptr<AIObject> aiObject;
    };

}
