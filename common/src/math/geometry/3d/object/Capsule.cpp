#include "Capsule.h"

namespace urchin
{

	template<class T> Capsule<T>::Capsule():
		capsuleShape(CapsuleShape<T>(0.0, 0.0, CapsuleShape<T>::CAPSULE_X)),
		centerPosition(Point3<T>(0.0, 0.0, 0.0))
	{
		axis[0] = Vector3<T>(0.0, 0.0, 0.0);
		axis[1] = Vector3<T>(0.0, 0.0, 0.0);
		axis[2] = Vector3<T>(0.0, 0.0, 0.0);
	}

	template<class T> Capsule<T>::Capsule(T radius, T cylinderHeight, typename CapsuleShape<T>::CapsuleOrientation capsuleOrientation,
			const Point3<T> &centerPosition, const Quaternion<T> &orientation) :
		capsuleShape(CapsuleShape<T>(radius, cylinderHeight, capsuleOrientation)),
		centerPosition(centerPosition),
		orientation(orientation)
	{
		axis[0] = orientation.rotatePoint(Point3<T>(1.0, 0.0, 0.0)).toVector();
		axis[1] = orientation.rotatePoint(Point3<T>(0.0, 1.0, 0.0)).toVector();
		axis[2] = orientation.rotatePoint(Point3<T>(0.0, 0.0, 1.0)).toVector();
	}

	template<class T> Capsule<T>::~Capsule()
	{

	}

	template<class T> T Capsule<T>::getRadius() const
	{
		return capsuleShape.getRadius();
	}

	template<class T> T Capsule<T>::getCylinderHeight() const
	{
		return capsuleShape.getCylinderHeight();
	}

	template<class T> typename CapsuleShape<T>::CapsuleOrientation Capsule<T>::getCapsuleOrientation() const
	{
		return capsuleShape.getCapsuleOrientation();
	}

	template<class T> const Point3<T> &Capsule<T>::getCenterPosition() const
	{
		return centerPosition;
	}

	template<class T> const Quaternion<T> &Capsule<T>::getOrientation() const
	{
		return orientation;
	}

	/**
	 * @return Capsule normalized axis for given index
	 */
	template<class T> const Vector3<T> &Capsule<T>::getAxis(unsigned int index) const
	{
		return axis[index];
	}

	template<class T> Point3<T> Capsule<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		Vector3<T> normalizedDirection;
		if(direction.X==0.0 && direction.Y==0.0 && direction.Z==0.0)
		{
			normalizedDirection = Vector3<T>(1.0, 0.0, 0.0);
		}else
		{
			normalizedDirection = direction.normalize();
		}

		Point3<T> spherePos1 = centerPosition.translate(axis[getCapsuleOrientation()] * (T)(getCylinderHeight() / (T)2.0));
		Point3<T> supportPoint1 = spherePos1.translate(normalizedDirection * getRadius());

		Point3<T> spherePos2 = centerPosition.translate(axis[getCapsuleOrientation()] * (T)(-getCylinderHeight() / (T)2.0));
		Point3<T> supportPoint2 = spherePos2.translate(normalizedDirection * getRadius());

		if(normalizedDirection.dotProduct(supportPoint1.toVector()) > normalizedDirection.dotProduct(supportPoint2.toVector()))
		{
			return supportPoint1;
		}

		return supportPoint2;
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const Capsule<T> &capsule)
	{
		stream.setf(std::ios::left);
		stream << std::setw(20) << "Shape radius: " << capsule.getRadius() << std::endl;
		stream << std::setw(20) << "Shape height: " << capsule.getCylinderHeight() << std::endl;
		stream << std::setw(20) << "Shape orientation: " << capsule.getCapsuleOrientation() << std::endl;
		stream << std::setw(20) << "Center position: " << capsule.getCenterPosition() << std::endl;
		stream << std::setw(20) << "Orientation: " << capsule.getOrientation() << std::endl;

		return stream;
	}

	//explicit template
	template class Capsule<float>;
	template std::ostream& operator <<<float>(std::ostream &, const Capsule<float> &);

	template class Capsule<double>;
	template std::ostream& operator <<<double>(std::ostream &, const Capsule<double> &);

}
