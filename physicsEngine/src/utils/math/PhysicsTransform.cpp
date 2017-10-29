#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	PhysicsTransform::PhysicsTransform()
	{
		position.setNull();
		orientation.setIdentity();
	}

	PhysicsTransform::PhysicsTransform(const PhysicsTransform &physicsTransform) :
		position(physicsTransform.getPosition()),
		orientation(physicsTransform.getOrientation())
	{

	}

	PhysicsTransform::PhysicsTransform(const Point3<float> &position, const Quaternion<float> &orientation) :
		position(position),
		orientation(orientation)
	{

	}

	void PhysicsTransform::setPosition(const Point3<float> &position)
	{
		this->position = position;
	}

	const Point3<float> &PhysicsTransform::getPosition() const
	{
		return position;
	}

	void PhysicsTransform::setOrientation(const Quaternion<float> &orientation)
	{
		this->orientation = orientation;
	}

	const Quaternion<float> &PhysicsTransform::getOrientation() const
	{
		return orientation;
	}

	Matrix3<float> PhysicsTransform::retrieveOrientationMatrix() const
	{
		return orientation.toMatrix3();
	}

	/**
	 * @return Transform object with a scale of 1.0
	 */
	const Transform<float> PhysicsTransform::toTransform() const
	{
		return Transform<float>(position, orientation, 1.0f);
	}

	PhysicsTransform PhysicsTransform::inverse() const
	{
		Quaternion<float> invOrientation = orientation.inverse();
		return PhysicsTransform(invOrientation.rotatePoint(-position), invOrientation);
	}

	bool PhysicsTransform::equals(const PhysicsTransform &physicsTransform) const
	{
		return physicsTransform.getPosition() == position && physicsTransform.getOrientation() == orientation;
	}

	Point3<float> PhysicsTransform::transform(const Point3<float> &point) const
	{
		return orientation.rotatePoint(point) + position;
	}

	Point3<float> PhysicsTransform::inverseTransform(const Point3<float> &point) const
	{
		return orientation.conjugate().rotatePoint(point - position);
	}

	PhysicsTransform PhysicsTransform::integrate(const Vector3<float> &linearVelocity, const Vector3<float> &angularVelocity, float timeStep) const
	{
		Point3<float> interpolatePosition = position.translate(linearVelocity * timeStep);
		Quaternion<float> interpolateOrientation = orientation;

		float length = angularVelocity.length();
		if(length > 0.0)
		{
			const Vector3<float> normalizedAxis = angularVelocity / length;
			const float angle = length * timeStep;

			interpolateOrientation = Quaternion<float>(normalizedAxis, angle) * interpolateOrientation;
			interpolateOrientation = interpolateOrientation.normalize();
		}

		return PhysicsTransform(interpolatePosition, interpolateOrientation);
	}

	PhysicsTransform PhysicsTransform::operator *(const PhysicsTransform &transform) const
	{
		return PhysicsTransform(position + orientation.rotatePoint(transform.getPosition()),
				orientation * transform.getOrientation());
	}

	const PhysicsTransform& PhysicsTransform::operator *=(const PhysicsTransform &transform)
	{
		*this = *this * transform;

		return *this;
	}

	std::ostream& operator <<(std::ostream &stream, const PhysicsTransform &transform)
	{
		stream.setf(std::ios::left);
		stream << "Position: " << transform.getPosition() << ", Orientation: " << transform.getOrientation();
		return stream;
	}

}
