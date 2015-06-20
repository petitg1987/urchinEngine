#ifndef ENGINE_RIGIDBODY_H
#define ENGINE_RIGIDBODY_H

#include <string>
#include <memory>
#include "UrchinCommon.h"

#include "body/AbstractBody.h"
#include "body/work/WorkRigidBody.h"

namespace urchin
{

	class RigidBody : public AbstractBody
	{
		public:
			RigidBody(const std::string &, const Transform<float> &, std::shared_ptr<const CollisionShape3D>);
			virtual ~RigidBody();

			AbstractWorkBody *createWorkBody() const;

			void update(AbstractWorkBody *);
			void apply(const AbstractWorkBody *);

			const Vector3<float> &getLinearVelocity() const;
			const Vector3<float> &getAngularVelocity() const;

			const Vector3<float> &getTotalForce() const;
			void applyCentralForce(const Vector3<float> &);
			void applyForce(const Vector3<float> &, const Point3<float> &);

			const Vector3<float> &getTotalTorque() const;
			void applyTorque(const Vector3<float> &);

			void setMassProperties(float, const Vector3<float> &);
			float getMass() const;
			const Vector3<float> &getLocalInertia() const;

			void setDamping(float, float);
			float getLinearDamping() const;
			float getAngularDamping() const;

			void setLinearFactor(const Vector3<float> &);
			const Vector3<float> &getLinearFactor() const;
			void setAngularFactor(const Vector3<float> &);
			const Vector3<float> &getAngularFactor() const;

		private:
			//rigid body representation data
			Vector3<float> linearVelocity;
			Vector3<float> angularVelocity;

			Vector3<float> totalForce;
			Vector3<float> totalTorque;

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
