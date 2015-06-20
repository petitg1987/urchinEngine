#ifndef ENGINE_WORKRIGIDBODY_H
#define ENGINE_WORKRIGIDBODY_H

#include <string>
#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"

namespace urchin
{

	class WorkRigidBody : public AbstractWorkBody
	{
		public:
			WorkRigidBody(const std::string &, const PhysicsTransform &, std::shared_ptr<const CollisionShape3D>);
			~WorkRigidBody();

			static WorkRigidBody *upCast(AbstractWorkBody *);
			static const WorkRigidBody *upCast(const AbstractWorkBody *);

			void setLinearVelocity(const Vector3<float> &);
			const Vector3<float> &getLinearVelocity() const;

			void setAngularVelocity(const Vector3<float> &);
			const Vector3<float> &getAngularVelocity() const;

			const Vector3<float> &getTotalForce() const;
			void setTotalForce(const Vector3<float> &);
			void applyCentralForce(const Vector3<float> &);
			void applyForce(const Vector3<float> &, const Point3<float> &);
			void resetForce();

			const Vector3<float> &getTotalTorque() const;
			void setTotalTorque(const Vector3<float> &);
			void applyTorque(const Vector3<float> &);
			void resetTorque();

			void setMassProperties(float, const Vector3<float> &);
			float getMass() const;
			float getInvMass() const;
			void refreshInvWorldInertia();
			const Vector3<float> &getInvLocalInertia() const;
			const Matrix3<float> &getInvWorldInertia() const;

			void setDamping(float, float);
			float getLinearDamping() const;
			float getAngularDamping() const;

			void setLinearFactor(const Vector3<float> &);
			const Vector3<float> &getLinearFactor() const;
			void setAngularFactor(const Vector3<float> &);
			const Vector3<float> &getAngularFactor() const;

		private:
			void refreshBodyActiveState();

			//work rigid body representation data
			Vector3<float> linearVelocity;
			Vector3<float> angularVelocity;

			Vector3<float> totalForce;
			Vector3<float> totalTorque;

			//work rigid body description data
			float mass;
			float invMass;
			Vector3<float> invLocalInertia;
			Matrix3<float> invWorldInertia;

			float linearDamping;
			float angularDamping;

			Vector3<float> linearFactor;
			Vector3<float> angularFactor;
	};

}

#endif
