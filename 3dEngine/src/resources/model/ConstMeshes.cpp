#include <limits>
#include <utility>

#include <resources/model/ConstMeshes.h>
#include <resources/model/boundingbox/SplitBoundingBox.h>

namespace urchin {

    ConstMeshes::ConstMeshes(std::string meshesName, std::optional<std::string> meshesFilename, std::vector<std::unique_ptr<const ConstMesh>> constMeshes) :
            meshesName(std::move(meshesName)),
            meshesFilename(std::move(meshesFilename)),
            constMeshes(std::move(constMeshes)) {
        //determines the bounding box
        Point3<float> min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point3<float> max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for (auto& constMesh : this->constMeshes) {
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

        for (std::size_t i = 0; i < 3; ++i) {
            if (min[i] == max[i]) {
                min[i] = std::nextafter(min[i], -std::numeric_limits<float>::max());
                max[i] = std::nextafter(max[i], std::numeric_limits<float>::max());
            }
        }

        originalBBox = std::make_unique<AABBox<float>>(min, max);
        originalSplitBBoxes = SplitBoundingBox().split(*originalBBox);
    }

    std::unique_ptr<ConstMeshes> ConstMeshes::fromMeshesFile(const std::string& meshFilename, std::vector<std::unique_ptr<const ConstMesh>> constMeshesVector) {
        std::string meshName = FileUtil::getFileName(meshFilename);
        return std::unique_ptr<ConstMeshes>(new ConstMeshes(meshName, std::make_optional(meshFilename), std::move(constMeshesVector)));
    }

    std::unique_ptr<ConstMeshes> ConstMeshes::fromMemory(const std::string& meshName, std::vector<std::unique_ptr<const ConstMesh>> constMeshesVector) {
        auto constMeshes = std::unique_ptr<ConstMeshes>(new ConstMeshes(meshName, std::nullopt, std::move(constMeshesVector)));
        constMeshes->setName(meshName);
        return constMeshes;
    }

    const std::string& ConstMeshes::getMeshesName() const {
        return meshesName;
    }

    const std::string& ConstMeshes::getMeshesFilename() const {
        if (meshesFilename.has_value()) {
            return meshesFilename.value();
        }
        throw std::runtime_error("Meshes " + meshesName + " is not build from a meshes file");
    }

    unsigned int ConstMeshes::getNumberConstMeshes() const {
        return (unsigned int)constMeshes.size();
    }

    const ConstMesh& ConstMeshes::getConstMesh(unsigned int index) const {
        return *constMeshes[index];
    }

    const std::vector<std::unique_ptr<const ConstMesh>>& ConstMeshes::getConstMeshes() const {
        return constMeshes;
    }

    const AABBox<float>& ConstMeshes::getOriginalAABBox() const {
        return *originalBBox;
    }

    const std::vector<AABBox<float>>& ConstMeshes::getOriginalSplitAABBoxes() const {
        return originalSplitBBoxes;
    }

}
