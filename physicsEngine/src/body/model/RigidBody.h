#ifndef URCHINENGINE_RIGIDBODY_H
#define URCHINENGINE_RIGIDBODY_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "AbstractBody.h"

namespace urchin {

    class RigidBody : public AbstractBody {
        public:
            RigidBody(const std::string&, const PhysicsTransform&, const std::shared_ptr<const CollisionShape3D>&);
            RigidBody(const RigidBody&);
            ~RigidBody() override = default;

            static RigidBody* upCast(AbstractBody*);
            static const RigidBody* upCast(const AbstractBody*);

            void setTransform(const PhysicsTransform&) override;
            void setPosition(const Point3<float>&) override;
            void setOrientation(const Quaternion<float>&) override;

            void setVelocity(const Vector3<float>&, const Vector3<float>&); //TODO internal method
            Vector3<float> getLinearVelocity() const;
            Vector3<float> getAngularVelocity() const;

            Vector3<float> getTotalMomentum() const;
            void applyCentralMomentum(const Vector3<float>&); //TODO [URGENT] could have no effect if called by thread 'x' and then thread 'y' call resetMomentum()
            void applyMomentum(const Vector3<float>&, const Point3<float>&);
            void resetMomentum(); //TODO internal method

            Vector3<float> getTotalTorqueMomentum() const;
            void applyTorqueMomentum(const Vector3<float>&);
            void resetTorqueMomentum(); //TODO internal method

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

            void setIsStatic(bool) override;

            bool isGhostBody() const override;

        private:
            void initializeRigidBody(float, float, float, const Vector3<float>&, const Vector3<float>&);
            void refreshMassProperties();
            void refreshInertia();
            void refreshWorldInertia();
            void refreshBodyActiveState();

            //rigid body representation data
            Vector3<float> linearVelocity;
            Vector3<float> angularVelocity;

            Vector3<float> totalMomentum;
            Vector3<float> totalTorqueMomentum;

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

#endif
