#include <cmath>
#include <chrono>
#include <algorithm>
#include <numeric>

#include "NavMeshGenerator.h"
#include "path/navmesh/csg/PolygonsUnion.h"
#include "path/navmesh/csg/PolygonsSubtraction.h"

#define WALKABLE_FACE_EXPAND_SIZE 0.0001f
#define OBSTACLE_REDUCE_SIZE 0.0001f

namespace urchin
{

	PolyhedronFaceIndex::PolyhedronFaceIndex(unsigned int polyhedronIndex, unsigned int faceIndex) :
			polyhedronIndex(polyhedronIndex), faceIndex(faceIndex)
	{

	}

    NavMeshGenerator::NavMeshGenerator() :
            polygonMinDotProductThreshold(std::cos(AngleConverter<float>::toRadian(ConfigService::instance()->getFloatValue("navMesh.polygon.removeAngleThresholdInDegree")))),
            polygonMergePointsDistanceThreshold(ConfigService::instance()->getFloatValue("navMesh.polygon.mergePointsDistanceThreshold"))
    {

    }

	void NavMeshGenerator::setNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig)
	{
		this->navMeshConfig = std::move(navMeshConfig);
	}

	std::shared_ptr<NavMesh> NavMeshGenerator::generate(std::shared_ptr<AIWorld> aiWorld)
	{
		if(!navMeshConfig)
		{
			throw std::runtime_error("Impossible to generate navigation mesh: no configuration provided.");
		}

		#ifdef _DEBUG
//			auto frameStartTime = std::chrono::high_resolution_clock::now();
		#endif

		std::vector<Polyhedron> expandedPolyhedrons = createExpandedPolyhedrons(std::move(aiWorld));
		std::vector<PolyhedronFaceIndex> polyhedronWalkableFaces = findWalkableFaces(expandedPolyhedrons);
		navMesh.reset(new NavMesh());

		for(const auto &polyhedronWalkableFace : polyhedronWalkableFaces)
		{
			std::vector<std::shared_ptr<NavPolygon>> navPolygons = createNavigationPolygonFor(polyhedronWalkableFace, expandedPolyhedrons);
			for(const auto &navPolygon : navPolygons)
			{
				navMesh->addPolygon(navPolygon);
			}
		}

		#ifdef _DEBUG
//			auto frameEndTime = std::chrono::high_resolution_clock::now();
//			auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
//			std::cout<<"Nav mesh generation time (ms): "<<diffTimeMicroSeconds/1000.0<<std::endl;
		#endif

		return navMesh;
	}

	std::vector<Polyhedron> NavMeshGenerator::createExpandedPolyhedrons(std::shared_ptr<AIWorld> aiWorld) const
	{
		std::vector<Polyhedron> polyhedrons;
		polyhedrons.reserve(aiWorld->getObjects().size());

		for(auto &aiObject : aiWorld->getObjects())
		{
			std::unique_ptr<ConvexObject3D<float>> object = aiObject.getShape()->toConvexObject(aiObject.getTransform());

			if(auto box = dynamic_cast<OBBox<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), box));
			}else if(auto capsule = dynamic_cast<Capsule<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), capsule));
			}else if(auto cone = dynamic_cast<Cone<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), cone));
			}else if(auto convexHull = dynamic_cast<ConvexHull3D<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), convexHull));
			}else if(auto cylinder = dynamic_cast<Cylinder<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), cylinder));
			}else if(auto sphere = dynamic_cast<Sphere<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), sphere));
			} else
			{
				throw std::invalid_argument("Shape type not supported by navigation mesh generator: " + std::string(typeid(*object).name()));
			}
		}

		expandPolyhedrons(polyhedrons);

		return polyhedrons;
	}

	/**
	 * Return box faces. Faces are guaranteed to be in the following order: right, left, top, bottom, front, back when
	 * points are sorted first on positive X axis, then on positive Y axis and then on positive Z axis.
	 */
	std::vector<PolyhedronFace> NavMeshGenerator::createPolyhedronFaces() const
	{
		std::vector<PolyhedronFace> faces;
		faces.reserve(6);

		faces.push_back(PolyhedronFace({0, 2, 3, 1})); //right
		faces.push_back(PolyhedronFace({4, 5, 7, 6})); //left
		faces.push_back(PolyhedronFace({0, 1, 5, 4})); //top
		faces.push_back(PolyhedronFace({3, 2, 6, 7})); //bottom
		faces.push_back(PolyhedronFace({0, 4, 6, 2})); //front
		faces.push_back(PolyhedronFace({1, 3, 7, 5})); //back

		return faces;
	}

	/**
	 * Return box points. This method suppose that faces index (0 to 5) are in the following order: right, left, top, bottom, front, back.
	 */
	std::vector<PolyhedronPoint> NavMeshGenerator::createPolyhedronPoints(OBBox<float> *box) const
	{
		std::vector<PolyhedronPoint> polyhedronPoints;
		polyhedronPoints.reserve(8);

		std::vector<Point3<float>> points = box->getPoints();
		constexpr unsigned int faceIndicesTab[8][3] = {{0, 2, 4}, {0, 2, 5}, {0, 3, 4}, {0, 3, 5}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5}};
		for(unsigned int i=0; i<8; ++i)
		{
			PolyhedronPoint polyhedronPoint;
			polyhedronPoint.point = points[i];
			polyhedronPoint.faceIndices.reserve(3);
			polyhedronPoint.faceIndices.insert(polyhedronPoint.faceIndices.end(), &faceIndicesTab[i][0], &faceIndicesTab[i][3]);
			polyhedronPoints.push_back(polyhedronPoint);
		}

		return polyhedronPoints;
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, OBBox<float> *box) const
	{
		std::vector<PolyhedronFace> polyhedronFaces = createPolyhedronFaces();
		return Polyhedron(name, polyhedronFaces, createPolyhedronPoints(box));
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Capsule<float> *capsule) const
	{
		Vector3<float> boxHalfSizes(capsule->getRadius(), capsule->getRadius(), capsule->getRadius());
		boxHalfSizes[capsule->getCapsuleOrientation()] += capsule->getCylinderHeight() / 2.0f;

		OBBox<float> capsuleBox(boxHalfSizes, capsule->getCenterOfMass(), capsule->getOrientation());
		Polyhedron polyhedron = createPolyhedronFor(name, &capsuleBox);
		polyhedron.setWalkableCandidate(false);

		return polyhedron;
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Cone<float> *cone) const
	{
		Vector3<float> boxHalfSizes(cone->getRadius(), cone->getRadius(), cone->getRadius());
		boxHalfSizes[cone->getConeOrientation()/2] = cone->getHeight() / 2.0f;

		OBBox<float> coneBox(boxHalfSizes, cone->getCenter(), cone->getOrientation());
		Polyhedron polyhedron = createPolyhedronFor(name, &coneBox);
		polyhedron.setWalkableCandidate(false);

		return polyhedron;
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, ConvexHull3D<float> *convexHull) const
	{
		const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTriangles = convexHull->getIndexedTriangles();
		const std::map<unsigned int, ConvexHullPoint<float>> &convexHullPoints = convexHull->getConvexHullPoints();

		std::vector<PolyhedronPoint> polyhedronPoints;
		polyhedronPoints.reserve(convexHullPoints.size());
		for(const auto &convexHullPoint : convexHullPoints)
		{
			PolyhedronPoint polyhedronPoint;
			polyhedronPoint.point = convexHullPoint.second.point;
			polyhedronPoint.faceIndices.reserve(convexHullPoint.second.triangleIndices.size());

			polyhedronPoints.push_back(polyhedronPoint);
		}

		std::vector<PolyhedronFace> faces;
		faces.reserve(indexedTriangles.size());
		unsigned int i=0;
		for(const auto &indexedTriangle : indexedTriangles)
		{
			const unsigned int *indices = indexedTriangle.second.getIndices();

			unsigned int polyhedronPoint0Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[0])));
			polyhedronPoints[polyhedronPoint0Index].faceIndices.push_back(i);

			unsigned int polyhedronPoint1Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[1])));
			polyhedronPoints[polyhedronPoint1Index].faceIndices.push_back(i);

			unsigned int polyhedronPoint2Index = static_cast<unsigned int>(std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[2])));
			polyhedronPoints[polyhedronPoint2Index].faceIndices.push_back(i);

			faces.push_back(PolyhedronFace({polyhedronPoint0Index, polyhedronPoint1Index, polyhedronPoint2Index}));
			i++;
		}

		return Polyhedron(name, faces, polyhedronPoints);
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Cylinder<float> *cylinder) const
	{
		Vector3<float> boxHalfSizes(cylinder->getRadius(), cylinder->getRadius(), cylinder->getRadius());
		boxHalfSizes[cylinder->getCylinderOrientation()] = cylinder->getHeight() / 2.0f;

		OBBox<float> cylinderBox(boxHalfSizes, cylinder->getCenterOfMass(), cylinder->getOrientation());
		std::vector<PolyhedronFace> faces = createPolyhedronFaces();
		for(unsigned int i=0; i<faces.size(); ++i)
		{
			faces[i].setWalkableCandidate(cylinder->getCylinderOrientation()==i/2);
		}

		return Polyhedron(name, faces, createPolyhedronPoints(&cylinderBox));
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Sphere<float> *sphere) const
	{
		OBBox<float> sphereBox(*sphere);
		Polyhedron polyhedron = createPolyhedronFor(name, &sphereBox);
		polyhedron.setWalkableCandidate(false);

		return polyhedron;
	}

	void NavMeshGenerator::expandPolyhedrons(std::vector<Polyhedron> &polyhedrons) const
	{
		for(auto &polyhedron : polyhedrons)
		{
			polyhedron.expand(navMeshConfig->getAgent());
		}
	}

	std::vector<PolyhedronFaceIndex> NavMeshGenerator::findWalkableFaces(const std::vector<Polyhedron> &expandedPolyhedrons) const
	{
		std::vector<PolyhedronFaceIndex> walkableFaces;
		walkableFaces.reserve(expandedPolyhedrons.size()/2); //estimated memory size

		for(unsigned int polyhedronIndex=0; polyhedronIndex<expandedPolyhedrons.size(); ++polyhedronIndex)
		{
			const Polyhedron &expandedPolyhedron = expandedPolyhedrons[polyhedronIndex];
			if(expandedPolyhedron.isWalkableCandidate())
			{
				for(unsigned int faceIndex=0; faceIndex<expandedPolyhedron.getFaces().size(); ++faceIndex)
				{
					const PolyhedronFace &face = expandedPolyhedron.getFace(faceIndex);

					if(face.isWalkableCandidate() && std::fabs(face.getAngleToHorizontal()) < navMeshConfig->getMaxSlope())
					{
						walkableFaces.emplace_back(PolyhedronFaceIndex(polyhedronIndex, faceIndex));
					}
				}
			}
		}

		return walkableFaces;
	}

	std::vector<std::shared_ptr<NavPolygon>> NavMeshGenerator::createNavigationPolygonFor(const PolyhedronFaceIndex &polyhedronWalkableFace, const std::vector<Polyhedron> &expandedPolyhedrons) const
	{
		const Polyhedron &polyhedron = expandedPolyhedrons[polyhedronWalkableFace.polyhedronIndex];
		const PolyhedronFace &walkableFace = polyhedron.getFace(polyhedronWalkableFace.faceIndex);

		std::string walkableName = polyhedron.getName() + "[" + std::to_string(polyhedronWalkableFace.faceIndex) + "]";
        std::vector<bool> isExternalPoints(walkableFace.getCcwPoints().size(), true);
        std::vector<CSGPolygon<float>> walkablePolygons = {CSGPolygon<float>(walkableName, reverseAndFlatPointsOnYAxis(walkableFace.getCcwPoints()))};

        auto obstaclePolygons = computeObstacles(expandedPolyhedrons, polyhedronWalkableFace);
		std::vector<CSGPolygon<float>> remainingObstaclePolygons;
		remainingObstaclePolygons.reserve(2); //estimated memory size

		for(const auto &obstaclePolygon : obstaclePolygons)
		{
            CSGPolygon<float> simplifiedObstaclePolygon = obstaclePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if (simplifiedObstaclePolygon.getCwPoints().size() > 2)
            {
                auto walkablePolygonsCounter = static_cast<int>(walkablePolygons.size());
                while (walkablePolygonsCounter-- != 0)
                {
                    const CSGPolygon<float> &walkablePolygon = walkablePolygons[0];

                    bool obstacleInsideWalkable;
                    std::vector<CSGPolygon<float>> subtractedPolygons = PolygonsSubtraction<float>::instance()->subtractPolygons(
                            walkablePolygon, simplifiedObstaclePolygon, obstacleInsideWalkable);

                    //replace 'walkablePolygon' by 'subtractedPolygons'
                    walkablePolygons.erase(walkablePolygons.begin());
                    for (auto &subtractedPolygon : subtractedPolygons)
                    {
                        walkablePolygons.emplace_back(subtractedPolygon);
                    }

                    if (obstacleInsideWalkable)
                    {
                        //slightly reduce to avoid obstacle points touch others obstacles points (not supported by triangulation)
                        std::vector<Point2<float>> reducedObstacleCwPoints = ResizePolygon2DService<float>::instance()->resizePolygon(
                                simplifiedObstaclePolygon.getCwPoints(), OBSTACLE_REDUCE_SIZE);

                        remainingObstaclePolygons.emplace_back(CSGPolygon<float>(simplifiedObstaclePolygon.getName(), reducedObstacleCwPoints));
                    }
                }
            }
		}

		std::vector<std::shared_ptr<NavPolygon>> navPolygons;
		navPolygons.reserve(walkablePolygons.size());
		for(auto &walkablePolygon : walkablePolygons)
		{
            //simplify polygon to improve performance and avoid degenerated walkable face
			CSGPolygon<float> simplifiedWalkablePolygons = walkablePolygon.simplify(polygonMinDotProductThreshold, polygonMergePointsDistanceThreshold);
            if(simplifiedWalkablePolygons.getCwPoints().size() > 2)
            {
                //slightly expand to avoid obstacle points to be in contact with walkable edges (not supported by triangulation)
                std::vector<Point2<float>> extendedWalkableCwPoints = ResizePolygon2DService<float>::instance()->resizePolygon(
                        simplifiedWalkablePolygons.getCwPoints(), -WALKABLE_FACE_EXPAND_SIZE);

                Triangulation triangulation(reversePoints(extendedWalkableCwPoints));

                for(const auto &remainingObstaclePolygon : remainingObstaclePolygons)
                {
                    if(simplifiedWalkablePolygons.pointInsideOrOnPolygon(remainingObstaclePolygon.getCwPoints()[0]))
                    { //obstacle fully inside walkable polygon
                        triangulation.addHolePoints(remainingObstaclePolygon.getCwPoints());
                    }
                }

                const std::vector<IndexedTriangle3D<float>> &triangles = toIndexedTriangle3D(triangulation.triangulate());
                std::vector<Point3<float>> points = elevateTriangulatedPoints(triangulation, walkableFace);
                navPolygons.push_back(std::make_shared<NavPolygon>(points, triangles));
            }
		}

		return navPolygons;
	}

	std::vector<Point2<float>> NavMeshGenerator::reversePoints(const std::vector<Point2<float>> &points) const
	{
		std::vector<Point2<float>> reversePoints;
		reversePoints.reserve(points.size());

		for(auto it = points.rbegin(); it!=points.rend(); ++it)
		{
			reversePoints.emplace_back(*it);
		}

		return reversePoints;
	}

	std::vector<Point2<float>> NavMeshGenerator::reverseAndFlatPointsOnYAxis(const std::vector<Point3<float>> &points) const
	{
		std::vector<Point2<float>> reverseFlatPoints;
		reverseFlatPoints.reserve(points.size());

		for(auto it = points.rbegin(); it!=points.rend(); ++it)
		{
			reverseFlatPoints.emplace_back(Point2<float>(it->X, -it->Z));
		}

		return reverseFlatPoints;
	}

	std::vector<CSGPolygon<float>> NavMeshGenerator::computeObstacles(const std::vector<Polyhedron> &expandedPolyhedrons, const PolyhedronFaceIndex &polyhedronWalkableFace) const
	{
		const Polyhedron &polyhedron = expandedPolyhedrons[polyhedronWalkableFace.polyhedronIndex];
		const PolyhedronFace &walkableFace = polyhedron.getFace(polyhedronWalkableFace.faceIndex);

		std::vector<CSGPolygon<float>> holePolygons;
		for(unsigned int i=0; i<expandedPolyhedrons.size(); ++i)
		{
			if(i!=polyhedronWalkableFace.polyhedronIndex)
			{
				CSGPolygon<float> footprintPolygon = computePolyhedronFootprint(expandedPolyhedrons[i], walkableFace);
				if(footprintPolygon.getCwPoints().size() >= 3)
				{
					holePolygons.push_back(footprintPolygon);
				}
			}
		}

		return PolygonsUnion<float>::instance()->unionPolygons(holePolygons);
	}

	CSGPolygon<float> NavMeshGenerator::computePolyhedronFootprint(const Polyhedron &polyhedron, const PolyhedronFace &walkableFace) const
	{
		std::vector<Point2<float>> footprintPoints;
		footprintPoints.reserve(polyhedron.getPoints().size() / 2); //estimated memory size

		Plane<float> walkablePlane(walkableFace.getCcwPoints()[0], walkableFace.getCcwPoints()[1], walkableFace.getCcwPoints()[2]);
		for(const auto &polyhedronFace : polyhedron.getFaces())
		{
			for(unsigned int i=0, previousI=polyhedronFace.getCcwPoints().size()-1; i<polyhedronFace.getCcwPoints().size(); previousI=i++)
			{
				float distance1 = walkablePlane.distance(polyhedronFace.getCcwPoints()[previousI]);
				float distance2 = walkablePlane.distance(polyhedronFace.getCcwPoints()[i]);
				if((distance1<0.0 && distance2>0.0) || (distance1>0.0 && distance2<0.0))
				{
					Line3D<float> polyhedronEdgeLine(polyhedronFace.getCcwPoints()[previousI], polyhedronFace.getCcwPoints()[i]);
					Point3<float> intersectionPoint = walkablePlane.intersectPoint(polyhedronEdgeLine);
					footprintPoints.emplace_back(Point2<float>(intersectionPoint.X, -intersectionPoint.Z));
				}
			}
		}

		ConvexHull2D<float> footprintConvexHull(footprintPoints);
		std::vector<Point2<float>> cwPoints(footprintConvexHull.getPoints());
		std::reverse(cwPoints.begin(), cwPoints.end());
		return CSGPolygon<float>(polyhedron.getName(), cwPoints);
	}

	std::vector<Point3<float>> NavMeshGenerator::elevateTriangulatedPoints(const Triangulation &triangulation, const PolyhedronFace &walkableFace) const
	{
        float shiftDistance = -navMeshConfig->getAgent().computeExpandDistance(walkableFace.getNormal());

		std::vector<Point3<float>> elevatedPoints;
		elevatedPoints.reserve(triangulation.getAllPointsSize());

        for(const auto &walkablePoint : triangulation.getPolygonPoints())
        {
            elevatedPoints.push_back(elevatePoints(walkablePoint, shiftDistance, walkableFace));
        }

		for(unsigned int holeIndex=0; holeIndex<triangulation.getHolesSize(); ++holeIndex)
		{
			const std::vector<Point2<float>> &holePoints = triangulation.getHolePoints(holeIndex);
			for(const auto &holePoint : holePoints)
			{
				elevatedPoints.push_back(elevatePoints(holePoint, shiftDistance, walkableFace));
			}
		}

		return elevatedPoints;
	}

    Point3<float> NavMeshGenerator::elevatePoints(const Point2<float> &point, float shiftDistance, const PolyhedronFace &walkableFace) const
    {
        Point3<float> point3D(point.X, 0.0, -point.Y);
        float shortestFaceDistance = walkableFace.getNormal().dotProduct(point3D.vector(walkableFace.getCcwPoints()[0]));
        float t = (shortestFaceDistance+shiftDistance) / walkableFace.getNormal().Y;
        return point3D.translate(t * Vector3<float>(0.0, 1.0, 0.0));
    }

	std::vector<IndexedTriangle3D<float>> NavMeshGenerator::toIndexedTriangle3D(const std::vector<IndexedTriangle2D<float>> &indexedTriangles2D) const
	{
		std::vector<IndexedTriangle3D<float>> indexedTriangles3D;
		indexedTriangles3D.reserve(indexedTriangles2D.size());

		for(const auto &indexedTriangle2D : indexedTriangles2D)
		{
			indexedTriangles3D.emplace_back(IndexedTriangle3D<float>(indexedTriangle2D.getIndices()));
		}

		return indexedTriangles3D;
	}

    std::shared_ptr<NavMesh> NavMeshGenerator::getNavMesh() const
    {
        return navMesh;
    }

}
