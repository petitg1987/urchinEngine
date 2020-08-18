#include "Meshes.h"

namespace urchin
{

    Meshes::Meshes(ConstMeshes *constMeshes) :
            constMeshes(constMeshes),
            numMeshes(constMeshes->getNumberConstMeshes())
    {
        //create mesh
        for(unsigned int m=0; m<numMeshes; m++)
        {
            auto *mesh = new Mesh(constMeshes->getConstMesh(m));
            meshes.push_back(mesh);
        }
    }

    Meshes::~Meshes()
    {
        for(unsigned int i=0;i<numMeshes;i++)
        {
            delete meshes[i];
        }

        constMeshes->release();
    }

    unsigned int Meshes::getNumberMeshes() const
    {
        return numMeshes;
    }

    Mesh *Meshes::getMesh(unsigned int index) const
    {
        return meshes[index];
    }

    const AABBox<float> &Meshes::getGlobalAABBox() const
    {
        return globalBBox;
    }

    const std::vector<AABBox<float>> &Meshes::getGlobalSplitAABBoxes() const
    {
        return globalSplitBBoxes;
    }

    const AABBox<float> &Meshes::getGlobalLocalAABBox() const
    {
        return constMeshes->getOriginalAABBox();
    }

    const ConstMeshes *Meshes::getConstMeshes() const
    {
        return constMeshes;
    }

    void Meshes::onMoving(const Transform<float> &newTransform)
    {
        globalBBox = constMeshes->getOriginalAABBox().moveAABBox(newTransform);

        globalSplitBBoxes.clear();
        for (const auto &originalSplitAABBox : constMeshes->getOriginalSplitAABBoxes())
        {
            globalSplitBBoxes.push_back(originalSplitAABBox.moveAABBox(newTransform));
        }
    }

}
