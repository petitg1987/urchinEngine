#include "NavMeshDisplayer.h"

namespace urchin
{

	NavMeshDisplayer::NavMeshDisplayer(SceneManager *sceneManager) :
			sceneManager(sceneManager)
	{

	}

	NavMeshDisplayer::~NavMeshDisplayer()
	{
		cleanCurrentDisplay();
	}

	void NavMeshDisplayer::displayNavMesh(std::shared_ptr<const NavMesh> navMesh)
	{
		cleanCurrentDisplay();

		if(navMesh)
		{ //TODO: avoid continuously reconstruction of models
			for(const auto &navPolygon : navMesh->getPolygons())
			{
				unsigned int nbPoints = navPolygon.getPoints().size();
				for(unsigned int i=0; i<nbPoints; ++i)
				{
					unsigned nextI = (i+1)%nbPoints;
					Line3D<float> line(navPolygon.getPoint(i), navPolygon.getPoint(nextI));
					navMeshModels.push_back(new LineModel(line, 5.0));
				}
			}

			for(unsigned int i=0; i<navMeshModels.size(); ++i)
			{
				navMeshModels[i]->setColor(0.0, 1.0, 0.0);
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
