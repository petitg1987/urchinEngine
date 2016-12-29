#ifndef ENGINE_ABSTRACTBODY_H
#define ENGINE_ABSTRACTBODY_H

#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "shape/CollisionShape3D.h"

namespace urchin
{

	class AbstractBody
	{
		public:
			AbstractBody(const std::string &, const Transform<float> &, const std::shared_ptr<const CollisionShape3D> &);
			virtual ~AbstractBody();

			void setIsNew(bool);
			bool isNew() const;

			void markAsDeleted();
			bool isDeleted() const;

			virtual AbstractWorkBody *createWorkBody() const = 0;
			void setWorkBody(AbstractWorkBody *);
			AbstractWorkBody *getWorkBody() const;

			virtual void update(AbstractWorkBody *);
			virtual void apply(const AbstractWorkBody *);

			void setTransform(const Transform<float> &);
			const Transform<float> &getTransform() const;
			const Point3<float> &getPosition() const;
			const Quaternion<float> &getOrientation() const;
			std::shared_ptr<const CollisionShape3D> getOriginalShape() const;
			std::shared_ptr<const CollisionShape3D> getScaledShape() const;

			const std::string &getId() const;

			void setRestitution(float);
			float getRestitution() const;
			void setFriction(float);
			float getFriction() const;
			void setRollingFriction(float);
			float getRollingFriction() const;

			float getCcdMotionThreshold() const;
			void setCcdMotionThreshold(float);

			bool isStatic() const;
			bool isActive() const;

		protected:
			void setIsStatic(bool);

			//mutex for attributes modifiable from external
			mutable std::mutex bodyMutex;

		private:
			//technical data
			std::atomic_bool bIsNew;
			std::atomic_bool bIsDeleted;
			AbstractWorkBody *workBody;

			//body representation data
			Transform<float> transform;
			bool newTransformToApply;

			//body description data
			std::shared_ptr<const CollisionShape3D> originalShape;
			std::shared_ptr<const CollisionShape3D> scaledShape;
			std::string id;
			float restitution;
			float friction;
			float rollingFriction;
			float ccdMotionThreshold;

			//state flags
			std::atomic_bool bIsStatic;
			std::atomic_bool bIsActive;
	};

}

#endif
