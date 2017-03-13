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
			for(const auto &navPolygon : navMesh->getPolygons())
			{
				TrianglesModel *triangleModel = new TrianglesModel(navPolygon.getPoints(), navPolygon.getTriangles());
				triangleModel->setColor(0.0, 0.0, 1.0, 0.5);
				triangleModel->setPolygonMode(GeometryModel::FILL);
				navMeshModels.push_back(triangleModel);
			}

			for(const auto &navPolygon : navMesh->getPolygons())
			{
				TrianglesModel *triangleModel = new TrianglesModel(navPolygon.getPoints(), navPolygon.getTriangles());
				triangleModel->setColor(0.0, 0.0, 1.0, 1.0);
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

}
