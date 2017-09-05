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
				TrianglesModel *triangleModel = new TrianglesModel(toDisplayPoints(navPolygon->getPoints(), Y_ELEVATION_FILL),
																   toDisplayTriangles(navPolygon->getTriangles()));
				triangleModel->setBlendMode(GeometryModel::ONE_MINUS_SRC_ALPHA);
				triangleModel->setColor(0.0, 0.0, 1.0, 0.5);
				triangleModel->setPolygonMode(GeometryModel::FILL);
				navMeshModels.push_back(triangleModel);
			}

			for(const auto &navPolygon : navMesh->getPolygons())
			{
				TrianglesModel *triangleModel = new TrianglesModel(toDisplayPoints(navPolygon->getPoints(), Y_ELEVATION_WIREFRAME),
																   toDisplayTriangles(navPolygon->getTriangles()));
				triangleModel->setLineSize(4.0);
				triangleModel->setColor(0.3, 0.3, 1.0, 1.0);
				triangleModel->setPolygonMode(GeometryModel::WIREFRAME);
				navMeshModels.push_back(triangleModel);
			}

			for (auto &navMeshModel : navMeshModels)
            {
				sceneManager->getActiveRenderer3d()->addGeometry(navMeshModel);
			}
		}
	}

	void NavMeshDisplayer::cleanCurrentDisplay()
	{
		for (auto &navMeshModel : navMeshModels)
        {
			sceneManager->getActiveRenderer3d()->removeGeometry(navMeshModel);
			delete navMeshModel;
		}

		navMeshModels.clear();
	}

	std::vector<Point3<float>> NavMeshDisplayer::toDisplayPoints(const std::vector<Point3<float>> &points, float yElevation) const
	{
		std::vector<Point3<float>> displayPoints;
		displayPoints.reserve(points.size());

		for(const auto &point : points)
		{
			//avoid Y fighting with walkable face
			displayPoints.emplace_back(Point3<float>(point.X, point.Y + yElevation, point.Z));
		}

		return displayPoints;
	}

	std::vector<IndexedTriangle3D<float>> NavMeshDisplayer::toDisplayTriangles(const std::vector<NavTriangle> &triangles) const
	{
		std::vector<IndexedTriangle3D<float>> displayTriangles;
		displayTriangles.reserve(triangles.size());

		for(const auto &triangle : triangles)
		{
			displayTriangles.emplace_back(IndexedTriangle3D<float>(triangle.getIndices()));
		}

		return displayTriangles;
	}

}
