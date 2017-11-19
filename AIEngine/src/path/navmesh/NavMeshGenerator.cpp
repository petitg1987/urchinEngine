#include <cmath>
#include <chrono>
#include <algorithm>
#include <numeric>

#include "NavMeshGenerator.h"
#include "path/navmesh/polyhedron/PolyhedronBuilder.h"
#include "path/navmesh/csg/PolygonsUnion.h"
#include "path/navmesh/csg/PolygonsSubtraction.h"

#define WALKABLE_FACE_EXPAND_SIZE 0.0001f
#define OBSTACLE_REDUCE_SIZE 0.0001f

namespace urchin
{

	PolyhedronFaceIndex::PolyhedronFaceIndex(it_polyhedron polyhedronRef, unsigned int faceIndex) :
            polyhedronRef(polyhedronRef),
            faceIndex(faceIndex)
	{

	}

    NavMeshGenerator::NavMeshGenerator() :
            polygonMinDotProductThreshold(std::cos(AngleConverter<float>::toRadian(ConfigService::instance()->getFloatValue("navMesh.polygon.removeAngleThresholdInDegree")))),
            polygonMergePointsDistanceThreshold(ConfigService::instance()->getFloatValue("navMesh.polygon.mergePointsDistanceThreshold")),
			navMeshConfig(std::make_shared<NavMeshConfig>(NavMeshAgent())),
			navMesh(std::make_shared<NavMesh>())
    {

	}

	void NavMeshGenerator::setNavMeshConfig(std::shared_ptr<NavMeshConfig> navMeshConfig)
	{
		std::lock_guard<std::mutex> lock(navMeshMutex);

		this->navMeshConfig = std::move(navMeshConfig);
	}

    NavMesh NavMeshGenerator::retrieveLastGeneratedNavMesh() const
    {
        std::lock_guard<std::mutex> lock(navMeshMutex);

        return NavMesh(*navMesh);
    }

	std::shared_ptr<NavMesh> NavMeshGenerator::generate(AIWorld &aiWorld)
	{
		#ifdef _DEBUG
//			auto frameStartTime = std::chrono::high_resolution_clock::now();
        #endif

		updateExpandedPolyhedrons(aiWorld);
		std::vector<PolyhedronFaceIndex> polyhedronWalkableFaces = findWalkableFaces();

		std::lock_guard<std::mutex> lock(navMeshMutex);
        navMesh.reset(new NavMesh());
        for (const auto &polyhedronWalkableFace : polyhedronWalkableFaces)
        {
            std::vector<std::shared_ptr<NavPolygon>> navPolygons = createNavigationPolygonFor(polyhedronWalkableFace);
            for (const auto &navPolygon : navPolygons)
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

	void NavMeshGenerator::updateExpandedPolyhedrons(AIWorld &aiWorld)
	{
		for(auto &aiObjectToRemove : aiWorld.getObjectsToRemove())
		{
			expandedPolyhedrons.erase(aiObjectToRemove);
		}
		aiWorld.removeObjectsTagged();

		for(auto &aiObject : aiWorld.getObjects())
		{
			if(!aiObject->isToRebuild())
			{
				continue;
			}

            expandedPolyhedrons.erase(aiObject);
            std::vector<std::unique_ptr<Polyhedron>> polyhedrons = PolyhedronBuilder::instance()->buildPolyhedrons(aiObject);
            for(auto &polyhedron : polyhedrons) //TODO fix memory problems
            {
                polyhedron->expand(navMeshConfig->getAgent());
                expandedPolyhedrons.insert(std::pair<std::shared_ptr<AIObject>, std::unique_ptr<Polyhedron>>(aiObject, std::move(polyhedron)));
            }

			aiObject->markRebuilt();
		}
	}

	std::vector<PolyhedronFaceIndex> NavMeshGenerator::findWalkableFaces() const
	{
		std::vector<PolyhedronFaceIndex> walkableFaces;
		walkableFaces.reserve(expandedPolyhedrons.size()/2); //estimated memory size

        for(auto itPolyhedron = expandedPolyhedrons.begin(); itPolyhedron!=expandedPolyhedrons.end(); ++itPolyhedron)
		{
			const std::unique_ptr<Polyhedron> &expandedPolyhedron = itPolyhedron->second;
			if(expandedPolyhedron->isWalkableCandidate())
			{
				for(unsigned int faceIndex=0; faceIndex<expandedPolyhedron->getFaces().size(); ++faceIndex)
				{
					const PolyhedronFace &face = expandedPolyhedron->getFace(faceIndex);

					if(face.isWalkableCandidate() && std::fabs(face.getAngleToHorizontal()) < navMeshConfig->getMaxSlope())
					{
						walkableFaces.emplace_back(PolyhedronFaceIndex(itPolyhedron, faceIndex));
					}
				}
			}
		}

		return walkableFaces;
	}

	std::vector<std::shared_ptr<NavPolygon>> NavMeshGenerator::createNavigationPolygonFor(const PolyhedronFaceIndex &polyhedronWalkableFace) const
	{
		const std::unique_ptr<Polyhedron> &polyhedron = polyhedronWalkableFace.polyhedronRef->second;
		const PolyhedronFace &walkableFace = polyhedron->getFace(polyhedronWalkableFace.faceIndex);

		std::string walkableName = polyhedron->getName() + "[" + std::to_string(polyhedronWalkableFace.faceIndex) + "]";
        std::vector<bool> isExternalPoints(walkableFace.getCcwPoints().size(), true);
        std::vector<CSGPolygon<float>> walkablePolygons = {CSGPolygon<float>(walkableName, reverseAndFlatPointsOnYAxis(walkableFace.getCcwPoints()))};

        auto obstaclePolygons = computeObstacles(polyhedronWalkableFace);
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

				std::string navPolygonName = "<" + simplifiedWalkablePolygons.getName() + ">";
				TriangulationAlgorithm triangulation(reversePoints(extendedWalkableCwPoints));

                for(const auto &remainingObstaclePolygon : remainingObstaclePolygons)
                {
                    if(simplifiedWalkablePolygons.pointInsideOrOnPolygon(remainingObstaclePolygon.getCwPoints()[0]))
                    { //obstacle fully inside walkable polygon
                        triangulation.addHolePoints(remainingObstaclePolygon.getCwPoints());
						navPolygonName += " - <" + remainingObstaclePolygon.getName() + ">";
                    }
                }

                std::vector<Point3<float>> points = elevateTriangulatedPoints(triangulation, walkableFace);
                navPolygons.push_back(std::make_shared<NavPolygon>(navPolygonName, points, triangulation.triangulate()));
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

	std::vector<CSGPolygon<float>> NavMeshGenerator::computeObstacles(const PolyhedronFaceIndex &polyhedronWalkableFace) const
	{
		const std::unique_ptr<Polyhedron> &polyhedron = polyhedronWalkableFace.polyhedronRef->second;
		const PolyhedronFace &walkableFace = polyhedron->getFace(polyhedronWalkableFace.faceIndex);

		std::vector<CSGPolygon<float>> holePolygons;
        for (const auto &expandedPolyhedron : expandedPolyhedrons)
        {
			if(expandedPolyhedron.first!=polyhedronWalkableFace.polyhedronRef->first && expandedPolyhedron.second->isObstacleCandidate())
			{
				CSGPolygon<float> footprintPolygon = computePolyhedronFootprint(expandedPolyhedron.second, walkableFace);
				if(footprintPolygon.getCwPoints().size() >= 3)
				{
					holePolygons.push_back(footprintPolygon);
				}
			}
		}

		return PolygonsUnion<float>::instance()->unionPolygons(holePolygons);
	}

	CSGPolygon<float> NavMeshGenerator::computePolyhedronFootprint(const std::unique_ptr<Polyhedron> &polyhedron, const PolyhedronFace &walkableFace) const
	{
		std::vector<Point2<float>> footprintPoints;
		footprintPoints.reserve(polyhedron->getPoints().size() / 2); //estimated memory size

		Plane<float> walkablePlane(walkableFace.getCcwPoints()[0], walkableFace.getCcwPoints()[1], walkableFace.getCcwPoints()[2]);
		for(const auto &polyhedronFace : polyhedron->getFaces())
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
		return CSGPolygon<float>(polyhedron->getName(), cwPoints);
	}

	std::vector<Point3<float>> NavMeshGenerator::elevateTriangulatedPoints(const TriangulationAlgorithm &triangulation, const PolyhedronFace &walkableFace) const
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

}
