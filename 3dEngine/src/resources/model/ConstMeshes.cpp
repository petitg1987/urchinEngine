#include <utility>

#include "resources/model/ConstMeshes.h"

namespace urchin {

    ConstMeshes::ConstMeshes(std::string meshesName, std::optional<std::string> meshesFilename, std::vector<std::unique_ptr<const ConstMesh>> constMeshes) :
            meshesName(std::move(meshesName)),
            meshesFilename(std::move(meshesFilename)),
            constMeshes(std::move(constMeshes)) {

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

}
