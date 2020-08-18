#ifndef URCHINENGINE_RIGIDBODY_H
#define URCHINENGINE_RIGIDBODY_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "AbstractBody.h"
#include "body/work/WorkRigidBody.h"

namespace urchin
{

    class RigidBody : public AbstractBody
    {
        public:
            RigidBody(const std::string &, const Transform<float> &, const std::shared_ptr<const CollisionShape3D> &);
            RigidBody(const RigidBody &);
            ~RigidBody() override = default;

            AbstractWorkBody *createWorkBody() const override;

            void updateTo(AbstractWorkBody *) override;
            bool applyFrom(const AbstractWorkBody *) override;

            Vector3<float> getLinearVelocity() const;
            Vector3<float> getAngularVelocity() const;

            Vector3<float> getTotalMomentum() const;
            void applyCentralMomentum(const Vector3<float> &);
            void applyMomentum(const Vector3<float> &, const Point3<float> &);

            Vector3<float> getTotalTorqueMomentum() const;
            void applyTorqueMomentum(const Vector3<float> &);

            void setMass(float);
            float getMass() const;
            Vector3<float> getLocalInertia() const;

            void setDamping(float, float);
            float getLinearDamping() const;
            float getAngularDamping() const;

            void setLinearFactor(const Vector3<float> &);
            Vector3<float> getLinearFactor() const;
            void setAngularFactor(const Vector3<float> &);
            Vector3<float> getAngularFactor() const;

        private:
            void initializeRigidBody(float, float, float, const Vector3<float> &, const Vector3<float> &);
            void refreshScaledShape() override;
            void refreshMassProperties();
            void refreshLocalInertia();

            //rigid body representation data
            Vector3<float> linearVelocity;
            Vector3<float> angularVelocity;

            Vector3<float> totalMomentum;
            Vector3<float> totalTorqueMomentum;

            //rigid body description data
            float mass;
            Vector3<float> localInertia;

            float linearDamping;
            float angularDamping;

            Vector3<float> linearFactor;
            Vector3<float> angularFactor;
    };

}

#endif
