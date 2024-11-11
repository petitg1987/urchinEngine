#include <UrchinCommon.h>

#include <resources/ResourceContainer.h>

namespace urchin {

    ResourceContainer& ResourceContainer::instance() {
        static ResourceContainer instance;
        return instance;
    }

    ResourceContainer::~ResourceContainer() {
        cleanResources(true);
        for (const auto& [resourceId, resource] : resources) {
            Logger::instance().logError("Resources not released: " + resource->getName() + ". Usage count: " + std::to_string(resource.use_count()));
        }
    }

    void ResourceContainer::addResource(const std::shared_ptr<Resource>& resource) {
        std::scoped_lock lock(mutex);
        #ifdef URCHIN_DEBUG
            assert(!resources.contains(resource->getId()));
        #endif
        resources.try_emplace(resource->getId(), resource);
    }

    void ResourceContainer::cleanResources() {
        cleanResources(false);
    }

    void ResourceContainer::cleanResources(bool forceClean) {
        std::scoped_lock lock(mutex);

        bool resourcesDestroyed;
        do {
            resourcesDestroyed = false;
            std::erase_if(resources, [&resourcesDestroyed, &forceClean](const auto& resource) {
                if (resource.second.use_count() <= 1) { //resource not used anymore
                    bool canDestroy = forceClean || !resource.second->isPermanent();
                    if (canDestroy) {
                        resourcesDestroyed = true;
                        return true;
                    }
                }
                return false;
            });
        } while(resourcesDestroyed);
    }

}
