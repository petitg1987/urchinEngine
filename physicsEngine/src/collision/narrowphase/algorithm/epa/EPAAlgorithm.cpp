#include "collision/narrowphase/algorithm/epa/EPAAlgorithm.h"

namespace urchin
{

	template<class T> EPAAlgorithm<T>::EPAAlgorithm() :
		maxIteration(ConfigService::instance()->getUnsignedIntValue("narrowPhase.epaMaxIteration")),
		terminationTolerance(ConfigService::instance()->getFloatValue("narrowPhase.epaTerminationTolerance"))
	{

	}

	template<class T> EPAAlgorithm<T>::~EPAAlgorithm()
	{

	}

	template<class T> std::unique_ptr<EPAResult<T>> EPAAlgorithm<T>::processEPA(const CollisionConvexObject3D &convexObject1, const CollisionConvexObject3D &convexObject2,
			const GJKResult<T> &gjkResult)
	{
		#ifdef _DEBUG
			assert(gjkResult.isCollide());
		#endif

		//1. check collision exist
		const Simplex<T> &simplex = gjkResult.getSimplex();
		if(simplex.getSize()==1)
		{ //simplex point is the origin
			return std::make_unique<EPAResultNoCollide<T>>();
		}

		//2. initialize global variables
		std::map<unsigned int, Point3<T>> supportPointsA, supportPointsB;  //first: index of point in convex hull, second: support points
		std::map<unsigned int, EPATriangleData<T>> trianglesData; //first: index of triangle in convex hull, second: EPA triangle data

		//3. create initial convex hull
		std::map<unsigned int, Point3<T>> indexedPoints;
		std::map<unsigned int, IndexedTriangle3D<T>> indexedTriangles;

		determineInitialPoints(simplex, convexObject1, convexObject2, indexedPoints, supportPointsA, supportPointsB);
		determineInitialTriangles(indexedPoints, indexedTriangles);

		if(indexedTriangles.size()!=4)
		{//due to numerical imprecision, it's impossible to create indexed triangles correctly
			return std::make_unique<EPAResultInvalid<T>>();
		}

		ConvexHull3D<T> convexHull(indexedPoints, indexedTriangles);

		for(typename std::map<unsigned int, IndexedTriangle3D<T>>::const_iterator it = indexedTriangles.begin(); it!=indexedTriangles.end(); ++it)
		{
			trianglesData.insert(std::pair<unsigned int, EPATriangleData<T>>(it->first, createTriangleData(convexHull, it->first)));
		}

		//4. find closest plane of extended polytope
		T upperBoundPenDepth = std::numeric_limits<T>::max();
		typename std::map<unsigned int, EPATriangleData<T>>::const_iterator itClosestTriangleData;
		unsigned int iterationNumber = 0;
		Vector3<T> normal;
		T distanceToOrigin;

		while(true)
		{
			itClosestTriangleData = getClosestTriangleData(trianglesData);
			const EPATriangleData<T> &closestTriangleData = itClosestTriangleData->second;

			normal = closestTriangleData.getNormal();
			distanceToOrigin = closestTriangleData.getDistanceToOrigin();

			if(iterationNumber > maxIteration)
			{ //can happen on spherical forms where EPA algorithm doesn't progress enough fast
				break;
			}

			//get support point in direction of normal
			const Point3<T> supportPointNormal = convexObject1.getSupportPoint(normal.template cast<float>(), true).template cast<T>();
			const Point3<T> supportPointMinusNormal = convexObject2.getSupportPoint((-normal).template cast<float>(), true).template cast<T>();
			const Point3<T> minkowskiDiffPoint = supportPointNormal - supportPointMinusNormal;

			upperBoundPenDepth = std::min(upperBoundPenDepth, std::abs(minkowskiDiffPoint.toVector().dotProduct(normal)));
			bool closeEnough = upperBoundPenDepth <= (1.0+terminationTolerance) * distanceToOrigin;

			if(!closeEnough)
			{ //polytope can be extended in direction of normal: add a new point
				std::vector<unsigned int> newTriangleIndexes;
				std::vector<unsigned int> removedTriangleIndexes;
				unsigned int index = convexHull.addNewPoint(minkowskiDiffPoint, newTriangleIndexes, removedTriangleIndexes);
				if(index==0)
				{ //finally, polytope cannot by extended in direction of normal. Cause: numerical imprecision.
					break;
				}

				//compute new triangles data
				for(unsigned int i=0; i<newTriangleIndexes.size(); ++i)
				{
					trianglesData.insert(std::pair<unsigned int, EPATriangleData<T>>(newTriangleIndexes[i], createTriangleData(convexHull, newTriangleIndexes[i])));
				}

				//remove triangle data not used anymore
				for(unsigned int i=0; i<removedTriangleIndexes.size(); ++i)
				{
					trianglesData.erase(removedTriangleIndexes[i]);
				}

				//save support points
				supportPointsA[index] = supportPointNormal;
				supportPointsB[index] = supportPointMinusNormal;
			}else
			{ //polytope cannot by extended in direction of normal: solution is found
				break;
			}
			iterationNumber++;
		};

		//5. compute EPA result: normal, penetration depth and contact points of collision
		const EPATriangleData<T> &closestTriangleData = itClosestTriangleData->second;
		const IndexedTriangle3D<T> &indexedTriangle = convexHull.getIndexedTriangles().find(itClosestTriangleData->first)->second;
		const unsigned int pointIndex1 = indexedTriangle.getIndexes()[0];
		const unsigned int pointIndex2 = indexedTriangle.getIndexes()[1];
		const unsigned int pointIndex3 = indexedTriangle.getIndexes()[2];

		const Point3<T> contactPointA = closestTriangleData.getBarycentric(0) * supportPointsA[pointIndex1] + closestTriangleData.getBarycentric(1) * supportPointsA[pointIndex2]
				+ closestTriangleData.getBarycentric(2) * supportPointsA[pointIndex3];
		const Point3<T> contactPointB = closestTriangleData.getBarycentric(0) * supportPointsB[pointIndex1] + closestTriangleData.getBarycentric(1) * supportPointsB[pointIndex2]
				+ closestTriangleData.getBarycentric(2) * supportPointsB[pointIndex3];

		#ifdef _DEBUG
			const T subtractDistance = contactPointA.vector(contactPointB).squareLength() - distanceToOrigin*distanceToOrigin;
			assert((subtractDistance-0.01) <= 0.0 && (subtractDistance+0.01) >= 0.0);
		#endif

		return std::make_unique<EPAResultCollide<T>>(contactPointA, contactPointB, normal, distanceToOrigin);
	}

	/**
	 * Determine initial points useful for EPA algorithm: points of initial convex hull as well as the linked support points
	 * @param simplex Simplex resulting from GJK algorithm
	 * @param indexedPoints [out] Initial index points for EPA algorithm
	 * @param supportPointsA [out] Support points of object A used to compute indexed points
	 * @param supportPointsB [out] Support points of object B used to compute indexed points
	 */
	template<class T> void EPAAlgorithm<T>::determineInitialPoints(const Simplex<T> &simplex, const CollisionConvexObject3D &convexObject1,
			const CollisionConvexObject3D &convexObject2, std::map<unsigned int, Point3<T>> &indexedPoints,
			std::map<unsigned int, Point3<T>> &supportPointsA, std::map<unsigned int, Point3<T>> &supportPointsB)
	{
		if(simplex.getSize()==2)
		{ //simplex is a segment line containing the origin
			//compute normalized direction vector
			const Vector3<T> lineDirection = simplex.getPoint(0).vector(simplex.getPoint(1)).normalize();

			//find best coordinate axis for which i-th component of 'lineDirection' has the smallest absolute value.
			//In this way, the angle between 'lineDirection' and best coordinate axis is the largest. Thus the cross
			// product has the largest length.
			int minAxis = 0; //0=X, 1=Y, 2=Z
			T minValue = std::abs(lineDirection.X);
			for(unsigned int i=1; i<3; ++i)
			{
				if(minValue > std::abs(lineDirection[i]))
				{
					minValue = std::abs(lineDirection[i]);
					minAxis = i;
				}
			}

			//create three vectors around 'lineDirection' separate of 120 degrees
			const T sin60 = 0.86602540378; //sin(pi/3)
			const T cos60 = 0.5; //cos(pi/3)
			const Quaternion<T> rotationQuaternion(lineDirection.X*sin60, lineDirection.Y*sin60, lineDirection.Z*sin60, cos60);
			const Matrix3<T> rotationMatrix = rotationQuaternion.toMatrix3();

			const Vector3<T> v1 = lineDirection.crossProduct(Vector3<T>(minAxis==0 ? 1.0 : 0.0, minAxis==1 ? 1.0 : 0.0, minAxis==2 ? 1.0 : 0.0));
			const Vector3<T> v2 = rotationMatrix * Vector3<T>(v1.X, v1.Y, v1.Z);
			const Vector3<T> v3 = rotationMatrix * Vector3<T>(v2.X, v2.Y, v2.Z);

			//retrieve all points forming a hexahedron (two tetrahedron glued together)
			Point3<T> supportPoints[3] = {
					convexObject1.getSupportPoint(v1.template cast<float>(), true).template cast<T>(),
					convexObject1.getSupportPoint(v2.template cast<float>(), true).template cast<T>(),
					convexObject1.getSupportPoint(v3.template cast<float>(), true).template cast<T>()};
			Point3<T> supportPointsMinus[3] = {
					convexObject2.getSupportPoint((-v1).template cast<float>(), true).template cast<T>(),
					convexObject2.getSupportPoint((-v2).template cast<float>(), true).template cast<T>(),
					convexObject2.getSupportPoint((-v3).template cast<float>(), true).template cast<T>()};

			for(unsigned int i=0; i<2; ++i)
			{
				indexedPoints[i] = simplex.getPoint(i);
				supportPointsA[i] = simplex.getSupportPointA(i);
				supportPointsB[i] = simplex.getSupportPointB(i);
			}
			for(unsigned int i=0; i<3; ++i)
			{
				indexedPoints[i+2] = supportPoints[i] - supportPointsMinus[i];
				supportPointsA[i+2] = supportPoints[i];
				supportPointsB[i+2] = supportPointsMinus[i];
			}

			//keep only the tetrahedron containing the origin
			if(Tetrahedron<T>(indexedPoints[0], indexedPoints[2], indexedPoints[3], indexedPoints[4]).collideWithPoint(Point3<T>(0.0, 0.0, 0.0)))
			{
				//we use the point 4 instead of point 1 for the initial tetrahedron
				indexedPoints[1] = indexedPoints[4];
				supportPointsA[1] = supportPointsA[4];
				supportPointsB[1] = supportPointsB[4];
			}else
			{
				//we use the point 4 instead of point 0 for the initial tetrahedron
				indexedPoints[0] = indexedPoints[4];
				supportPointsA[0] = supportPointsA[4];
				supportPointsB[0] = supportPointsB[4];
			}
		}else if(simplex.getSize()==3)
		{ //simplex is a triangle containing the origin
			//create two vectors based on three points
			Triangle3D<T> triangle(simplex.getPoint(0), simplex.getPoint(1), simplex.getPoint(2));
			const Vector3<T> v1 = triangle.computeNormal();
			const Vector3<T> v2 = -v1;

			//retrieve all points forming a hexahedron (two tetrahedron glued together)
			Point3<T> supportPoints[2] = {
					convexObject1.getSupportPoint(v1.template cast<float>(), true).template cast<T>(),
					convexObject1.getSupportPoint(v2.template cast<float>(), true).template cast<T>()};
			Point3<T> supportPointsMinus[2] = {
					convexObject2.getSupportPoint((-v1).template cast<float>(), true).template cast<T>(),
					convexObject2.getSupportPoint((-v2).template cast<float>(), true).template cast<T>()};

			for(unsigned int i=0; i<3; ++i)
			{
				indexedPoints[i] = simplex.getPoint(i);
				supportPointsA[i] = simplex.getSupportPointA(i);
				supportPointsB[i] = simplex.getSupportPointB(i);
			}
			for(unsigned int i=0; i<2; ++i)
			{
				indexedPoints[i+3] = supportPoints[i] - supportPointsMinus[i];
				supportPointsA[i+3] = supportPoints[i];
				supportPointsB[i+3] = supportPointsMinus[i];
			}

			//keep only the tetrahedron containing the origin
			if(Tetrahedron<T>(indexedPoints[0], indexedPoints[1], indexedPoints[2], indexedPoints[3]).collideWithPoint(Point3<T>(0.0, 0.0, 0.0)))
			{
				//we use the 4 first point - nothing to do
			}else
			{
				//we use the point 4 instead of point 3 for the initial tetrahedron
				indexedPoints[3] = indexedPoints[4];
				supportPointsA[3] = supportPointsA[4];
				supportPointsB[3] = supportPointsB[4];
			}
		}else if(simplex.getSize()==4)
		{ //simplex is a tetrahedron containing the origin
			for(unsigned int i=0; i<4; ++i)
			{
				indexedPoints[i] = simplex.getPoint(i);
				supportPointsA[i] = simplex.getSupportPointA(i);
				supportPointsB[i] = simplex.getSupportPointB(i);
			}
		}else
		{
			throw std::invalid_argument("Size of simplex unsupported: " + std::to_string(simplex.getSize()) + ".");
		}
	}

	/**
	 * Determine triangles of initial convex hull of EPA algorithm. Normal of triangle must be outside the convex hull.
	 * @param indexedPoints Points of initial convex hull
	 * @param indexedTriangles [out] Triangle of initial convex hull. When points are too close together or almost on same
	 * plane: the indexed triangles can be incomplete
	 */
	template<class T> void EPAAlgorithm<T>::determineInitialTriangles(const std::map<unsigned int, Point3<T>> &indexedPoints,
			std::map<unsigned int, IndexedTriangle3D<T>> &indexedTriangles)
	{
		for(unsigned int i=0; i<3; ++i)
		{
			for(unsigned int j=i+1; j<4; ++j)
			{
				T distance = indexedPoints.at(i).vector(indexedPoints.at(j)).length();
				T minPointsDistance = (std::nextafter(distance, std::numeric_limits<T>::max()) - distance) * 10.0;

				if(distance < minPointsDistance)
				{
					return;
				}
			}
		}

		const unsigned int indexes[4][3] = 		{{0, 1, 2}, {0, 3, 1}, {0, 2, 3}, {1, 3, 2}};
		const unsigned int revIndexes[4][3] = 	{{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}};
		for(unsigned int i=0; i<4; ++i)
		{
			const unsigned int pointOutsideTriangle = 6 - (indexes[i][0] + indexes[i][1] + indexes[i][2]);
			const Vector3<T> normalTriangle = IndexedTriangle3D<T>(indexes[i]).computeNormal(indexedPoints);
			const Vector3<T> trianglePointToOutsidePoint = indexedPoints.at(indexes[i][0]).vector(indexedPoints.at(pointOutsideTriangle));
			T dotProduct = normalTriangle.dotProduct(trianglePointToOutsidePoint);

			T trianglePointToOutsidePointLength = trianglePointToOutsidePoint.length();
			T dotProductTolerance = (std::nextafter(trianglePointToOutsidePointLength, std::numeric_limits<T>::max()) - trianglePointToOutsidePointLength);

			if(dotProduct < -dotProductTolerance)
			{
				indexedTriangles.insert(std::pair<unsigned int, IndexedTriangle3D<T>>(i, IndexedTriangle3D<T>(indexes[i])));
			}else if(dotProduct > dotProductTolerance)
			{
				indexedTriangles.insert(std::pair<unsigned int, IndexedTriangle3D<T>>(i, IndexedTriangle3D<T>(revIndexes[i])));
			}else
			{
				return;
			}
		}
	}

	/**
	 * Find closest plane (triangle plane) near the origin
	 * @param trianglesData Triangles data
	 * @return Iterator on closest triangle data
	 */
	template<class T> typename std::map<unsigned int, EPATriangleData<T>>::const_iterator EPAAlgorithm<T>::getClosestTriangleData(
			const typename std::map<unsigned int, EPATriangleData<T>> &trianglesData) const
	{
		T minDistanceToOrigin = std::numeric_limits<T>::max();
		typename std::map<unsigned int, EPATriangleData<T>>::const_iterator closestTriangleData;

		for(typename std::map<unsigned int, EPATriangleData<T>>::const_iterator it = trianglesData.begin(); it!=trianglesData.end(); ++it)
		{
			const EPATriangleData<T> &triangleData = it->second;
			T distanceToOrigin = std::abs(triangleData.getDistanceToOrigin());

			if(minDistanceToOrigin > distanceToOrigin)
			{
				minDistanceToOrigin = distanceToOrigin;
				closestTriangleData = it;
			}
		}

		return closestTriangleData;
	}

	/**
	 * @param triangleIndex Index of triangle in convex hull used to compute the data
	 * @return Computed triangle data (normal, distance to origin...).
	 */
	template<class T> EPATriangleData<T> EPAAlgorithm<T>::createTriangleData(const ConvexHull3D<T> &convexHull, unsigned int triangleIndex)
	{
		const IndexedTriangle3D<T> &indexedTriangle = convexHull.getIndexedTriangles().find(triangleIndex)->second;
		const unsigned int pointIndex1 = indexedTriangle.getIndexes()[0];
		const unsigned int pointIndex2 = indexedTriangle.getIndexes()[1];
		const unsigned int pointIndex3 = indexedTriangle.getIndexes()[2];

		const Point3<T> &point1 = convexHull.getIndexedPoints().find(pointIndex1)->second;
		const Point3<T> &point2 = convexHull.getIndexedPoints().find(pointIndex2)->second;
		const Point3<T> &point3 = convexHull.getIndexedPoints().find(pointIndex3)->second;
		const Triangle3D<T> triangle(point1, point2, point3);

		//compute point on the triangle nearest to origin
		T barycentrics[3];
		Point3<T> closestPointToOrigin = triangle.closestPoint(Point3<T>(0.0, 0.0, 0.0), barycentrics);

		//compute minimum distance between triangle and the origin
		T distanceToOrigin = closestPointToOrigin.toVector().length();

		//compute normal (external to convex hull)
		const Vector3<T> normal = triangle.computeNormal();

		return EPATriangleData<T>(distanceToOrigin, normal, closestPointToOrigin, barycentrics);
	}

	#ifdef _DEBUG
		template<class T> void EPAAlgorithm<T>::logMaximumIterationReach() const
		{
			Logger::setLogger(new FileLogger());
			Logger::logger()<<Logger::prefix(Logger::LOG_WARNING);
			Logger::logger()<<"Maximum of iteration reached on EPA algorithm ("<<maxIteration<<")."<<"\n";
			Logger::logger()<<" - Termination tolerance: "<<terminationTolerance<<"\n\n";
			Logger::setLogger(nullptr);
		}
	#endif

	//explicit template
	template class EPAAlgorithm<float>;
	template class EPAAlgorithm<double>;

}
