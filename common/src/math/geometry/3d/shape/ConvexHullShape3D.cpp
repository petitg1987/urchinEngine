#include <algorithm>

#include "ConvexHullShape3D.h"
#include "math/geometry/3d/util/ResizeConvexHull3DService.h"
#include "math/geometry/3d/object/ConvexHull3D.h"
#include "math/algebra/point/Point4.h"
#include "tools/logger/Logger.h"
#include "tools/logger/FileLogger.h"

namespace urchin
{
	/**
	 * @param points Points used to construct the convex hull shape. Points inside the convex hull shape are accepted but will unused.
	 */
	template<class T> ConvexHullShape3D<T>::ConvexHullShape3D(const std::vector<Point3<T>> &points) :
		nextPointIndex(0),
		nextTriangleIndex(0)
	{
		//build tetrahedron
		std::set<unsigned int> pointsToExclude = buildTetrahedron(points);

		//add each point to the tetrahedron
		for(unsigned int i=0;i<points.size();i++)
		{
			if(pointsToExclude.find(i)==pointsToExclude.end())
			{
				addNewPoint(points[i]);
			}
		}
	}

	/**
	 * @param points Points of the convex hull shape (all points must belong to the convex hull shape and indexes should start from 0 to points size - 1)
	 * @param indexedTriangles Triangles of the convex hull shape (the triangles must form a convex and indexes should start from 0 to triangle size - 1)
	 */
	template<class T> ConvexHullShape3D<T>::ConvexHullShape3D(const std::map<unsigned int, ConvexHullPoint<T>> &points, const std::map<unsigned int, IndexedTriangle3D<T>> &indexedTriangles) :
		nextPointIndex(points.size()),
		nextTriangleIndex(indexedTriangles.size()),
		points(points),
		indexedTriangles(indexedTriangles)
	{

	}

	template<class T> ConvexHullShape3D<T>::~ConvexHullShape3D()
	{

	}

	/**
	 * Points of convex hull shape indexed to be used with indexed triangles.
	 */
	template<class T> const typename std::map<unsigned int, ConvexHullPoint<T>> &ConvexHullShape3D<T>::getConvexHullPoints() const
	{
		return points;
	}

	/**
	 * Points of convex hull shape. Order of points is undetermined.
	 */
	template<class T> std::vector<Point3<T>> ConvexHullShape3D<T>::getPoints() const
	{
		std::vector<Point3<T>> result;
		result.reserve(points.size());

		for(const auto &it : points)
		{
			result.push_back(it.second.point);
		}

		return result;
	}

	/**
	 * Triangles of convex hull shape where points are sorted in counter clockwise direction in a right hand coordinate system (Z+ directed to the observer).
	 */
	template<class T> const typename std::map<unsigned int, IndexedTriangle3D<T>> &ConvexHullShape3D<T>::getIndexedTriangles() const
	{
		return indexedTriangles;
	}

	/**
	 * @return Returns index of point added. If point doesn't make part of convex, result is zero.
	 */
	template<class T> unsigned int ConvexHullShape3D<T>::addNewPoint(const Point3<T> &newPoint)
	{
		std::vector<unsigned int> removedTriangleIndexes;
		removedTriangleIndexes.reserve(4);

		return addNewPoint(newPoint, removedTriangleIndexes);
	}

	/**
	* @param removedTriangleIndexes [out] Indexes of removed triangles from convex hull shape
	* @return Returns index of point added. If point doesn't make part of convex, result is zero.
	*/
	template<class T> unsigned int ConvexHullShape3D<T>::addNewPoint(const Point3<T> &newPoint, std::vector<unsigned int> &removedTriangleIndexes)
	{
		std::map<long long, std::pair<unsigned int, unsigned int>> edges;
		constexpr int HALF_SIZE_INDEX = (sizeof(unsigned int) * 8) / 2;

		//deletes all triangles visible by the new point
		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::iterator itTriangle=indexedTriangles.begin(); itTriangle!=indexedTriangles.end();)
		{
			const IndexedTriangle3D<T> indexedTriangle = itTriangle->second;
			const Vector3<T> &triangleNormal = indexedTriangle.computeNormal(
					points.at(indexedTriangle.getIndex(0)).point,
					points.at(indexedTriangle.getIndex(1)).point,
					points.at(indexedTriangle.getIndex(2)).point);

			const Point3<T> &point0 = points.at(indexedTriangle.getIndex(0)).point;
			const Vector3<T> &triangleToPoint = point0.vector(newPoint);

			if(triangleNormal.dotProduct(triangleToPoint) > 0.0)
			{
				for(int i=0; i<3; i++)
				{ //each edge
					int index1 = indexedTriangle.getIndexes()[i];
					int index2 = indexedTriangle.getIndexes()[(i+1)%3];

					long long idEdge = (std::min(index1, index2) << HALF_SIZE_INDEX) | std::max(index1, index2);
					std::map<long long, std::pair<unsigned int, unsigned int>>::iterator itEdge = edges.find(idEdge);
					if(itEdge==edges.end())
					{
						edges[idEdge] = std::make_pair(index1, index2);
					}else
					{
						edges.erase(itEdge);
					}
				}

				removedTriangleIndexes.push_back(itTriangle->first);
				removeTriangle(itTriangle++);
			}else
			{
				++itTriangle;
			}
		}

		//adds the new triangles
		if(edges.size()>0)
		{
			unsigned int newPointIndex = nextPointIndex++;
			points[newPointIndex].point = newPoint;

			for(std::map<long long, std::pair<unsigned int, unsigned int>>::iterator it = edges.begin(); it!=edges.end(); ++it)
			{
				addTriangle(IndexedTriangle3D<T>(it->second.first, it->second.second, newPointIndex));
			}

			return newPointIndex;
		}

		return 0;
	}

	template<class T> Point3<T> ConvexHullShape3D<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		T maxPointDotDirection = points.begin()->second.point.toVector().dotProduct(direction);
		Point3<T> maxPoint = points.begin()->second.point;

		for(const auto &itPoints : points)
		{
			T currentPointDotDirection  = itPoints.second.point.toVector().dotProduct(direction);
			if(currentPointDotDirection > maxPointDotDirection)
			{
				maxPointDotDirection = currentPointDotDirection;
				maxPoint = itPoints.second.point;
			}
		}

		return maxPoint;
	}

	/**
	 * @param distance All planes of convex hull shape will be moved along their normal to the specified distance.
	 * Positive distance will extend convex hull shape and negative distance will shrink the convex hull shape.
	 * @return Convex hull shape resized. If resize is impossible to keep plane normal outside convex hull shape: nullptr is returned.
	 */
	template<class T> std::unique_ptr<ConvexHullShape3D<T>> ConvexHullShape3D<T>::resize(T distance) const
	{
		return ResizeConvexHull3DService<T>::instance()->resizeConvexHullShape(*this, distance);
	}

	template<class T> std::unique_ptr<ConvexObject3D<T>> ConvexHullShape3D<T>::toConvexObject(const Transform<T> &transform) const
	{
		std::map<unsigned int, ConvexHullPoint<T>> transformedConvexHullPoints = points;
		for(auto &it : transformedConvexHullPoints)
		{
			it.second.point = (transform.getTransformMatrix() * Point4<T>(it.second.point)).toPoint3();
		}

		return std::make_unique<ConvexHull3D<T>>(ConvexHullShape3D<T>(transformedConvexHullPoints, indexedTriangles));
	}

	template<class T> void ConvexHullShape3D<T>::addTriangle(const IndexedTriangle3D<T> &indexedTriangle)
	{
		//add indexed triangles to triangles map
		unsigned int triangleIndex = nextTriangleIndex++;
		indexedTriangles.insert(std::pair<unsigned int, IndexedTriangle3D<T>>(triangleIndex, indexedTriangle));

		//add triangles reference on points
		for(unsigned int i=0; i<3; i++)
		{
			points[indexedTriangle.getIndex(i)].triangles.push_back(triangleIndex);
		}
	}

	template<class T> void ConvexHullShape3D<T>::removeTriangle(const typename std::map<unsigned int, IndexedTriangle3D<T>>::iterator &itTriangle)
	{
		//remove reference of triangles on points
		const unsigned int *indexes = itTriangle->second.getIndexes();
		for(unsigned int i=0;i<3;i++)
		{
			std::vector<unsigned int> &pointTriangles = points[indexes[i]].triangles;
			pointTriangles.erase(std::remove(pointTriangles.begin(), pointTriangles.end(), itTriangle->first), pointTriangles.end());
		}

		//remove indexed triangles from triangle map
		indexedTriangles.erase(itTriangle);
	}

	/**
	 * @return All indexes points used to build the tetrahedron
	 */
	template<class T> std::set<unsigned int> ConvexHullShape3D<T>::buildTetrahedron(const std::vector<Point3<T>> &points)
	{
		//1. initialize
		std::set<unsigned int> pointsUsed;

		//2. build a point (use first point)
		if(points.size()<1)
		{
			throw buildException(points, pointsUsed);
		}

		this->points[nextPointIndex++].point = points[0];
		pointsUsed.insert(0);

		//3. build a line (find two distinct points)
		for(unsigned int i=1; i<points.size(); i++)
		{
			if(points[i]!=this->points[0].point)
			{
				this->points[nextPointIndex++].point = points[i];
				pointsUsed.insert(i);
				break;
			}
		}

		if(pointsUsed.size()!=2)
		{
			throw buildException(points, pointsUsed);
		}

		//4. build triangles (find a point which doesn't belong to line).
		Vector3<T> lineVector = this->points[0].point.vector(this->points[1].point);
		for(unsigned int i=1; i<points.size(); i++)
		{
			if(pointsUsed.find(i)!=pointsUsed.end())
			{ //point already used to build the tetrahedron
				continue;
			}

			Vector3<T> linesCrossProduct = lineVector.crossProduct(this->points[0].point.vector(points[i]));
			if(linesCrossProduct.X != (T)0.0 || linesCrossProduct.Y != (T)0.0 || linesCrossProduct.Z != (T)0.0)
			{
				this->points[nextPointIndex++].point = points[i];
				pointsUsed.insert(i);
				break;
			}
		}

		if(pointsUsed.size()!=3)
		{
			throw buildException(points, pointsUsed);
		}

		unsigned int triangleIndex1 = nextTriangleIndex++;
		unsigned int triangleIndex2 = nextTriangleIndex++;
		indexedTriangles.insert(std::pair<unsigned int, IndexedTriangle3D<T>>(triangleIndex1, IndexedTriangle3D<T>(0, 1, 2)));
		indexedTriangles.insert(std::pair<unsigned int, IndexedTriangle3D<T>>(triangleIndex2, IndexedTriangle3D<T>(0, 2, 1)));
		for(auto &it : this->points)
		{
			it.second.triangles.push_back(triangleIndex1);
			it.second.triangles.push_back(triangleIndex2);
		}

		//5. build tetrahedron (find a no coplanar point to the triangle)
		const IndexedTriangle3D<T> firstIndexedTriangle = this->indexedTriangles.at(0);
		const Vector3<T> &firstTriangleNormal = firstIndexedTriangle.computeNormal(
				this->points.at(firstIndexedTriangle.getIndex(0)).point,
				this->points.at(firstIndexedTriangle.getIndex(1)).point,
				this->points.at(firstIndexedTriangle.getIndex(2)).point);
		const Point3<T> &firstPoint = this->points.at(0).point;
		for(unsigned int i=1;i<points.size();i++)
		{
			if(pointsUsed.find(i)!=pointsUsed.end())
			{ //point already used to build the tetrahedron
				continue;
			}

			const Vector3<T> &triangleToPoint = firstPoint.vector(points[i]);
			if(firstTriangleNormal.dotProduct(triangleToPoint) != (T)0.0)
			{
				addNewPoint(points[i]);
				pointsUsed.insert(i);
				break;
			}
		}

		if(pointsUsed.size()!=4)
		{
			throw buildException(points, pointsUsed);
		}

		return pointsUsed;
	}

	/**
	 * Build an exception during building of tetrahedron
	 * @param pointsUsed Points currently used to construct the tetrahedron
	 * @return Exception during building of tetrahedron
	 */
	template<class T> std::invalid_argument ConvexHullShape3D<T>::buildException(const std::vector<Point3<T>> &points, const std::set<unsigned int> &pointsUsed)
	{
		std::string formName;
		if(pointsUsed.size()==0)
		{
			formName = "empty set";
		}else if(pointsUsed.size()==1)
		{
			formName = "point";
		}else if(pointsUsed.size()==2)
		{
			formName = "line";
		}else if(pointsUsed.size()==3)
		{
			formName = "plane";
		}else
		{
			std::ostringstream oss;
			oss << pointsUsed.size();
			return std::invalid_argument("Number of points used to build the tetrahedron unsupported: " + oss.str() + ".");
		}

		//log points in error log file
		std::stringstream logStream;
		if(points.size()>0)
		{
			logStream<<"Impossible to build a convex hull shape with following points:"<<std::endl;
			for (unsigned int i=0; i<points.size(); ++i)
			{
				logStream << " - " << points[i] << std::endl;
			}
		}else
		{
			logStream<<"Impossible to build a convex hull shape with zero point.";
		}
		Logger::logger().logError(logStream.str());

		return std::invalid_argument("Impossible to build the convex hull shape. All points form a " + formName + ".");
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const ConvexHullShape3D<T> &ch)
	{
		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::const_iterator it = ch.getIndexedTriangles().begin(); it!=ch.getIndexedTriangles().end(); ++it)
		{
			stream << "Triangle "<< it->first <<": "
				<<"("<<ch.getConvexHullPoints().at(it->second.getIndex(0)).point<<") "
				<<"("<<ch.getConvexHullPoints().at(it->second.getIndex(1)).point<<") "
				<<"("<<ch.getConvexHullPoints().at(it->second.getIndex(2)).point<<") "
				<<std::endl;
		}

		return stream;
	}

	//explicit template
	template class ConvexHullShape3D<float>;
	template std::ostream& operator <<<float>(std::ostream &, const ConvexHullShape3D<float> &);
	template struct ConvexHullPoint<float>;

	template class ConvexHullShape3D<double>;
	template std::ostream& operator <<<double>(std::ostream &, const ConvexHullShape3D<double> &);
	template struct ConvexHullPoint<double>;

}
