#pragma once

#include <string>

#include <resources/model/ConstMeshes.h>
#include <loader/Loader.h>

namespace urchin {

    class LoaderUrchinMesh final : public Loader<ConstMeshes> {
        public:
            ~LoaderUrchinMesh() override = default;

            std::shared_ptr<ConstMeshes> loadFromFile(const std::string&, const std::map<std::string, std::string, std::less<>>&) override;
    };

}
