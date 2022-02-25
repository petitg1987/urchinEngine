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
        assert(meshes.size() > meshIndex);
        return *meshes[meshIndex];
    }

    /**
    * @return Bounding box regrouping all meshes (transformed by the model matrix)
    */
    const AABBox<float>& Meshes::getMeshesAABBox() const {
        return meshesBBox;
    }

    /**
    * @return Split bounding box regrouping all meshes (transformed by the model matrix)
    */
    const std::vector<AABBox<float>>& Meshes::getMeshesSplitAABBoxes() const {
        return meshesSplitBBoxes;
    }

    /**
    * @return Bounding box regrouping all meshes (not transformed)
    */
    const AABBox<float>& Meshes::getLocalMeshesAABBox() const {
        return *localMeshesBBox;
    }

    const ConstMeshes& Meshes::getConstMeshes() const {
        return *constMeshes;
    }

    std::shared_ptr<ConstMeshes> Meshes::copyConstMeshesRef() const {
        return constMeshes;
    }

    void Meshes::updateMesh(unsigned int meshIndex, const std::vector<Point3<float>>& vertices) {
        getMesh(meshIndex).updateVertices(vertices);
        computeLocalAABBox(true);
    }

    void Meshes::updateMaterial(unsigned int meshIndex, const std::shared_ptr<Material>& material) const {
        getMesh(meshIndex).updateMaterial(material);
    }

    void Meshes::onMoving(const Transform<float>& newTransform) {
        meshesBBox = localMeshesBBox->moveAABBox(newTransform);

        if (meshesSplitBBoxes.size() == 1) {
            meshesSplitBBoxes[0] = meshesBBox;
        } else {
            meshesSplitBBoxes.clear();
            for (const auto &localSplitAABBox: localSplitBBoxes) {
                meshesSplitBBoxes.push_back(localSplitAABBox.moveAABBox(newTransform));
            }
        }
    }

    void Meshes::computeLocalAABBox(bool useMeshVertices) {
        Point3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point3 max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
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

        localMeshesBBox = std::make_unique<AABBox<float>>(min, max);
        SplitBoundingBox().split(*localMeshesBBox, localSplitBBoxes);
    }

}
