#include <scene/renderer3d/model/Meshes.h>
#include <resources/model/boundingbox/SplitBoundingBox.h>

namespace urchin {

    Meshes::Meshes(std::shared_ptr<ConstMeshes> constMeshes) :
            constMeshes(std::move(constMeshes)),
            numMeshes(this->constMeshes->getNumberConstMeshes()) {
        for (unsigned int meshIndex = 0;  meshIndex < numMeshes; ++meshIndex) {
            meshes.push_back(std::make_unique<Mesh>(this->constMeshes->getConstMesh(meshIndex)));
        }
        computeLocalAABBox(false);
    }

    unsigned int Meshes::getNumberMeshes() const {
        return numMeshes;
    }

    Mesh& Meshes::getMesh(unsigned int meshIndex) const {
        return *meshes[meshIndex];
    }

    const AABBox<float>& Meshes::getGlobalAABBox() const {
        return globalBBox;
    }

    const std::vector<AABBox<float>>& Meshes::getGlobalSplitAABBoxes() const {
        return globalSplitBBoxes;
    }

    const AABBox<float>& Meshes::getGlobalLocalAABBox() const {
        return *localBBox;
    }

    const ConstMeshes& Meshes::getConstMeshes() const {
        return *constMeshes;
    }

    std::shared_ptr<ConstMeshes> Meshes::copyConstMeshesRef() const {
        return constMeshes;
    }

    void Meshes::updateMesh(unsigned int meshIndex, const std::vector<Point3<float>>& vertices) {
        meshes[meshIndex]->updateVertices(vertices);
        computeLocalAABBox(true);
    }

    void Meshes::onMoving(const Transform<float>& newTransform) {
        globalBBox = localBBox->moveAABBox(newTransform);

        globalSplitBBoxes.clear();
        for (const auto& localSplitAABBox : localSplitBBoxes) {
            globalSplitBBoxes.push_back(localSplitAABBox.moveAABBox(newTransform));
        }
    }

    void Meshes::computeLocalAABBox(bool useMeshVertices) {
        Point3<float> min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point3<float> max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (unsigned int meshIndex = 0; meshIndex < getNumberMeshes(); ++meshIndex) {
            const auto& vertices = useMeshVertices ? getMesh(meshIndex).getVertices() : getConstMeshes().getConstMesh(meshIndex).getBaseVertices();
            for (auto& vertex : vertices) {
                if (min.X > vertex.X) {
                    min.X = vertex.X;
                }
                if (min.Y > vertex.Y) {
                    min.Y = vertex.Y;
                }
                if (min.Z > vertex.Z) {
                    min.Z = vertex.Z;
                }

                if (max.X < vertex.X) {
                    max.X = vertex.X;
                }
                if (max.Y < vertex.Y) {
                    max.Y = vertex.Y;
                }
                if (max.Z < vertex.Z) {
                    max.Z = vertex.Z;
                }
            }
        }

        for (std::size_t i = 0; i < 3; ++i) {
            float diff = max[i] - min[i];
            if (diff < MIN_BBOX_SIZE) {
                float halfMove = (MIN_BBOX_SIZE - diff) / 2.0f;
                min[i] -= halfMove;
                max[i] += halfMove;
            }
        }

        localBBox = std::make_unique<AABBox<float>>(min, max);
        localSplitBBoxes = SplitBoundingBox().split(*localBBox);
    }

}
