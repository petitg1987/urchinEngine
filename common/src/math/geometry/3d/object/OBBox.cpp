#include "OBBox.h"

namespace urchin
{

	template<class T> OBBox<T>::OBBox():
		boxShape(BoxShape<T>(Vector3<T>(0.0, 0.0, 0.0))),
		centerPosition(Point3<T>(0.0, 0.0, 0.0))
	{
		axis[0] = Vector3<T>(0.0, 0.0, 0.0);
		axis[1] = Vector3<T>(0.0, 0.0, 0.0);
		axis[2] = Vector3<T>(0.0, 0.0, 0.0);
	}

	template<class T> OBBox<T>::OBBox(const Vector3<T> &halfSizes, const Point3<T> &centerPosition, const Quaternion<T> &orientation):
		boxShape(BoxShape<T>(halfSizes)),
		centerPosition(centerPosition),
		orientation(orientation)
	{
		axis[0] = orientation.rotatePoint(Point3<float>(1.0, 0.0, 0.0)).toVector();
		axis[1] = orientation.rotatePoint(Point3<float>(0.0, 1.0, 0.0)).toVector();
		axis[2] = orientation.rotatePoint(Point3<float>(0.0, 0.0, 1.0)).toVector();
	}

	template<class T> OBBox<T>::OBBox(const AABBox<T> &aabb) :
		boxShape(BoxShape<T>(Vector3<T>((aabb.getMax().X - aabb.getMin().X) / 2.0, (aabb.getMax().Y - aabb.getMin().Y) / 2.0, (aabb.getMax().Z - aabb.getMin().Z) / 2.0))),
		centerPosition((aabb.getMin() + aabb.getMax()) / (T)2.0)
	{
		axis[0] = Vector3<T>(1.0, 0.0, 0.0);
		axis[1] = Vector3<T>(0.0, 1.0, 0.0);
		axis[2] = Vector3<T>(0.0, 0.0, 1.0);
	}

	template<class T> OBBox<T>::OBBox(const Sphere<T> &sphere) :
		boxShape(BoxShape<T>(Vector3<T>(sphere.getRadius(), sphere.getRadius(), sphere.getRadius()))),
		centerPosition(sphere.getPosition())
	{
		axis[0] = Vector3<T>(1.0, 0.0, 0.0);
		axis[1] = Vector3<T>(0.0, 1.0, 0.0);
		axis[2] = Vector3<T>(0.0, 0.0, 1.0);
	}

	template<class T> OBBox<T>::~OBBox()
	{

	}

	template<class T> const T OBBox<T>::getHalfSize(unsigned int index) const
	{
		return boxShape.getHalfSize(index);
	}

	template<class T> const Vector3<T> &OBBox<T>::getHalfSizes() const
	{
		return boxShape.getHalfSizes();
	}

	template<class T> const Point3<T> &OBBox<T>::getCenterPosition() const
	{
		return centerPosition;
	}

	template<class T> const Quaternion<T> &OBBox<T>::getOrientation() const
	{
		return orientation;
	}

	/**
	 * @return Bounding box normalized axis for given index
	 */
	template<class T> const Vector3<T> &OBBox<T>::getAxis(unsigned int index) const
	{
		return axis[index];
	}

	template<class T> const Point3<T> OBBox<T>::getPoint(unsigned int index) const
	{
		switch(index)
		{
			case 0:
				return centerPosition.translate(-(this->getHalfSize(0) * axis[0]) - this->getHalfSize(1) * axis[1] - this->getHalfSize(2) * axis[2]);
			case 1:
				return centerPosition.translate(-(this->getHalfSize(0) * axis[0]) - this->getHalfSize(1) * axis[1] + this->getHalfSize(2) * axis[2]);
			case 2:
				return centerPosition.translate(-(this->getHalfSize(0) * axis[0]) + this->getHalfSize(1) * axis[1] - this->getHalfSize(2) * axis[2]);
			case 3:
				return centerPosition.translate(-(this->getHalfSize(0) * axis[0]) + this->getHalfSize(1) * axis[1] + this->getHalfSize(2) * axis[2]);
			case 4:
				return centerPosition.translate(this->getHalfSize(0) * axis[0] - this->getHalfSize(1) * axis[1] - this->getHalfSize(2) * axis[2]);
			case 5:
				return centerPosition.translate(this->getHalfSize(0) * axis[0] - this->getHalfSize(1) * axis[1] + this->getHalfSize(2) * axis[2]);
			case 6:
				return centerPosition.translate(this->getHalfSize(0) * axis[0] + this->getHalfSize(1) * axis[1] - this->getHalfSize(2) * axis[2]);
			case 7:
				return centerPosition.translate(this->getHalfSize(0) * axis[0] + this->getHalfSize(1) * axis[1] + this->getHalfSize(2) * axis[2]);
		}

		std::stringstream errorMessage;
		errorMessage<< "Invalid index: " << index <<".";
		throw std::invalid_argument(errorMessage.str());
	}

	template<class T> Point3<T> OBBox<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		T maxPointDotDirection = getPoint(0).toVector().dotProduct(direction);
		Point3<T> maxPoint = getPoint(0);

		for(unsigned int i=1;i<8; ++i)
		{
			T currentPointDotDirection  = getPoint(i).toVector().dotProduct(direction);
			if(currentPointDotDirection > maxPointDotDirection)
			{
				maxPointDotDirection = currentPointDotDirection;
				maxPoint = getPoint(i);
			}
		}

		return maxPoint;
	}

	template<class T> AABBox<T> OBBox<T>::toAABBox() const
	{
		return AABBox<T>(centerPosition.translate(-(this->getHalfSize(0) * axis[0]) - this->getHalfSize(1) * axis[1] - this->getHalfSize(2) * axis[2]),
			 centerPosition.translate(this->getHalfSize(0) * axis[0] + this->getHalfSize(1) * axis[1] + this->getHalfSize(2) * axis[2]));
	}

	/**
	* @return True if the bounding box collides or is inside this bounding box
	*/
	template<class T> bool OBBox<T>::collideWithOBBox(const OBBox<T> &bbox) const
	{ //Separated axis theorem (see http://jkh.me/files/tutorials/Separating%20Axis%20Theorem%20for%20Oriented%20Bounding%20Boxes.pdf)

		//test spheres collide
		Sphere<float> thisSphere(getPoint(0).distance(centerPosition), centerPosition);
		Sphere<float> bboxSphere(bbox.getPoint(0).distance(bbox.getCenterPosition()), bbox.getCenterPosition());
		if(!thisSphere.collideWithSphere(bboxSphere))
		{
			return false;
		}

		Vector3<T> distCenterPoint = getCenterPosition().vector(bbox.getCenterPosition());
		const T AdotB[3][3] = {
				{axis[0].dotProduct(bbox.getAxis(0)), axis[0].dotProduct(bbox.getAxis(1)), axis[0].dotProduct(bbox.getAxis(2))},
				{axis[1].dotProduct(bbox.getAxis(0)), axis[1].dotProduct(bbox.getAxis(1)), axis[1].dotProduct(bbox.getAxis(2))},
				{axis[2].dotProduct(bbox.getAxis(0)), axis[2].dotProduct(bbox.getAxis(1)), axis[2].dotProduct(bbox.getAxis(2))}
		};
		const T epsilon = (T)0.00001; //projectionAxis could be near to zero: need epsilon to avoid rounding error

		//case 1, 2, 3 (projectionAxis = axis[0] | axis[1] | axis[2])
		for(unsigned int i=0; i<3;i++)
		{
			if(std::abs(distCenterPoint.dotProduct(axis[i]))
				> this->getHalfSize(i)
				+ std::abs(bbox.getHalfSize(0) * AdotB[i][0])
				+ std::abs(bbox.getHalfSize(1) * AdotB[i][1])
				+ std::abs(bbox.getHalfSize(2) * AdotB[i][2]))
			{
				return false;
			}
		}

		//case 4, 5, 6 (projectionAxis = bbox.getAxis(0) | bbox.getAxis(1) | bbox.getAxis(2))
		for(unsigned int i=0; i<3;i++)
		{
			if(std::abs(distCenterPoint.dotProduct(bbox.getAxis(i)))
				> std::abs(this->getHalfSize(0) * AdotB[0][i])
				+ std::abs(this->getHalfSize(1) * AdotB[1][i])
				+ std::abs(this->getHalfSize(2) * AdotB[2][i])
				+ bbox.getHalfSize(i))
			{
				return false;
			}
		}

		//case 7 (projectionAxis = axis[0].crossProduct(bbox.getAxis(0)))
		if(std::abs(distCenterPoint.dotProduct(axis[0].crossProduct(bbox.getAxis(0)))) - epsilon
			> std::abs(this->getHalfSize(1) * AdotB[2][0])
			+ std::abs(this->getHalfSize(2) * AdotB[1][0])
			+ std::abs(bbox.getHalfSize(1) * AdotB[0][2])
			+ std::abs(bbox.getHalfSize(2) * AdotB[0][1]))
		{
			return false;
		}

		//case 8 (projectionAxis = axis[0].crossProduct(bbox.getAxis(1)))
		if(std::abs(distCenterPoint.dotProduct(axis[0].crossProduct(bbox.getAxis(1)))) - epsilon
		> std::abs(this->getHalfSize(1) * AdotB[2][1])
		+ std::abs(this->getHalfSize(2) * AdotB[1][1])
		+ std::abs(bbox.getHalfSize(0) * AdotB[0][2])
		+ std::abs(bbox.getHalfSize(2) * AdotB[0][0]))
		{
			return false;
		}

		//case 9 (projectionAxis = axis[0].crossProduct(bbox.getAxis(2)))
		if(std::abs(distCenterPoint.dotProduct(axis[0].crossProduct(bbox.getAxis(2)))) - epsilon
		> std::abs(this->getHalfSize(1) * AdotB[2][2])
		+ std::abs(this->getHalfSize(2) * AdotB[1][2])
		+ std::abs(bbox.getHalfSize(0) * AdotB[0][1])
		+ std::abs(bbox.getHalfSize(1) * AdotB[0][0]))
		{
			return false;
		}

		//case 10 (projectionAxis = axis[1].crossProduct(bbox.getAxis(0)))
		if(std::abs(distCenterPoint.dotProduct(axis[1].crossProduct(bbox.getAxis(0)))) - epsilon
			> std::abs(this->getHalfSize(0) * AdotB[2][0])
			+ std::abs(this->getHalfSize(2) * AdotB[0][0])
			+ std::abs(bbox.getHalfSize(1) * AdotB[1][2])
			+ std::abs(bbox.getHalfSize(2) * AdotB[1][1]))
		{
			return false;
		}

		//case 11 (projectionAxis = axis[1].crossProduct(bbox.getAxis(1)))
		if(std::abs(distCenterPoint.dotProduct(axis[1].crossProduct(bbox.getAxis(1)))) - epsilon
			> std::abs(this->getHalfSize(0) * AdotB[2][1])
			+ std::abs(this->getHalfSize(2) * AdotB[0][1])
			+ std::abs(bbox.getHalfSize(0) * AdotB[1][2])
			+ std::abs(bbox.getHalfSize(2) * AdotB[1][0]))
		{
			return false;
		}

		//case 12 (projectionAxis = axis[1].crossProduct(bbox.getAxis(2)))
		if(std::abs(distCenterPoint.dotProduct(axis[1].crossProduct(bbox.getAxis(2)))) - epsilon
			> std::abs(this->getHalfSize(0) * AdotB[2][2])
			+ std::abs(this->getHalfSize(2) * AdotB[0][2])
			+ std::abs(bbox.getHalfSize(0) * AdotB[1][1])
			+ std::abs(bbox.getHalfSize(1) * AdotB[1][0]))
		{
			return false;
		}

		//case 13 (projectionAxis = axis[2].crossProduct(bbox.getAxis(0)))
		if(std::abs(distCenterPoint.dotProduct(axis[2].crossProduct(bbox.getAxis(0)))) - epsilon
			> std::abs(this->getHalfSize(0) * AdotB[1][0])
			+ std::abs(this->getHalfSize(1) * AdotB[0][0])
			+ std::abs(bbox.getHalfSize(1) * AdotB[2][2])
			+ std::abs(bbox.getHalfSize(2) * AdotB[2][1]))
		{
			return false;
		}

		//case 14 (projectionAxis = axis[2].crossProduct(bbox.getAxis(1)))
		if(std::abs(distCenterPoint.dotProduct(axis[2].crossProduct(bbox.getAxis(1)))) - epsilon
			> std::abs(this->getHalfSize(0) * AdotB[1][1])
			+ std::abs(this->getHalfSize(1) * AdotB[0][1])
			+ std::abs(bbox.getHalfSize(0) * AdotB[2][2])
			+ std::abs(bbox.getHalfSize(2) * AdotB[2][0]))
		{
			return false;
		}

		//case 15 (projectionAxis = axis[2].crossProduct(bbox.getAxis(2)))
		if(std::abs(distCenterPoint.dotProduct(axis[2].crossProduct(bbox.getAxis(2)))) - epsilon
			> std::abs(this->getHalfSize(0) * AdotB[1][2])
			+ std::abs(this->getHalfSize(1) * AdotB[0][2])
			+ std::abs(bbox.getHalfSize(0) * AdotB[2][1])
			+ std::abs(bbox.getHalfSize(1) * AdotB[2][0]))
		{
			return false;
		}

		return true;
	}

	/**
	* @return True if the bounding box collides or is inside this bounding box
	*/
	template<class T> bool OBBox<T>::collideWithAABBox(const AABBox<T> &bbox) const
	{
		return collideWithOBBox(OBBox<T>(bbox));
	}

	template<class T> OBBox<T> operator *(const Matrix4<T> &m, const OBBox<T> &obb)
	{
		//projection matrix not accepted because result will not an oriented bounding box
		#ifdef _DEBUG
			assert(fabs(m(3,0)) < std::numeric_limits<T>::epsilon());
			assert(fabs(m(3,1)) < std::numeric_limits<T>::epsilon());
			assert(fabs(m(3,2)) < std::numeric_limits<T>::epsilon());
		#endif

		//build OBB
		Matrix3<float> m3 = m.toMatrix3();
		Vector3<T> halfSizes(
				(m3 * obb.getAxis(0) * obb.getHalfSize(0)).length(),
				(m3 * obb.getAxis(1) * obb.getHalfSize(1)).length(),
				(m3 * obb.getAxis(2) * obb.getHalfSize(2)).length()
		);
		Point3<T> centerPosition = (m * Point4<T>(obb.getCenterPosition())).toPoint3();
		Quaternion<T> orientation(m3);

		return OBBox<T>(halfSizes, centerPosition, orientation);
	}

	template<class T> OBBox<T> operator *(const OBBox<T> &obb, const Matrix4<T> &m)
	{
		return m * obb;
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const OBBox<T> &obbox)
	{
		stream.setf(std::ios::left);
		stream << "OBBox point 1: " << obbox.getPoint(0) << std::endl;
		stream << "OBBox point 2: " << obbox.getPoint(1) << std::endl;
		stream << "OBBox point 3: " << obbox.getPoint(2) << std::endl;
		stream << "OBBox point 4: " << obbox.getPoint(3) << std::endl;
		stream << "OBBox point 5: " << obbox.getPoint(4) << std::endl;
		stream << "OBBox point 6: " << obbox.getPoint(5) << std::endl;
		stream << "OBBox point 7: " << obbox.getPoint(6) << std::endl;
		stream << "OBBox point 8: " << obbox.getPoint(7) << std::endl;

		return stream;
	}

	//explicit template
	template class OBBox<float>;
	template OBBox<float> operator *<float>(const Matrix4<float> &, const OBBox<float> &);
	template OBBox<float> operator *<float>(const OBBox<float> &, const Matrix4<float> &);
	template std::ostream& operator <<<float>(std::ostream &, const OBBox<float> &);

}
