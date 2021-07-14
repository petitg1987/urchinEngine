#include <UrchinCommon.h>

#include <resources/ResourceManager.h>

namespace urchin {

    ResourceManager::ResourceManager() :
            Singleton<ResourceManager>() {

    }

    ResourceManager::~ResourceManager() {
        cleanResources();
        for (auto& resource : resources) {
            Logger::instance()->logError("Resources not released: " + resource.second.lock()->getName());
        }
    }

    void ResourceManager::addResource(const std::shared_ptr<Resource>& resource) {
        cleanResources();
        resources.emplace(resource->getId(), resource);
    }

    void ResourceManager::cleanResources() {
        for(auto it = resources.begin(); it != resources.end();) {
            if(it->second.expired()) {
                it = resources.erase(it);
            } else {
                ++it;
            }
        }
    }

}
