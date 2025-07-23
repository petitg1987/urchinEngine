#pragma once

#include <string>
#include <memory>
#include <UrchinCommon.h>

#include "body/model/AbstractBody.h"
#include "body/BodyMomentum.h"

namespace urchin {

    class RigidBody final : public AbstractBody {
        public:
            RigidBody(std::string, const PhysicsTransform&, std::unique_ptr<const CollisionShape3D>);
            RigidBody(const RigidBody&);
            RigidBody& operator=(const RigidBody&) = delete;
            ~RigidBody() override = default;

            static RigidBody* upCast(AbstractBody*);
            static RigidBody& upCast(AbstractBody&);

            void setTransform(const PhysicsTransform&) override;

            void setVelocity(const Vector3<float>&, const Vector3<float>&);
            Vector3<float> getLinearVelocity() const;
            Vector3<float> getAngularVelocity() const;

            void applyCentralMomentum(const Vector3<float>&);
            void applyMomentum(const Vector3<float>&, const Point3<float>&);
            void applyTorqueMomentum(const Vector3<float>&);
            BodyMomentum getMomentumAndReset();

            void setMass(float);
            float getMass() const;
            float getInvMass() const;
            Vector3<float> getLocalInertia() const;
            Matrix3<float> getInvWorldInertia() const;

            void setDamping(float, float);
            float getLinearDamping() const;
            float getAngularDamping() const;

            void setLinearFactor(const Vector3<float>&);
            Vector3<float> getLinearFactor() const;
            void setAngularFactor(const Vector3<float>&);
            Vector3<float> getAngularFactor() const;

        private:
            void initializeRigidBody(float, float, float, const Vector3<float>&, const Vector3<float>&);
            void refreshMassProperties();
            void refreshInertia();
            void refreshWorldInertia();
            void refreshBodyActiveState();

            //rigid body representation data
            Vector3<float> linearVelocity;
            Vector3<float> angularVelocity;
            BodyMomentum bodyMomentum;

            //rigid body description data
            float mass;
            float invMass;
            Vector3<float> localInertia;
            Vector3<float> invLocalInertia;
            Matrix3<float> invWorldInertia;

            float linearDamping;
            float angularDamping;

            Vector3<float> linearFactor;
            Vector3<float> angularFactor;
    };

}
