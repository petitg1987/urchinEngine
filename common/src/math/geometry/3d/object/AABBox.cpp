#include "AABBox.h"

namespace urchin
{

	template<class T> AABBox<T>::AABBox():
		boxShape(BoxShape<T>(Vector3<T>(0.0, 0.0, 0.0))),
		min(Point3<T>(0.0, 0.0, 0.0)),
		max(Point3<T>(0.0, 0.0, 0.0))
	{

	}

	template<class T> AABBox<T>::AABBox(const Point3<T> &min,  const Point3<T> &max):
		boxShape(BoxShape<T>(Vector3<T>((max.X-min.X)/2.0, (max.Y-min.Y)/2.0, (max.Z-min.Z)/2.0))),
		min(min),
		max(max)
	{

	}

	template<class T> AABBox<T>::AABBox(const Point3<T> &min,  const Vector3<T> &diagonal):
		boxShape(BoxShape<T>(Vector3<T>(diagonal.X/2.0, diagonal.Y/2.0, diagonal.Z/2.0))),
		min(min),
		max(min.translate(diagonal))
	{

	}

	template<class T> AABBox<T>::~AABBox()
	{

	}

	template<class T> const T AABBox<T>::getHalfSize(unsigned int index) const
	{
		return boxShape.getHalfSize(index);
	}

	template<class T> const Vector3<T> &AABBox<T>::getHalfSizes() const
	{
		return boxShape.getHalfSizes();
	}

	template<class T> const T AABBox<T>::getMaxHalfSize() const
	{
		return std::max(std::max(boxShape.getHalfSize(0), boxShape.getHalfSize(1)), boxShape.getHalfSize(2));
	}

	template<class T> const Point3<T> &AABBox<T>::getMin() const
	{
		return min;
	}

	template<class T> const Point3<T> &AABBox<T>::getMax() const
	{
		return max;
	}

	template<class T> Point3<T> AABBox<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		const int signX = direction.X < 0.0 ? 0 : 1;
		const int signY = direction.Y < 0.0 ? 0 : 1;
		const int signZ = direction.Z < 0.0 ? 0 : 1;

		return min + Point3<T>(
				signX * boxShape.getHalfSize(0) * 2.0,
				signY * boxShape.getHalfSize(1) * 2.0,
				signZ * boxShape.getHalfSize(2) * 2.0);
	}

	template<class T> AABBox<T> AABBox<T>::moveAABBox(const Transform<T> &transform) const
	{
		return transform.getTransformMatrix() * (*this);
	}

	/**
	 * @return Orthogonal projection matrix based on AABBox
	 */
	template<class T> Matrix4<T> AABBox<T>::toProjectionMatrix() const
	{
		float tx = -((max.X+min.X)/(max.X-min.X));
		float ty = -((max.Y+min.Y)/(max.Y-min.Y));
		float tz = -((-min.Z-max.Z)/(-min.Z+max.Z));

		return Matrix4<T>(
			2.0f/(max.X-min.X), 0.0, 0.0, tx,
			0.0, 2.0f/(max.Y-min.Y), 0.0, ty,
			0.0, 0.0, -2.0f/(-min.Z+max.Z), tz,
			0.0, 0.0, 0.0, 1.0);
	}

	template<class T> AABBox<T> AABBox<T>::merge(const AABBox<T> &aabbox) const
	{
		Point3<T> mergedMin(
			min.X<aabbox.getMin().X ? min.X : aabbox.getMin().X,
			min.Y<aabbox.getMin().Y ? min.Y : aabbox.getMin().Y,
			min.Z<aabbox.getMin().Z ? min.Z : aabbox.getMin().Z
		);

		Point3<T> mergedMax(
			max.X>aabbox.getMax().X ? max.X : aabbox.getMax().X,
			max.Y>aabbox.getMax().Y ? max.Y : aabbox.getMax().Y,
			max.Z>aabbox.getMax().Z ? max.Z : aabbox.getMax().Z
		);

		return AABBox<T>(mergedMin, mergedMax);
	}

	/**
	 * @return Bounding box cut to maximum size of bounding box given in input
	 */
	template<class T> AABBox<T> AABBox<T>::cutTo(const AABBox<T> &aabbox) const
	{
		Point3<T> cutMin(
			min.X<aabbox.getMin().X ? aabbox.getMin().X : min.X,
			min.Y<aabbox.getMin().Y ? aabbox.getMin().Y : min.Y,
			min.Z<aabbox.getMin().Z ? aabbox.getMin().Z : min.Z
		);

		Point3<T> cutMax(
			max.X>aabbox.getMax().X ? aabbox.getMax().X : max.X,
			max.Y>aabbox.getMax().Y ? aabbox.getMax().Y : max.Y,
			max.Z>aabbox.getMax().Z ? aabbox.getMax().Z : max.Z
		);

		return AABBox<T>(cutMin, cutMax);
	}

	template<class T> const std::vector<Point3<T>> AABBox<T>::getEightPoints() const
	{
		std::vector<Point3<T>> result;
		result.reserve(8);

		result.push_back(Point3<T>(max.X, max.Y, max.Z));
		result.push_back(Point3<T>(max.X, min.Y, max.Z));
		result.push_back(Point3<T>(min.X, min.Y, max.Z));
		result.push_back(Point3<T>(min.X, max.Y, max.Z));
		result.push_back(Point3<T>(max.X, max.Y, min.Z));
		result.push_back(Point3<T>(max.X, min.Y, min.Z));
		result.push_back(Point3<T>(min.X, min.Y, min.Z));
		result.push_back(Point3<T>(min.X, max.Y, min.Z));

		return result;
	}

	template<class T> bool AABBox<T>::collideWithPoint(const Point3<T> &point) const
	{
		if(	point.X > min.X && point.Y > min.Y && point.Z > min.Z &&
				point.X < max.X && point.Y < max.Y && point.Z < max.Z)
		{
			return true;
		}

		return false;
	}

	/**
	* @return True if the bounding box collides or is inside this bounding box
	*/
	template<class T> bool AABBox<T>::collideWithAABBox(const AABBox<T> &bbox) const
	{
		if(bbox.getMin().X < max.X && bbox.getMax().X > min.X &&
				bbox.getMin().Y < max.Y && bbox.getMax().Y > min.Y &&
				bbox.getMin().Z < max.Z && bbox.getMax().Z > min.Z)
		{
			return true;
		}

		return false;
	}

	template<class T> AABBox<T> operator *(const Matrix4<T> &m, const AABBox<T> &aabb)
	{
		//projection matrix not accepted
		#ifdef _DEBUG
			assert(fabs(m(3,0)) < std::numeric_limits<T>::epsilon());
			assert(fabs(m(3,1)) < std::numeric_limits<T>::epsilon());
			assert(fabs(m(3,2)) < std::numeric_limits<T>::epsilon());
		#endif

		Point3<T> xa = Point3<T>(m(0,0), m(1,0), m(2,0)) * aabb.getMin().X;
		Point3<T> xb = Point3<T>(m(0,0), m(1,0), m(2,0)) * aabb.getMax().X;
		Point3<T> minX = Point3<T>(xa.X<xb.X ? xa.X : xb.X, xa.Y<xb.Y ? xa.Y : xb.Y, xa.Z<xb.Z ? xa.Z : xb.Z);
		Point3<T> maxX = Point3<T>(xa.X>xb.X ? xa.X : xb.X, xa.Y>xb.Y ? xa.Y : xb.Y, xa.Z>xb.Z ? xa.Z : xb.Z);

		Point3<T> ya = Point3<T>(m(0,1), m(1,1), m(2,1)) * aabb.getMin().Y;
		Point3<T> yb = Point3<T>(m(0,1), m(1,1), m(2,1)) * aabb.getMax().Y;
		Point3<T> minY = Point3<T>(ya.X<yb.X ? ya.X : yb.X, ya.Y<yb.Y ? ya.Y : yb.Y, ya.Z<yb.Z ? ya.Z : yb.Z);
		Point3<T> maxY = Point3<T>(ya.X>yb.X ? ya.X : yb.X, ya.Y>yb.Y ? ya.Y : yb.Y, ya.Z>yb.Z ? ya.Z : yb.Z);

		Point3<T> za = Point3<T>(m(0,2), m(1,2), m(2,2)) * aabb.getMin().Z;
		Point3<T> zb = Point3<T>(m(0,2), m(1,2), m(2,2)) * aabb.getMax().Z;
		Point3<T> minZ = Point3<T>(za.X<zb.X ? za.X : zb.X, za.Y<zb.Y ? za.Y : zb.Y, za.Z<zb.Z ? za.Z : zb.Z);
		Point3<T> maxZ = Point3<T>(za.X>zb.X ? za.X : zb.X, za.Y>zb.Y ? za.Y : zb.Y, za.Z>zb.Z ? za.Z : zb.Z);

		Point3<T> translation(m(0,3), m(1,3), m(2,3));

		return AABBox<T>(minX+minY+minZ+translation, maxX+maxY+maxZ+translation);
	}

	template<class T> AABBox<T> operator *(const AABBox<T> &aabb, const Matrix4<T> &m)
	{
		return m * aabb;
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const AABBox<T> &aabbox)
	{
		stream.setf(std::ios::left);
		stream << "AABBox min point: " << aabbox.getMin() << std::endl;
		stream << "AABBox max point: " << aabbox.getMax() << std::endl;

		return stream;
	}

	//explicit template
	template class AABBox<float>;
	template AABBox<float> operator *<float>(const Matrix4<float> &, const AABBox<float> &);
	template AABBox<float> operator *<float>(const AABBox<float> &, const Matrix4<float> &);
	template std::ostream& operator <<<float>(std::ostream &, const AABBox<float> &);

	template class AABBox<double>;
	template AABBox<double> operator *<double>(const Matrix4<double> &, const AABBox<double> &);
	template AABBox<double> operator *<double>(const AABBox<double> &, const Matrix4<double> &);
	template std::ostream& operator <<<double>(std::ostream &, const AABBox<double> &);
}
