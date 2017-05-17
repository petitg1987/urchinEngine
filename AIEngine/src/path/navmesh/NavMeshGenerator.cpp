#include <cmath>
#include <chrono>
#include <algorithm>

#include "NavMeshGenerator.h"
#include "path/navmesh/csg/CSGPolygon.h"
#include "path/navmesh/csg/PolygonsUnion.h"
#include "path/navmesh/csg/PolygonsIntersection.h"

namespace urchin
{

	NavMeshGenerator::NavMeshGenerator(std::shared_ptr<AIWorld> aiWorld, NavMeshConfig navMeshConfig) :
		aiWorld(aiWorld),
		navMeshConfig(navMeshConfig)
	{

	}

	std::shared_ptr<NavMesh> NavMeshGenerator::generate() const
	{
		auto frameStartTime = std::chrono::high_resolution_clock::now(); //TODO remove counter:

		std::vector<Polyhedron> expandedPolyhedrons = createExpandedPolyhedrons();
		std::vector<PolyhedronFaceIndex> polyhedronWalkableFaces = findWalkableFaces(expandedPolyhedrons);
		std::shared_ptr<NavMesh> navMesh = std::make_shared<NavMesh>();

		for(const auto &polyhedronWalkableFace : polyhedronWalkableFaces)
		{
			NavPolygon navPolygon = createNavigationPolygonFor(polyhedronWalkableFace, expandedPolyhedrons);
			navMesh->addPolygon(navPolygon);
		}

		auto frameEndTime = std::chrono::high_resolution_clock::now();
		auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
		std::cout<<"Nav mesh generation time (ms): "<<diffTimeMicroSeconds/1000.0<<std::endl;

		return navMesh;
	}

	std::vector<Polyhedron> NavMeshGenerator::createExpandedPolyhedrons() const
	{
		std::vector<Polyhedron> polyhedrons;
		polyhedrons.reserve(aiWorld->getObjects().size());

		for(auto &aiObject : aiWorld->getObjects())
		{
			std::unique_ptr<ConvexObject3D<float>> object = aiObject.getShape()->toConvexObject(aiObject.getTransform());

			if(OBBox<float> *box = dynamic_cast<OBBox<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), box));
			}else if(Capsule<float> *capsule = dynamic_cast<Capsule<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), capsule));
			}else if(Cone<float> *cone = dynamic_cast<Cone<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), cone));
			}else if(ConvexHull3D<float> *convexHull = dynamic_cast<ConvexHull3D<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), convexHull));
			}else if(Cylinder<float> *cylinder = dynamic_cast<Cylinder<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), cylinder));
			}else if(Sphere<float> *sphere = dynamic_cast<Sphere<float>*>(object.get()))
			{
				polyhedrons.push_back(createPolyhedronFor(aiObject.getName(), sphere));
			} else
			{
				throw std::invalid_argument("Shape type not supported by navigation mesh generator: " + std::string(typeid(*object.get()).name()));
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

			unsigned int polyhedronPoint0Index = std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[0]));
			polyhedronPoints[polyhedronPoint0Index].faceIndices.push_back(i);

			unsigned int polyhedronPoint1Index = std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[1]));
			polyhedronPoints[polyhedronPoint1Index].faceIndices.push_back(i);

			unsigned int polyhedronPoint2Index = std::distance(convexHullPoints.begin(), convexHullPoints.find(indices[2]));
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
			polyhedron.expand(navMeshConfig.getAgentBox());
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

					if(face.isWalkableCandidate() && std::fabs(face.getAngleToHorizontal()) < navMeshConfig.getMaxSlope())
					{
						PolyhedronFaceIndex polyhedronFaceIndex;
						polyhedronFaceIndex.polyhedronIndex = polyhedronIndex;
						polyhedronFaceIndex.faceIndex = faceIndex;

						walkableFaces.push_back(polyhedronFaceIndex);
					}
				}
			}
		}

		return walkableFaces;
	}

	NavPolygon NavMeshGenerator::createNavigationPolygonFor(const PolyhedronFaceIndex &polyhedronWalkableFace, const std::vector<Polyhedron> &expandedPolyhedrons) const
	{
		const Polyhedron &polydedron = expandedPolyhedrons[polyhedronWalkableFace.polyhedronIndex];
		const PolyhedronFace &walkableFace = polydedron.getFace(polyhedronWalkableFace.faceIndex);

		Triangulation triangulation(flatPointsOnYAxis(walkableFace.getCcwPoints()));
		addObstacles(expandedPolyhedrons, polyhedronWalkableFace, triangulation);

		std::vector<Point3<float>> points = elevateTriangulatedPoints(triangulation, walkableFace);
		const std::vector<IndexedTriangle3D<float>> &triangles = toIndexedTriangle3D(triangulation.triangulate());

		return NavPolygon(points, triangles);
	}

	std::vector<Point2<float>> NavMeshGenerator::flatPointsOnYAxis(const std::vector<Point3<float>> &points) const
	{
		std::vector<Point2<float>> flatPoints;
		flatPoints.reserve(points.size());

		for(const auto &point : points)
		{
			flatPoints.push_back(Point2<float>(point.X, -point.Z));
		}

		return flatPoints;
	}

	void NavMeshGenerator::addObstacles(const std::vector<Polyhedron> &expandedPolyhedrons, const PolyhedronFaceIndex &polyhedronWalkableFace, Triangulation &triangulation) const
	{
		const Polyhedron &polydedron = expandedPolyhedrons[polyhedronWalkableFace.polyhedronIndex];
		const PolyhedronFace &walkableFace = polydedron.getFace(polyhedronWalkableFace.faceIndex);
		CSGPolygon walkableFacePolygon = walkableFace.computeCSGPolygon(polydedron.getName());
		CSGPolygon reducedWalkableFacePolygon = walkableFacePolygon.expand(-0.1f);

		std::vector<CSGPolygon> holePolygons;
		for(unsigned int i=0; i<expandedPolyhedrons.size(); ++i)
		{ //TODO select only polygons AABBox above 'walkFace' and reserve 'holePolygons'
			if(i!=polyhedronWalkableFace.polyhedronIndex)
			{
				CSGPolygon *footprintPolygon = expandedPolyhedrons[i].getOrComputeCSGPolygon().get();
				CSGPolygon footprintPolygonOnWalkableFace = PolygonsIntersection().intersectionPolygons(*footprintPolygon, reducedWalkableFacePolygon);
				if(footprintPolygonOnWalkableFace.getCwPoints().size() >= 3)
				{
					holePolygons.push_back(footprintPolygonOnWalkableFace);
				}
			}
		}

		std::vector<CSGPolygon> mergedPolygons = PolygonsUnion().unionPolygons(holePolygons);
		for(const auto &mergedPolygon : mergedPolygons)
		{
			triangulation.addHolePoints(mergedPolygon.getCwPoints());
		}
	}

	std::vector<Point3<float>> NavMeshGenerator::elevateTriangulatedPoints(const Triangulation &triangulation, const PolyhedronFace &walkableFace) const
	{
		std::vector<Point3<float>> elevatedPoints = walkableFace.getCcwPoints();
		elevatedPoints.reserve(triangulation.getAllPointsSize());

		for(unsigned int holeIndex=0; holeIndex<triangulation.getHolesSize(); ++holeIndex)
		{
			const std::vector<Point2<float>> &holePoints = triangulation.getHolePoints(holeIndex);
			for(const auto &holePoint : holePoints)
			{
				Point3<float> holePoint3D(holePoint.X, 0.0, -holePoint.Y);
				float t = walkableFace.getNormal().dotProduct(holePoint3D.vector(walkableFace.getCcwPoints()[0])) / walkableFace.getNormal().Y;
				Point3<float> projectedPoint = holePoint3D.translate(t * Vector3<float>(0.0, 1.0, 0.0));

				elevatedPoints.push_back(projectedPoint);
			}
		}

		return elevatedPoints;
	}

	std::vector<IndexedTriangle3D<float>> NavMeshGenerator::toIndexedTriangle3D(const std::vector<IndexedTriangle2D<float>> &indexedTriangles2D) const
	{
		std::vector<IndexedTriangle3D<float>> indexedTriangles3D;
		indexedTriangles3D.reserve(indexedTriangles2D.size());

		for(const auto &indexedTriangle2D : indexedTriangles2D)
		{
			indexedTriangles3D.push_back(IndexedTriangle3D<float>(indexedTriangle2D.getIndices()));
		}

		return indexedTriangles3D;
	}

}
