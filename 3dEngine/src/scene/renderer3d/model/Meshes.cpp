#include "Meshes.h"

namespace urchin
{

	Meshes::Meshes(const ConstMeshes *const constMeshes) :
			constMeshes(constMeshes),
			numMeshes(constMeshes->getNumberConstMeshes())
	{
		//create mesh
		for(unsigned int m=0; m<numMeshes; m++)
		{
			Mesh *mesh = new Mesh(constMeshes->getConstMesh(m));
			meshes.push_back(mesh);
		}
	}

	Meshes::~Meshes()
	{
		for(unsigned int i=0;i<numMeshes;i++)
		{
			delete meshes[i];
		}
	}

	unsigned int Meshes::getNumberMeshes() const
	{
		return numMeshes;
	}

	Mesh *Meshes::getMesh(unsigned int index)
	{
		return meshes[index];
	}

	const AABBox<float> &Meshes::getGlobalAABBox() const
	{
		return globalBBox;
	}

	const std::vector<AABBox<float>> &Meshes::getGlobalSplittedAABBox() const
	{
		return globalSplittedBBox;
	}

	const AABBox<float> &Meshes::getGlobalLocalAABBox() const
	{
		return constMeshes->getOriginalAABBox();
	}

	void Meshes::onMoving(const Transform<float> &newTransform)
	{
		globalBBox = constMeshes->getOriginalAABBox().moveAABBox(newTransform);

		globalSplittedBBox.clear();
		const std::vector<AABBox<float>> &originalSplittedAABBox = constMeshes->getOriginalSplittedAABBox();
		for(unsigned int i=0; i<originalSplittedAABBox.size(); ++i)
		{
			globalSplittedBBox.push_back(originalSplittedAABBox[i].moveAABBox(newTransform));
		}
	}

}
