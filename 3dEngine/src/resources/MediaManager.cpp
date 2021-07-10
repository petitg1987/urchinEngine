#include <resources/MediaManager.h>
#include <loader/image/LoaderTGA.h>
#include <loader/image/LoaderPNG.h>
#include <loader/model/LoaderUrchinMesh.h>
#include <loader/model/LoaderUrchinAnim.h>
#include <loader/material/LoaderMaterial.h>
#include <loader/font/LoaderTTF.h>

namespace urchin {

    MediaManager::MediaManager() {
        loadersRegistry.emplace("tga", std::make_unique<LoaderTGA>());
        loadersRegistry.emplace("tga", std::make_unique<LoaderTGA>());
        loadersRegistry.emplace("png", std::make_unique<LoaderPNG>());

        loadersRegistry.emplace("urchinMesh", std::make_unique<LoaderUrchinMesh>());
        loadersRegistry.emplace("urchinAnim", std::make_unique<LoaderUrchinAnim>());

        loadersRegistry.emplace("material", std::make_unique<LoaderMaterial>());

        loadersRegistry.emplace("ttf", std::make_unique<LoaderTTF>());
    }

}
