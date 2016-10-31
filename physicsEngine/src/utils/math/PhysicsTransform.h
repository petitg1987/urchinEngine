#ifndef ENGINE_PHYSICSTRANSFORM_H
#define ENGINE_PHYSICSTRANSFORM_H

#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Class allow to create transform matrix from a given position and orientation
	*/
	class PhysicsTransform
	{
		public:
			PhysicsTransform();
			PhysicsTransform(const PhysicsTransform &);
			PhysicsTransform(const Point3<float> &, const Quaternion<float> &q = Quaternion<float>());

			void setPosition(const Point3<float> &);
			const Point3<float> &getPosition() const;

			void setOrientation(const Quaternion<float> &);
			const Quaternion<float> &getOrientation() const;
			Matrix3<float> retrieveOrientationMatrix() const;

			const Transform<float> toTransform() const;

			Point3<float> transform(const Point3<float> &) const;
			Point3<float> inverseTransform(const Point3<float> &) const;
			PhysicsTransform integrate(const Vector3<float> &, const Vector3<float> &, float) const;

		private:
			Point3<float> position;
			Quaternion<float> orientation;
	};

}

#endif
