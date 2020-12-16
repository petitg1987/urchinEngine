#include <limits>
#include <utility>

#include "resources/model/ConstMeshes.h"
#include "resources/model/boundingbox/SplitBoundingBox.h"

namespace urchin {

    ConstMeshes::ConstMeshes(std::string meshFilename, const std::vector<const ConstMesh*>& constMeshes) :
            meshFilename(std::move(meshFilename)),
            constMeshes(constMeshes) {
        //determines the bounding box
        Point3<float> min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point3<float> max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (auto constMesh : constMeshes) {
            for (unsigned int i = 0; i < constMesh->getNumberVertices(); i++) {
                if (min.X > constMesh->getBaseVertices()[i].X) {
                    min.X = constMesh->getBaseVertices()[i].X;
                }
                if (min.Y > constMesh->getBaseVertices()[i].Y) {
                    min.Y = constMesh->getBaseVertices()[i].Y;
                }
                if (min.Z > constMesh->getBaseVertices()[i].Z) {
                    min.Z = constMesh->getBaseVertices()[i].Z;
                }

                if (max.X < constMesh->getBaseVertices()[i].X) {
                    max.X = constMesh->getBaseVertices()[i].X;
                }
                if (max.Y < constMesh->getBaseVertices()[i].Y) {
                    max.Y = constMesh->getBaseVertices()[i].Y;
                }
                if (max.Z < constMesh->getBaseVertices()[i].Z) {
                    max.Z = constMesh->getBaseVertices()[i].Z;
                }
            }
        }
        originalBBox = new AABBox<float>(min, max);
        originalSplitBBoxes = SplitBoundingBox().split(*originalBBox);
    }

    ConstMeshes::~ConstMeshes() {
        for (auto& constMesh : constMeshes) {
            delete constMesh;
        }

        delete originalBBox;
    }

    const std::string& ConstMeshes::getMeshFilename() const {
        return meshFilename;
    }

    unsigned int ConstMeshes::getNumberConstMeshes() const {
        return (unsigned int)constMeshes.size();
    }

    const ConstMesh* ConstMeshes::getConstMesh(unsigned int index) const {
        return constMeshes[index];
    }

    const std::vector<const ConstMesh*>& ConstMeshes::getConstMeshes() const {
        return constMeshes;
    }

    const AABBox<float>& ConstMeshes::getOriginalAABBox() const {
        return *originalBBox;
    }

    const std::vector<AABBox<float>>& ConstMeshes::getOriginalSplitAABBoxes() const {
        return originalSplitBBoxes;
    }

}
