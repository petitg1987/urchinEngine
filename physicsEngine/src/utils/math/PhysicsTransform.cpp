#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	PhysicsTransform::PhysicsTransform()
	{
		pPosition.setNull();
		qOrientation.setIdentity();
	}

	PhysicsTransform::PhysicsTransform(const PhysicsTransform &physicsTransform) :
		pPosition(physicsTransform.getPosition()),
		qOrientation(physicsTransform.getOrientation()),
		mOrientation(physicsTransform.getOrientationMatrix())
	{

	}

	PhysicsTransform::PhysicsTransform(const Point3<float> &position, const Quaternion<float> &orientation) :
		pPosition(position),
		qOrientation(orientation)
	{
		mOrientation = qOrientation.toMatrix3();
	}

	void PhysicsTransform::setPosition(const Point3<float> &position)
	{
		pPosition = position;
	}

	const Point3<float> &PhysicsTransform::getPosition() const
	{
		return pPosition;
	}

	void PhysicsTransform::setOrientation(const Quaternion<float> &orientation)
	{
		qOrientation = orientation;

		mOrientation = qOrientation.toMatrix3();
	}

	const Quaternion<float> &PhysicsTransform::getOrientation() const
	{
		return qOrientation;
	}

	const Matrix3<float> &PhysicsTransform::getOrientationMatrix() const
	{
		return mOrientation;
	}

	/**
	 * @return Transform object with a scale of 1.0
	 */
	const Transform<float> PhysicsTransform::toTransform() const
	{
		return Transform<float>(pPosition, qOrientation, 1.0f);
	}

	Point3<float> PhysicsTransform::transform(const Point3<float> &point) const
	{
		//apply rotation matrix
		Point3<float> v = mOrientation * point;

		//apply translation
		return v + pPosition;
	}

	Point3<float> PhysicsTransform::inverseTransform(const Point3<float> &point) const
	{
		//apply inverse translation
		Point3<float> v = point - pPosition;

		//apply inverse rotation matrix (transpose=inverse for rotation matrix)
		return (mOrientation.transpose() * v);
	}

}
