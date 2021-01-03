template<class T> T* MediaManager::getMedia(const std::string& filename, const std::map<std::string, std::string>& params) {
    //resource already charged ?
    std::string resourceId = filename + "_" + MapSerializer::serialize(params);
    T* resource = ResourceManager::instance()->getResource<T>(resourceId);
    if (resource) {
        return resource;
    }

    //resource not already charged
    std::string extension = filename.substr(filename.find_last_of('.') + 1);

    auto it = loadersRegistry.find(extension);
    if (it == loadersRegistry.end()) {
        throw std::runtime_error("There isn't loader for this type of file, filename: " + filename + ".");
    }

    Loader<T>* loader = static_cast<Loader<T>*>(it->second);
    resource = loader->loadFromFile(filename, params);

    ResourceManager::instance()->addResource(resourceId, filename, resource);
    return resource;
}
