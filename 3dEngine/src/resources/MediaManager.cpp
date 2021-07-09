#include <resources/MediaManager.h>
#include <loader/image/LoaderTGA.h>
#include <loader/image/LoaderPNG.h>
#include <loader/model/LoaderUrchinMesh.h>
#include <loader/model/LoaderUrchinAnim.h>
#include <loader/material/LoaderMaterial.h>
#include <loader/font/LoaderTTF.h>

namespace urchin {

    MediaManager::MediaManager() {
        loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("tga", new LoaderTGA));
        loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("png", new LoaderPNG));

        loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("urchinMesh", new LoaderUrchinMesh));
        loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("urchinAnim", new LoaderUrchinAnim));

        loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("material", new LoaderMaterial));

        loadersRegistry.insert(std::pair<std::string, LoaderInterface*>("ttf", new LoaderTTF));
    }

    MediaManager::~MediaManager() {
        for (const auto& loaderRegistry : loadersRegistry) {
            delete loaderRegistry.second;
        }
    }

}
