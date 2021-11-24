#include <resources/ResourceRetriever.h>
#include <loader/image/LoaderTGA.h>
#include <loader/image/LoaderPNG.h>
#include <loader/model/LoaderUrchinMesh.h>
#include <loader/model/LoaderUrchinAnim.h>
#include <loader/material/LoaderMaterial.h>
#include <loader/font/LoaderTTF.h>

namespace urchin {

    ResourceRetriever::ResourceRetriever() {
        loadersRegistry.try_emplace("tga", std::make_unique<LoaderTGA>());
        loadersRegistry.try_emplace("png", std::make_unique<LoaderPNG>());

        loadersRegistry.try_emplace(typeid(ConstMeshes).name(), std::make_unique<LoaderUrchinMesh>());
        loadersRegistry.try_emplace(typeid(ConstAnimation).name(), std::make_unique<LoaderUrchinAnim>());

        loadersRegistry.try_emplace(typeid(Material).name(), std::make_unique<LoaderMaterial>());

        loadersRegistry.try_emplace(typeid(Font).name(), std::make_unique<LoaderTTF>());
    }

}
