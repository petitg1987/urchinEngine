/**
 * @param filename Resource filename
 * @param params Parameters required to load the resource
 * @param keepForever Indicates if resource must be keep in memory forever. This parameter can be useful when a resource is loaded / unloaded constantly over different frames.
 */
template<class T> std::shared_ptr<T> ResourceRetriever::getResource(const std::string& filename, const std::map<std::string, std::string, std::less<>>& params, bool keepForever) {
    //compute resource path
    std::string resourcePath = FileUtil::isAbsolutePath(filename) ? filename : FileSystem::instance().getResourcesDirectory() + filename;

    //resource already loaded ?
    std::string resourceId = resourcePath + "_" + MapSerializer::serialize(params);
    std::shared_ptr<T> resource = ResourceContainer::instance().getResource<T>(resourceId);
    if (resource) {
        return resource;
    }

    //resource not already loaded
    auto resourceType = std::string(typeid(T).name());
    auto itFind = loadersRegistry.find(resourceType);

    if (itFind == loadersRegistry.end()) {
        itFind = loadersRegistry.find(FileUtil::getFileExtension(resourcePath));

        if (itFind == loadersRegistry.end()) {
            throw std::runtime_error("There is not loader for this type of file. Resource type: " + resourceType + ", resource path: " + resourcePath);
        }
    }

    auto loader = static_cast<Loader<T>*>(itFind->second.get());
    resource = loader->loadFromFile(resourcePath, params);
    resource->setId(resourceId);
    resource->setName(resourcePath);
    resource->setPermanent(keepForever);

    ResourceContainer::instance().addResource(resource);
    return resource;
}
