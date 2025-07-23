#pragma once

#include <vector>
#include <string>
#include <optional>

#include "resources/model/ConstMesh.h"
#include "resources/Resource.h"

namespace urchin {

    /**
     * Contains all the constant/common data for meshes.
     * Two identical models can use the instance of this class.
     */
    class ConstMeshes final : public Resource {
        public:
            static std::unique_ptr<ConstMeshes> fromMeshesFile(const std::string&, std::vector<std::unique_ptr<const ConstMesh>>);
            static std::unique_ptr<ConstMeshes> fromMemory(const std::string&, std::vector<std::unique_ptr<const ConstMesh>>);

            const std::string& getMeshesName() const;
            const std::string& getMeshesFilename() const;
            unsigned int getNumberConstMeshes() const;
            const ConstMesh& getConstMesh(unsigned int) const;
            const std::vector<std::unique_ptr<const ConstMesh>>& getConstMeshes() const;

        private:
            ConstMeshes(std::string, std::optional<std::string>, std::vector<std::unique_ptr<const ConstMesh>>);

            std::string meshesName;
            std::optional<std::string> meshesFilename;
            std::vector<std::unique_ptr<const ConstMesh>> constMeshes;
    };

}
