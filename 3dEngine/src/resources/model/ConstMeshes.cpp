#include <utility>

#include "resources/model/ConstMeshes.h"

namespace urchin {

    ConstMeshes::ConstMeshes(std::string meshesName, std::optional<std::string> meshesFilename, std::vector<std::unique_ptr<const ConstMesh>> constMeshes) :
            meshesName(std::move(meshesName)),
            meshesFilename(std::move(meshesFilename)),
            constMeshes(std::move(constMeshes)) {

    }

    std::unique_ptr<ConstMeshes> ConstMeshes::fromMeshesFile(const std::string& meshesFilename, std::vector<std::unique_ptr<const ConstMesh>> constMeshesVector) {
        std::string meshesName = FileUtil::getFileName(meshesFilename);
        return std::unique_ptr<ConstMeshes>(new ConstMeshes(meshesName, std::make_optional(meshesFilename), std::move(constMeshesVector)));
    }

    std::unique_ptr<ConstMeshes> ConstMeshes::fromMemory(const std::string& meshesName, std::vector<std::unique_ptr<const ConstMesh>> constMeshesVector) {
        auto constMeshes = std::unique_ptr<ConstMeshes>(new ConstMeshes(meshesName, std::nullopt, std::move(constMeshesVector)));
        constMeshes->setName(meshesName);
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
