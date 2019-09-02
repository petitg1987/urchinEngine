#include <cassert>
#include <limits>

#include "AABBox.h"

namespace urchin
{

	template<class T> AABBox<T>::AABBox():
		boxShape(BoxShape<T>(Vector3<T>(0.0, 0.0, 0.0))),
		min(Point3<T>(0.0, 0.0, 0.0)),
		max(Point3<T>(0.0, 0.0, 0.0))
	{

	}

	template<class T> AABBox<T>::AABBox(const Point3<T> &min, const Point3<T> &max) :
		boxShape(BoxShape<T>(Vector3<T>((max.X-min.X)/2.0, (max.Y-min.Y)/2.0, (max.Z-min.Z)/2.0))),
		min(min),
		max(max)
	{
		#ifdef _DEBUG
			assert(min.X <= max.X);
			assert(min.Y <= max.Y);
			assert(min.Z <= max.Z);
    	#endif
	}

	template<class T> AABBox<T>::AABBox(const Point3<T> &min, const Vector3<T> &diagonal) :
		boxShape(BoxShape<T>(Vector3<T>(diagonal.X/2.0, diagonal.Y/2.0, diagonal.Z/2.0))),
		min(min),
		max(min.translate(diagonal))
	{
		#ifdef _DEBUG
			assert(min.X <= max.X);
			assert(min.Y <= max.Y);
			assert(min.Z <= max.Z);
		#endif
	}

	template<class T> AABBox<T>::AABBox(const std::vector<Point3<T>> &points) :
            min(Point3<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max())),
            max(Point3<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max()))
	{
		for(const auto &point : points)
		{
			min.X = std::min(min.X, point.X);
			min.Y = std::min(min.Y, point.Y);
			min.Z = std::min(min.Z, point.Z);

			max.X = std::max(max.X, point.X);
			max.Y = std::max(max.Y, point.Y);
			max.Z = std::max(max.Z, point.Z);
		}

		boxShape = BoxShape<T>(Vector3<T>((max.X-min.X)/2.0, (max.Y-min.Y)/2.0, (max.Z-min.Z)/2.0));
	}

	template<class T> AABBox<T>::AABBox(const Point3<T> *points, unsigned int size) :
            min(Point3<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max())),
            max(Point3<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max()))
	{
		for(unsigned int i=0; i<size; ++i)
		{
			const Point3<T> &point = points[i];

			min.X = std::min(min.X, point.X);
			min.Y = std::min(min.Y, point.Y);
			min.Z = std::min(min.Z, point.Z);

			max.X = std::max(max.X, point.X);
			max.Y = std::max(max.Y, point.Y);
			max.Z = std::max(max.Z, point.Z);
		}

		boxShape = BoxShape<T>(Vector3<T>((max.X-min.X)/2.0, (max.Y-min.Y)/2.0, (max.Z-min.Z)/2.0));
	}

	template<class T> AABBox<T>::AABBox(const AABBox<T> &aabBox) :
			boxShape(aabBox.getHalfSizes()),
			min(aabBox.getMin()),
			max(aabBox.getMax())
	{

	}

	template<class T> T AABBox<T>::getHalfSize(unsigned int index) const
	{
		return boxShape.getHalfSize(index);
	}

	template<class T> const Vector3<T> &AABBox<T>::getHalfSizes() const
	{
		return boxShape.getHalfSizes();
	}

	template<class T> T AABBox<T>::getMaxHalfSize() const
	{
		return boxShape.getMaxHalfSize();
	}

	template<class T> unsigned int AABBox<T>::getMaxHalfSizeIndex() const
	{
		return boxShape.getMaxHalfSizeIndex();
	}

	template<class T> T AABBox<T>::getMinHalfSize() const
	{
		return boxShape.getMinHalfSize();
	}

	template<class T> unsigned int AABBox<T>::getMinHalfSizeIndex() const
	{
		return boxShape.getMinHalfSizeIndex();
	}

	template<class T> const Point3<T> &AABBox<T>::getMin() const
	{
		return min;
	}

	template<class T> const Point3<T> &AABBox<T>::getMax() const
	{
		return max;
	}

	template<class T> Point3<T> AABBox<T>::getCenterOfMass() const
	{
		return min.translate(boxShape.getHalfSizes());
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

	template<class T> std::vector<Point3<T>> AABBox<T>::getPoints() const
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

	template<class T> T AABBox<T>::getVolume() const
	{
		return boxShape.getVolume();
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

	template<class T> bool AABBox<T>::include(const AABBox<T> &aabb) const
	{
		return aabb.getMin().X > min.X && aabb.getMax().X < max.X &&
				aabb.getMin().Y > min.Y && aabb.getMax().Y < max.Y &&
				aabb.getMin().Z > min.Z && aabb.getMax().Z < max.Z;
	}

	template<class T> AABBox<T> AABBox<T>::initMergeableAABBox()
	{
		AABBox aabbox;

		aabbox.min.X = std::numeric_limits<T>::max();
		aabbox.min.Y = std::numeric_limits<T>::max();
		aabbox.min.Z = std::numeric_limits<T>::max();

		aabbox.max.X = -std::numeric_limits<T>::max();
		aabbox.max.Y = -std::numeric_limits<T>::max();
		aabbox.max.Z = -std::numeric_limits<T>::max();

		return aabbox;
	}

	template<class T> AABBox<T> AABBox<T>::merge(const AABBox<T> &aabb) const
	{
		Point3<T> mergedMin(std::min(min.X, aabb.getMin().X), std::min(min.Y, aabb.getMin().Y), std::min(min.Z, aabb.getMin().Z));
		Point3<T> mergedMax(std::max(max.X, aabb.getMax().X), std::max(max.Y, aabb.getMax().Y), std::max(max.Z, aabb.getMax().Z));

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

	template<class T> AABBox<T> AABBox<T>::enlarge(const Vector3<T> &minAdditionalSize, const Vector3<T> &maxAdditionalSize) const
	{
		return AABBox<T>(getMin()-Point3<T>(minAdditionalSize), getMax()+Point3<T>(maxAdditionalSize));
	}

	template<class T> AABBox<T> AABBox<T>::enlarge(T minAdditionalSize, T maxAdditionalSize) const
	{
		return AABBox<T>(getMin()-Point3<T>(minAdditionalSize, minAdditionalSize, minAdditionalSize), getMax()+Point3<T>(maxAdditionalSize, maxAdditionalSize, maxAdditionalSize));
	}

	template<class T> bool AABBox<T>::collideWithPoint(const Point3<T> &point) const
	{
		return point.X > min.X && point.Y > min.Y && point.Z > min.Z &&
            point.X < max.X && point.Y < max.Y && point.Z < max.Z;
	}

	/**
	* @return True if the bounding box collides or is inside this bounding box
	*/
	template<class T> bool AABBox<T>::collideWithAABBox(const AABBox<T> &aabb) const
	{
		return aabb.getMin().X < max.X && aabb.getMax().X > min.X &&
				aabb.getMin().Y < max.Y && aabb.getMax().Y > min.Y &&
				aabb.getMin().Z < max.Z && aabb.getMax().Z > min.Z;
	}

	/**
	 * @return True if the ray is inside or partially inside the bounding box
	 */
	template<class T> bool AABBox<T>::collideWithRay(const Ray<T> &ray) const
	{
		T lengthToMinPlane = ((*this)[ray.getDirectionSign(0)].X - ray.getOrigin().X) * ray.getInverseDirection().X;
		T lengthToMaxPlane = ((*this)[1-ray.getDirectionSign(0)].X - ray.getOrigin().X) * ray.getInverseDirection().X;

		T lengthToMinYPlane = ((*this)[ray.getDirectionSign(1)].Y - ray.getOrigin().Y) * ray.getInverseDirection().Y;
		T lengthToMaxYPlane = ((*this)[1-ray.getDirectionSign(1)].Y - ray.getOrigin().Y) * ray.getInverseDirection().Y;


		if(lengthToMinPlane > lengthToMaxYPlane || lengthToMinYPlane > lengthToMaxPlane)
		{
			return false;
		}

		if(lengthToMinYPlane > lengthToMinPlane)
		{
			lengthToMinPlane = lengthToMinYPlane;
		}
		if(lengthToMaxYPlane < lengthToMaxPlane)
		{
			lengthToMaxPlane = lengthToMaxYPlane;
		}

		T lengthToMinZPlane = ((*this)[ray.getDirectionSign(2)].Z - ray.getOrigin().Z) * ray.getInverseDirection().Z;
		T lengthToMaxZPlane = ((*this)[1-ray.getDirectionSign(2)].Z - ray.getOrigin().Z) * ray.getInverseDirection().Z;

		if(lengthToMinPlane > lengthToMaxZPlane || lengthToMinZPlane > lengthToMaxPlane)
		{
			return false;
		}

		if(lengthToMinZPlane > lengthToMinPlane)
		{
			lengthToMinPlane = lengthToMinZPlane;
		}
		if(lengthToMaxZPlane < lengthToMaxPlane)
		{
			lengthToMaxPlane = lengthToMaxZPlane;
		}

		return lengthToMinPlane < ray.getLength() && lengthToMaxPlane > 0.0;
	}

	/**
	 * Allow to access to min and max value as an array (const).
	 */
	template<class T> const Point3<T>& AABBox<T>::operator [](std::size_t i) const
	{
		return (&min)[i];
	}

	/**
	 * Allow to access to min and max value as an array (no const).
	 */
	template<class T> Point3<T>& AABBox<T>::operator [](std::size_t i)
	{
		return (&min)[i];
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
