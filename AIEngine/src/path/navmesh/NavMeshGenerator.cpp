#include <cmath>

#include "NavMeshGenerator.h"
#include "path/navmesh/polyhedron/Face.h"

namespace urchin
{

	NavMeshGenerator::NavMeshGenerator(std::shared_ptr<AIWorld> aiWorld, NavMeshConfig navMeshConfig) :
		aiWorld(aiWorld),
		navMeshConfig(navMeshConfig)
	{

	}

	std::shared_ptr<NavMesh> NavMeshGenerator::generate()
	{
		std::vector<Polyhedron> polyhedrons = createPolyhedrons();

		std::shared_ptr<NavMesh> navMesh = std::make_shared<NavMesh>();
		extractWalkablePolygons(polyhedrons, navMesh);

		//TODO High level algorithm v2
		//1. Create polyhedrons: list<Face> & Face: list<Point3> (coplanar & clockwise oriented)
		//2. Expand polyhedrons. Bonus: create bevel planes
		//3. Find walkable faces (dot product with up vector)
		//4. For each walkable faces:
		//		- Create new object containing points of walkable faces
		//		- Find expanded polyhedrons which create holes on current walkable face. Add holes points on new object
		//		- Find walkable faces which can be linked to this one and add points to new objects
		//		- Triangulate new object by taking into account: walkable faces points, holes points and links points. Use: constrained delaunay triangulation.

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
				polyhedrons.push_back(createPolyhedronFor(box));
			} //TODO others shape types
		}

		return polyhedrons;
	}

	Polyhedron NavMeshGenerator::createPolyhedronFor(OBBox<float> *box)
	{
		std::vector<Face> faces;

		Vector3<float> upVector(0.0, 1.0, 0.0);
		for(unsigned int axisIndex=0; axisIndex<3; ++axisIndex)
		{
			for(unsigned int axisDirection=0; axisDirection<2; ++axisDirection)
			{
				Vector3<float> boxAxis = axisDirection==0 ? box->getAxis(axisIndex) : -box->getAxis(axisIndex);

				unsigned int nextAxisIndex = (axisIndex+1)%3;
				unsigned int previousAxisIndex = (axisIndex+2)%3;

				Point3<float> point1 = box->getCenterOfMass().translate(box->getHalfSize(axisIndex) * boxAxis
						+ box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						+ box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				Point3<float> point2 = box->getCenterOfMass().translate(box->getHalfSize(axisIndex) * boxAxis
						+ box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						- box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				Point3<float> point3 = box->getCenterOfMass().translate(box->getHalfSize(axisIndex) * boxAxis
						- box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						+ box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				Point3<float> point4 = box->getCenterOfMass().translate(box->getHalfSize(axisIndex) * boxAxis
						- box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						- box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				std::vector<Point3<float>> unsortedPoints({point1, point2, point3, point4});
				std::vector<Point3<float>> sortedPoints = SortPointsService<float>::instance()->sortPointsCounterClockwise(unsortedPoints, boxAxis);

				faces.push_back(Face(sortedPoints));
			}
		}

		return Polyhedron(faces);
	}

	void NavMeshGenerator::extractWalkablePolygons(const std::vector<Polyhedron> &polyhedrons, std::shared_ptr<NavMesh> navMesh)
	{
		Vector3<float> upVector(0.0, 1.0, 0.0);
		for(const auto &polyhedron : polyhedrons)
		{
			for(const auto &face : polyhedron.getFaces())
			{
				float angleToHorizontal = std::acos(face.getNormal().dotProduct(upVector));
				if(std::fabs(angleToHorizontal) < navMeshConfig.getMaxSlope())
				{
					navMesh->addPolygon(NavPolygon(face.getPoints()));
				}
			}
		}
	}


}
