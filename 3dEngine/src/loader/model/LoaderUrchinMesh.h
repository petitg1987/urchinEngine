#ifndef URCHINENGINE_LOADERURCHINMESH_H
#define URCHINENGINE_LOADERURCHINMESH_H

#include <string>

#include "resources/model/ConstMeshes.h"
#include "loader/Loader.h"

namespace urchin {

    class LoaderUrchinMesh : public Loader<ConstMeshes> {
        public:
            ~LoaderUrchinMesh() override = default;

            ConstMeshes* loadFromFile(const std::string&) override;
    };

}

#endif
