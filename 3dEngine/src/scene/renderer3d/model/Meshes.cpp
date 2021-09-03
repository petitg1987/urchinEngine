#include <scene/renderer3d/model/Meshes.h>

namespace urchin {

    Meshes::Meshes(std::shared_ptr<ConstMeshes> constMeshes) :
            constMeshes(std::move(constMeshes)),
            numMeshes(this->constMeshes->getNumberConstMeshes()) {
        //create mesh
        for (unsigned int m = 0;  m < numMeshes; m++) {
            meshes.push_back(std::make_unique<Mesh>(this->constMeshes->getConstMesh(m)));
        }
    }

    unsigned int Meshes::getNumberMeshes() const {
        return numMeshes;
    }

    Mesh& Meshes::getMesh(unsigned int index) const {
        return *meshes[index];
    }

    const AABBox<float>& Meshes::getGlobalAABBox() const {
        return globalBBox;
    }

    const std::vector<AABBox<float>>& Meshes::getGlobalSplitAABBoxes() const {
        return globalSplitBBoxes;
    }

    const AABBox<float>& Meshes::getGlobalLocalAABBox() const {
        return constMeshes->getOriginalAABBox();
    }

    const ConstMeshes& Meshes::getConstMeshes() const {
        return *constMeshes;
    }

    std::shared_ptr<ConstMeshes> Meshes::getConstMeshesPtr() const {
        return constMeshes;
    }

    void Meshes::onMoving(const Transform<float>& newTransform) {
        globalBBox = constMeshes->getOriginalAABBox().moveAABBox(newTransform);

        globalSplitBBoxes.clear();
        for (const auto& originalSplitAABBox : constMeshes->getOriginalSplitAABBoxes()) {
            globalSplitBBoxes.push_back(originalSplitAABBox.moveAABBox(newTransform));
        }
    }

}
