#ifndef ENGINE_CONVEXHULLSHAPE3D_H
#define ENGINE_CONVEXHULLSHAPE3D_H

#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <sstream>
#include <memory>

#include "math/geometry/3d/shape/ConvexShape3D.h"
#include "math/geometry/3d/IndexedTriangle3D.h"
#include "math/algebra/point/Point3.h"

namespace urchin
{

	template<class T> class ConvexHullShape3D : public ConvexShape3D<T>
	{
		public:
			ConvexHullShape3D(const std::vector<Point3<T>> &);
			ConvexHullShape3D(const std::map<unsigned int, Point3<T>> &, const std::map<unsigned int, IndexedTriangle3D<T>> &);
			~ConvexHullShape3D();

			const typename std::map<unsigned int, IndexedTriangle3D<T>> &getIndexedTriangles() const;
			const typename std::map<unsigned int, Point3<T>> &getIndexedPoints() const;
			std::vector<Point3<T>> getPoints() const;

			unsigned int addNewPoint(const Point3<T> &);
			unsigned int addNewPoint(const Point3<T> &, std::vector<unsigned int> &, std::vector<unsigned int> &);

			Point3<T> getSupportPoint(const Vector3<T> &) const;

			std::unique_ptr<ConvexHullShape3D<T>> resize(T) const;
			std::unique_ptr<ConvexObject3D<T>> toConvexObject(const Transform<T> &) const;

		private:
			unsigned int addTriangle(const IndexedTriangle3D<T> &);
			void removeTriangle(const typename std::map<unsigned int, IndexedTriangle3D<T>>::iterator &);
			std::set<unsigned int> buildTetrahedron(const std::vector<Point3<T>> &);
			std::invalid_argument buildException(const std::vector<Point3<T>> &, const std::set<unsigned int> &);

			unsigned int nextPointIndex; //next index to use to store points in 'points' map
			unsigned int nextTriangleIndex; //next index to use to store triangles in 'indexedTriangles' map

			std::map<unsigned int, Point3<T>> points; //first: point index, second: point
			std::map<unsigned int, IndexedTriangle3D<T>> indexedTriangles; //first: triangle index, second: triangle representing the convex hull
			std::map<unsigned int, unsigned int> nbTrianglesByPoint; //first: point index, second: number of triangles attached to the point
	};

	template<class T> std::ostream& operator <<(std::ostream &, const ConvexHullShape3D<T> &);

}

#endif
