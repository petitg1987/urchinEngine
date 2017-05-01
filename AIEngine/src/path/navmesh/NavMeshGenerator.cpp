#include <cmath>
#include <chrono>

#include "NavMeshGenerator.h"
#include "path/navmesh/polyhedron/Face.h"
#include "path/navmesh/csg/CSGPolygon.h"
#include "path/navmesh/csg/PolygonsUnion.h"

namespace urchin
{

	NavMeshGenerator::NavMeshGenerator(std::shared_ptr<AIWorld> aiWorld, NavMeshConfig navMeshConfig) :
		aiWorld(aiWorld),
		navMeshConfig(navMeshConfig)
	{

	}

	std::shared_ptr<NavMesh> NavMeshGenerator::generate()
	{
		auto frameStartTime = std::chrono::high_resolution_clock::now(); //TODO remove counter:

		std::vector<Polyhedron> polyhedrons = createPolyhedrons();
		std::vector<PolyhedronFace> walkableFaces = findWalkableFaces(polyhedrons);
		std::shared_ptr<NavMesh> navMesh = std::make_shared<NavMesh>();

		for(const auto &walkableFace : walkableFaces)
		{
			NavPolygon navPolygon = createNavigationPolygonFor(walkableFace, polyhedrons);
			navMesh->addPolygon(navPolygon);
		}

		auto frameEndTime = std::chrono::high_resolution_clock::now();
		auto diffTimeMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count();
		std::cout<<"Nav mesh generation time (ms): "<<diffTimeMicroSeconds/1000.0<<std::endl;

		return navMesh;
	}

	std::vector<Polyhedron> NavMeshGenerator::createPolyhedrons()
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

		return polyhedrons;
	}

	/**
	 * Returne box faces. Faces are guaranteed to be in the following order: right, left, top, bottom, front, back.
	 */
	std::vector<Face> NavMeshGenerator::createPolyhedronFaces(OBBox<float> *box)
	{
		std::vector<Face> faces;
		faces.reserve(6);

		std::vector<Point3<float>> points = box->getPoints();
		faces.push_back(Face({points[0], points[2], points[3], points[1]})); //right
		faces.push_back(Face({points[4], points[5], points[7], points[6]})); //left
		faces.push_back(Face({points[0], points[1], points[5], points[4]})); //top
		faces.push_back(Face({points[3], points[2], points[6], points[7]})); //bottom
		faces.push_back(Face({points[0], points[4], points[6], points[2]})); //front
		faces.push_back(Face({points[1], points[3], points[7], points[5]})); //back

		return faces;
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, OBBox<float> *box)
	{
		return Polyhedron(name, createPolyhedronFaces(box), box->getPoints());
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Capsule<float> *capsule)
	{
		Vector3<float> boxHalfSizes(capsule->getRadius(), capsule->getRadius(), capsule->getRadius());
		boxHalfSizes[capsule->getCapsuleOrientation()] += capsule->getCylinderHeight() / 2.0f;

		OBBox<float> capsuleBox(boxHalfSizes, capsule->getCenterOfMass(), capsule->getOrientation());
		Polyhedron polyhedron = createPolyhedronFor(name, &capsuleBox);
		polyhedron.setWalkableCandidate(false);

		return polyhedron;
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Cone<float> *cone)
	{
		Vector3<float> boxHalfSizes(cone->getRadius(), cone->getRadius(), cone->getRadius());
		boxHalfSizes[cone->getConeOrientation()/2] = cone->getHeight() / 2.0f;

		OBBox<float> coneBox(boxHalfSizes, cone->getCenter(), cone->getOrientation());
		Polyhedron polyhedron = createPolyhedronFor(name, &coneBox);
		polyhedron.setWalkableCandidate(false);

		return polyhedron;
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, ConvexHull3D<float> *convexHull)
	{
		const std::map<unsigned int, IndexedTriangle3D<float>> &indexedTriangles = convexHull->getIndexedTriangles();
		const std::map<unsigned int, ConvexHullPoint<float>> &points = convexHull->getConvexHullPoints();

		std::vector<Face> faces;
		faces.reserve(indexedTriangles.size());

		for(const auto &indexedTriangle : indexedTriangles)
		{
			const unsigned int *indexes = indexedTriangle.second.getIndexes();
			Point3<float> point1 = points.at(indexes[0]).point;
			Point3<float> point2 = points.at(indexes[1]).point;
			Point3<float> point3 = points.at(indexes[2]).point;

			faces.push_back(Face({point1, point2, point3}));
		}

		return Polyhedron(name, faces, convexHull->getPoints());
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Cylinder<float> *cylinder)
	{
		Vector3<float> boxHalfSizes(cylinder->getRadius(), cylinder->getRadius(), cylinder->getRadius());
		boxHalfSizes[cylinder->getCylinderOrientation()] = cylinder->getHeight() / 2.0f;

		OBBox<float> cylinderBox(boxHalfSizes, cylinder->getCenterOfMass(), cylinder->getOrientation());
		std::vector<Face> faces = createPolyhedronFaces(&cylinderBox);
		for(unsigned int i=0; i<faces.size(); ++i)
		{
			faces[i].setWalkableCandidate(cylinder->getCylinderOrientation()==i/2);
		}

		return Polyhedron(name, faces, cylinderBox.getPoints());
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(const std::string &name, Sphere<float> *sphere)
	{
		OBBox<float> sphereBox(*sphere);
		Polyhedron polyhedron = createPolyhedronFor(name, &sphereBox);
		polyhedron.setWalkableCandidate(false);

		return polyhedron;
	}

	std::vector<PolyhedronFace> NavMeshGenerator::findWalkableFaces(const std::vector<Polyhedron> &polyhedrons)
	{
		std::vector<PolyhedronFace> walkableFaces;
		walkableFaces.reserve(polyhedrons.size()/2); //estimated memory size

		for(unsigned int polyhedronIndex=0; polyhedronIndex<polyhedrons.size(); ++polyhedronIndex)
		{
			const Polyhedron &polyhedron = polyhedrons[polyhedronIndex];
			if(polyhedron.isWalkableCandidate())
			{
				if(polyhedron.getName().compare("ground")!=0)
					continue; //TODO remove me...

				for(unsigned int faceIndex=0; faceIndex<polyhedron.getFaceSize(); ++faceIndex)
				{
					const Face &face = polyhedron.getFace(faceIndex);

					if(face.isWalkableCandidate() && std::fabs(face.getAngleToHorizontal()) < navMeshConfig.getMaxSlope())
					{
						PolyhedronFace polyhedronFace;
						polyhedronFace.polyhedronIndex = polyhedronIndex;
						polyhedronFace.faceIndex = faceIndex;

						walkableFaces.push_back(polyhedronFace);
					}
				}
			}
		}

		return walkableFaces;
	}

	NavPolygon NavMeshGenerator::createNavigationPolygonFor(const PolyhedronFace &polyhedronFace, const std::vector<Polyhedron> &polyhedrons)
	{
		const Polyhedron &polydedron = polyhedrons[polyhedronFace.polyhedronIndex];
		const Face &face = polydedron.getFace(polyhedronFace.faceIndex);

		Triangulation triangulation(flatPointsOnYAxis(face.getCcwPoints()));
		addObstacles(polyhedrons, polyhedronFace.polyhedronIndex, triangulation);

		std::vector<Point3<float>> points = elevateTriangulatedPoints(triangulation, face);
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

	void NavMeshGenerator::addObstacles(const std::vector<Polyhedron> &polyhedrons, unsigned int processingPolyhedron, Triangulation &triangulation)
	{
		std::vector<CSGPolygon> holePolygons; //TODO reserve
		for(unsigned int i=0; i<polyhedrons.size(); ++i)
		{ //TODO select only polygons above 'processingPolyhedron'
			if(i!=processingPolyhedron)
			{
				std::vector<Point2<float>> cwFootprintPoints = polyhedrons[i].computeCwFootprintPoints(navMeshConfig.getAgentBox());
				holePolygons.push_back(CSGPolygon(polyhedrons[i].getName(), cwFootprintPoints));
			}
		}

		std::vector<CSGPolygon> mergedPolygons = PolygonsUnion().unionPolygons(holePolygons); //TODO union only if polygons boxes collide
		for(const auto &mergedPolygon : mergedPolygons)
		{
			triangulation.addHolePoints(mergedPolygon.getCwPoints());
		}
	}

	std::vector<Point3<float>> NavMeshGenerator::elevateTriangulatedPoints(const Triangulation &triangulation, const Face &walkableFace)
	{
		Vector3<float> verticalVector(0.0, 1.0, 0.0);
		float numerator = walkableFace.getNormal().dotProduct(verticalVector);

		std::vector<Point3<float>> elevatedPoints = walkableFace.getCcwPoints();
		elevatedPoints.reserve(triangulation.getAllPointsSize());

		for(unsigned int holeIndex=0; holeIndex<triangulation.getHolesSize(); ++holeIndex)
		{
			const std::vector<Point2<float>> &holePoints = triangulation.getHolePoints(holeIndex);
			for(const auto &holePoint : holePoints)
			{
				Point3<float> holePoint3D(holePoint.X, 0.0, -holePoint.Y);
				float t = walkableFace.getNormal().dotProduct(holePoint3D.vector(walkableFace.getCcwPoints()[0])) / numerator;
				Point3<float> projectedPoint = holePoint3D.translate(t * verticalVector);

				elevatedPoints.push_back(projectedPoint);
			}
		}

		return elevatedPoints;
	}

	std::vector<IndexedTriangle3D<float>> NavMeshGenerator::toIndexedTriangle3D(const std::vector<IndexedTriangle2D<float>> &indexedTriangles2D)
	{
		std::vector<IndexedTriangle3D<float>> indexedTriangles3D;
		indexedTriangles3D.reserve(indexedTriangles2D.size());

		for(const auto &indexedTriangle2D : indexedTriangles2D)
		{
			indexedTriangles3D.push_back(IndexedTriangle3D<float>(indexedTriangle2D.getIndexes()));
		}

		return indexedTriangles3D;
	}

}
