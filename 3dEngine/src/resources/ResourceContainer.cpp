#include <UrchinCommon.h>

#include <resources/ResourceContainer.h>

namespace urchin {

    ResourceContainer::ResourceContainer() :
            ThreadSafeSingleton<ResourceContainer>() {

    }

    ResourceContainer::~ResourceContainer() {
        cleanResources();
        for (auto& resource : resources) {
            Logger::instance().logError("Resources not released: " + resource.second->getName() + ". Usage count: " + std::to_string(resource.second.use_count()));
        }
    }

    void ResourceContainer::addResource(const std::shared_ptr<Resource>& resource) {
        std::lock_guard<std::mutex> lock(mutex);
        #ifndef NDEBUG
            assert(resources.find(resource->getId()) == resources.end());
        #endif
        resources.emplace(resource->getId(), resource);
    }

    void ResourceContainer::cleanResources() {
        std::lock_guard<std::mutex> lock(mutex);

        bool resourcesDestroyed;
        do {
            resourcesDestroyed = false;
            for (auto it = resources.begin(); it != resources.end();) {
                if (it->second.use_count() <= 1) { //resource not used anymore
                    it = resources.erase(it);
                    resourcesDestroyed = true;
                } else {
                    ++it;
                }
            }
        } while(resourcesDestroyed);
    }

}
