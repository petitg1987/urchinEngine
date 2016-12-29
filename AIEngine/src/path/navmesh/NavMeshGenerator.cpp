#include "NavMeshGenerator.h"

namespace urchin
{

	NavMeshGenerator::NavMeshGenerator(std::shared_ptr<AIWorld> aiWorld, NavMeshConfig navMeshConfig) :
		aiWorld(aiWorld),
		navMeshConfig(navMeshConfig)
	{

	}

	std::shared_ptr<NavMesh> NavMeshGenerator::generate()
	{
		std::shared_ptr<NavMesh> navMesh = std::make_shared<NavMesh>();

		createNavPolygons(navMesh);

		//1. decompose AIObject in faces/planes
		//2. expand these plane
		//3. eliminate unnecessary faces/planes (CSG)
		//4. create links
		//5. triangulation (delaunay triangulation c++ 3d)

		return navMesh;
	}

	void NavMeshGenerator::createNavPolygons(std::shared_ptr<NavMesh> navMesh)
	{
		for(auto &aiObject : aiWorld->getObjects())
		{
			std::unique_ptr<ConvexObject3D<float>> object = aiObject.getShape()->toConvexObject(aiObject.getTransform());

			if(OBBox<float> *box = dynamic_cast<OBBox<float>*>(object.get()))
			{
				createNavPolygonFor(box, navMesh);
			} //TODO others shape types
		}
	}

	void NavMeshGenerator::createNavPolygonFor(OBBox<float> *box, std::shared_ptr<NavMesh> navMesh)
	{
		Vector3<float> upVector(0.0, 1.0, 0.0);
		for(unsigned int axisIndex=0; axisIndex<3; ++axisIndex)
		{
			bool invertAxis = box->getAxis(axisIndex).Y < 0.0f;
			Vector3<float> boxAxis = invertAxis ? -box->getAxis(axisIndex) : box->getAxis(axisIndex);

			float angleToHorizontal = std::acos(boxAxis.dotProduct(upVector));
			if(std::fabs(angleToHorizontal) < navMeshConfig.getMaxSlope())
			{
				unsigned int nextAxisIndex = (axisIndex+1)%3;
				unsigned int previousAxisIndex = (axisIndex+2)%3;

				Point3<float> point1 = box->getCenterPosition().translate(box->getHalfSize(axisIndex) * boxAxis
						+ box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						+ box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				Point3<float> point2 = box->getCenterPosition().translate(box->getHalfSize(axisIndex) * boxAxis
						+ box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						- box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				Point3<float> point3 = box->getCenterPosition().translate(box->getHalfSize(axisIndex) * boxAxis
						- box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						+ box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				Point3<float> point4 = box->getCenterPosition().translate(box->getHalfSize(axisIndex) * boxAxis
						- box->getHalfSize(nextAxisIndex) * box->getAxis(nextAxisIndex)
						- box->getHalfSize(previousAxisIndex) * box->getAxis(previousAxisIndex));

				std::vector<Point3<float>> unsortedPoints({point1, point2, point3, point4});
				std::vector<Point3<float>> sortedPoints = SortPointsService<float>::instance()->sortPointsClockwise(unsortedPoints, boxAxis);

				navMesh->addPolygon(NavPolygon(sortedPoints));
			}
		}
	}
}
