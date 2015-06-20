#include "ConvexHull3D.h"

namespace urchin
{

	/**
	 * @param points Points used to construct the convex hull. Points inside the convex hull are accepted but will unused.
	 */
	template<class T> ConvexHull3D<T>::ConvexHull3D(const std::vector<Point3<T>> &points) :
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
	 * @param points Points of the convex hull (all points must belong to the convex hull and indexes should start from 0 to points size - 1)
	 * @param indexedTriangles Triangles of the convex hull (the triangles must form a convex and indexes should start from 0 to triangle size - 1)
	 */
	template<class T> ConvexHull3D<T>::ConvexHull3D(const std::map<unsigned int, Point3<T>> &points, const std::map<unsigned int, IndexedTriangle3D<T>> &indexedTriangles) :
		nextPointIndex(points.size()),
		nextTriangleIndex(0)
	{
		this->points = points;

		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::const_iterator it = indexedTriangles.begin(); it!=indexedTriangles.end(); ++it)
		{
			addTriangle(it->second);
		}
	}

	template<class T> ConvexHull3D<T>::~ConvexHull3D()
	{

	}

	template<class T> const typename std::map<unsigned int, IndexedTriangle3D<T>> &ConvexHull3D<T>::getIndexedTriangles() const
	{
		return indexedTriangles;
	}

	template<class T> const typename std::map<unsigned int, Point3<T>> &ConvexHull3D<T>::getIndexedPoints() const
	{
		return points;
	}

	template<class T> std::vector<Point3<T>> ConvexHull3D<T>::getPoints() const
	{
		std::vector<Point3<T>> result;
		result.reserve(points.size());

		for(typename std::map<unsigned int, Point3<T>>::const_iterator it=points.begin(); it!=points.end(); ++it)
		{
			result.push_back(it->second);
		}

		return result;
	}

	/**
	 * @return Returns index of point added. If point doesn't make part of convex, result is zero.
	 */
	template<class T> unsigned int ConvexHull3D<T>::addNewPoint(const Point3<T> &newPoint)
	{
		std::vector<unsigned int> newTriangleIndexes;
		std::vector<unsigned int> removedTriangleIndexes;
		return addNewPoint(newPoint, newTriangleIndexes, removedTriangleIndexes);
	}

	/**
	* @param newTriangleIndexes [out] Indexes of new triangles added to convex hull
	* @param removedTriangleIndexes [out] Indexes of removed triangles from convex hull
	* @return Returns index of point added. If point doesn't make part of convex, result is zero.
	*/
	template<class T> unsigned int ConvexHull3D<T>::addNewPoint(const Point3<T> &newPoint,
			std::vector<unsigned int> &newTriangleIndexes, std::vector<unsigned int> &removedTriangleIndexes)
	{
		std::map<long long, std::pair<unsigned int, unsigned int>> edges;
		const int HALF_SIZE_INDEX = (sizeof(unsigned int) * 8) / 2;

		//deletes all triangles visible by the new point
		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::iterator itTriangle=indexedTriangles.begin(); itTriangle!=indexedTriangles.end();)
		{
			const IndexedTriangle3D<T> indexedTriangle = itTriangle->second;
			const Vector3<T> &triangleNormal = indexedTriangle.computeNormal(points);

			const Point3<T> &point0 = points.find(indexedTriangle.getIndexes()[0])->second;
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
			newTriangleIndexes.reserve(edges.size());
			unsigned int pointIndex = nextPointIndex++;
			points[pointIndex] = newPoint;

			for(std::map<long long, std::pair<unsigned int, unsigned int>>::iterator it = edges.begin(); it!=edges.end(); ++it)
			{
				unsigned int triangleIndex = addTriangle(IndexedTriangle3D<T>(it->second.first, it->second.second, pointIndex));
				newTriangleIndexes.push_back(triangleIndex);
			}

			return pointIndex;
		}

		return 0;
	}

	template<class T> Point3<T> ConvexHull3D<T>::getSupportPoint(const Vector3<T> &direction) const
	{
		T maxPointDotDirection = points.begin()->second.toVector().dotProduct(direction);
		Point3<T> maxPoint = points.begin()->second;

		for(typename std::map<unsigned int, Point3<T>>::const_iterator itPoints= ++points.begin(); itPoints!=points.end(); ++itPoints)
		{
			T currentPointDotDirection  = itPoints->second.toVector().dotProduct(direction);
			if(currentPointDotDirection > maxPointDotDirection)
			{
				maxPointDotDirection = currentPointDotDirection;
				maxPoint = itPoints->second;
			}
		}

		return maxPoint;
	}

	template<class T> unsigned int ConvexHull3D<T>::addTriangle(const IndexedTriangle3D<T> &indexedTriangle)
	{
		//add indexed triangles to triangle map
		unsigned int triangleIndex = nextTriangleIndex++;
		indexedTriangles.insert(std::pair<unsigned int, IndexedTriangle3D<T>>(triangleIndex, indexedTriangle));

		//increment counter of number of triangle by point
		const unsigned int *indexes = indexedTriangle.getIndexes();
		for(int i=0; i<3; i++)
		{
			std::map<unsigned int, unsigned int>::iterator itFind = nbTrianglesByPoint.find(indexes[i]);
			if(itFind!=nbTrianglesByPoint.end())
			{
				itFind->second++;
			}else
			{
				nbTrianglesByPoint[indexes[i]] = 1;
			}
		}

		return triangleIndex;
	}

	template<class T> void ConvexHull3D<T>::removeTriangle(const typename std::map<unsigned int, IndexedTriangle3D<T>>::iterator &itTriangle)
	{
		//decrement counter of number of triangle by point
		const unsigned int *indexes = itTriangle->second.getIndexes();
		for(int i=0;i<3;i++)
		{
			int value = --nbTrianglesByPoint[indexes[i]];
			if(value<=0)
			{
				points.erase(indexes[i]);
			}
		}

		//remove indexed triangles from triangle map
		indexedTriangles.erase(itTriangle);
	}

	/**
	 * @return All indexes points used to build the tetrahedron
	 */
	template<class T> std::set<unsigned int> ConvexHull3D<T>::buildTetrahedron(const std::vector<Point3<T>> &points)
	{
		//1. initialize
		std::set<unsigned int> pointsUsed;

		//2. build a point (use first point)
		if(points.size()<1)
		{
			throw buildException(points, pointsUsed);
		}

		this->points[nextPointIndex++] = points[0];
		pointsUsed.insert(0);

		//3. build a line (find two distinct points)
		for(unsigned int i=1; i<points.size(); i++)
		{
			if(points[i]!=this->points[0])
			{
				this->points[nextPointIndex++] = points[i];
				pointsUsed.insert(i);
				break;
			}
		}

		if(pointsUsed.size()!=2)
		{
			throw buildException(points, pointsUsed);
		}

		//4. build triangles (find a point which doesn't belong to line).
		Vector3<T> lineVector = this->points[0].vector(this->points[1]);
		for(unsigned int i=1; i<points.size(); i++)
		{
			if(pointsUsed.find(i)!=pointsUsed.end())
			{ //point already used to build the tetrahedron
				continue;
			}

			Vector3<T> linesCrossProduct = lineVector.crossProduct(this->points[0].vector(points[i]));
			if(linesCrossProduct.X != (T)0.0 || linesCrossProduct.Y != (T)0.0 || linesCrossProduct.Z != (T)0.0)
			{
				this->points[nextPointIndex++] = points[i];
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
		nbTrianglesByPoint[0] = 2;
		nbTrianglesByPoint[1] = 2;
		nbTrianglesByPoint[2] = 2;

		//5. build tetrahedron (find a no coplanar point to the triangle)
		const Vector3<T> &firstTriangleNormal = this->indexedTriangles.find(0)->second.computeNormal(this->points);
		const Point3<T> &firstPoint = this->points.find(0)->second;
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
	template<class T> std::invalid_argument ConvexHull3D<T>::buildException(const std::vector<Point3<T>> &points, const std::set<unsigned int> &pointsUsed)
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
		Logger::setLogger(new FileLogger());
		Logger::logger()<<Logger::prefix(Logger::LOG_ERROR);
		if(points.size()>0)
		{
			Logger::logger()<<"Impossible to build a convex hull with following points:\n";
			for (unsigned int i=0; i<points.size(); ++i)
			{
				Logger::logger()<< " - " << points[i] << "\n";
			}
		}else
		{
			Logger::logger()<<"Impossible to build a convex hull with zero point.\n";
		}
		Logger::setLogger(nullptr);

		return std::invalid_argument("Impossible to build the convex hull. All points form a " + formName + ".");
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const ConvexHull3D<T> &ch)
	{
		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::const_iterator it = ch.getIndexedTriangles().begin(); it!=ch.getIndexedTriangles().end(); ++it)
		{
			stream << "Triangle "<< it->first <<": "
				<<"("<<ch.getIndexedPoints().find(it->second.getIndexes()[0])->second<<") "
				<<"("<<ch.getIndexedPoints().find(it->second.getIndexes()[1])->second<<") "
				<<"("<<ch.getIndexedPoints().find(it->second.getIndexes()[2])->second<<") "
				<<std::endl;
		}

		return stream;
	}

	//explicit template
	template class ConvexHull3D<float>;
	template std::ostream& operator <<<float>(std::ostream &, const ConvexHull3D<float> &);

}
