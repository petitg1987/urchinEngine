#include <limits>

#include "NavMeshDisplayer.h"

namespace urchin
{

	NavMeshDisplayer::NavMeshDisplayer(SceneManager *sceneManager) :
			sceneManager(sceneManager),
			loadedNavMeshId(std::numeric_limits<unsigned int>::max())
	{

	}

	NavMeshDisplayer::~NavMeshDisplayer()
	{
		cleanCurrentDisplay();
	}

	void NavMeshDisplayer::displayNavMesh(std::shared_ptr<const NavMesh> navMesh)
	{
		cleanCurrentDisplay();

		if(navMesh && loadedNavMeshId!=navMesh->getId())
		{
			constexpr float Y_ELEVATION_FILL = 0.01f;
			constexpr float Y_ELEVATION_WIREFRAME = 0.015f;

			for(const auto &navPolygon : navMesh->getPolygons())
			{
				TrianglesModel *triangleModel = new TrianglesModel(toDisplayPoints(navPolygon->getPoints(), Y_ELEVATION_FILL), navPolygon->getTriangles());
				triangleModel->setBlendMode(GeometryModel::ONE_MINUS_SRC_ALPHA);
				triangleModel->setColor(0.0, 0.0, 1.0, 0.5);
				triangleModel->setPolygonMode(GeometryModel::FILL);
				navMeshModels.push_back(triangleModel);
			}

			for(const auto &navPolygon : navMesh->getPolygons())
			{
				TrianglesModel *triangleModel = new TrianglesModel(toDisplayPoints(navPolygon->getPoints(), Y_ELEVATION_WIREFRAME), navPolygon->getTriangles());
				triangleModel->setLineSize(4.0);
				triangleModel->setColor(0.3, 0.3, 1.0, 1.0);
				triangleModel->setPolygonMode(GeometryModel::WIREFRAME);
				navMeshModels.push_back(triangleModel);
			}

			for(unsigned int i=0; i<navMeshModels.size(); ++i)
			{
				sceneManager->getActiveRenderer3d()->addGeometry(navMeshModels[i]);
			}
		}
	}

	void NavMeshDisplayer::cleanCurrentDisplay()
	{
		for(unsigned int i=0; i<navMeshModels.size(); ++i)
		{
			sceneManager->getActiveRenderer3d()->removeGeometry(navMeshModels[i]);
			delete navMeshModels[i];
		}

		navMeshModels.clear();
	}

	std::vector<Point3<float>> NavMeshDisplayer::toDisplayPoints(const std::vector<Point3<float>> &points, float yElevation)
	{
		std::vector<Point3<float>> displayPoints;
		displayPoints.reserve(points.size());

		for(const auto &point : points)
		{
			//avoid Y fighting with walkable face
			displayPoints.push_back(Point3<float>(point.X, point.Y + yElevation, point.Z));
		}

		return displayPoints;
	}

}
