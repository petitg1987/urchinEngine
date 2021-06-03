#include <sstream>
#include <UrchinCommon.h>

#include <resources/ResourceManager.h>

namespace urchin {

    ResourceManager::ResourceManager() :
            Singleton<ResourceManager>() {

    }

    ResourceManager::~ResourceManager() {
        if (!mResources.empty()) {
            std::stringstream logStream;
            logStream << "Resources not released:" << std::endl;
            for (auto& mResource : mResources) {
                logStream<< " - " << mResource.second->getName() << std::endl;
            }
            Logger::instance()->logError(logStream.str());
        }
    }

    void ResourceManager::addResource(const std::string& resourceId, const std::string& name, Resource* resource) {
        mResources[resourceId] = resource;
        resource->setId(resourceId);
        resource->setName(name);
    }

    void ResourceManager::removeResource(const std::string& resourceId) {
        auto it = mResources.find(resourceId);
        if (it != mResources.end()) {
            mResources.erase(it);
        }
    }

}
