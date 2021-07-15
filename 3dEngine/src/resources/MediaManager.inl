/**
 * @param filename Resource filename
 * @param params (optional) Parameter to load the resource
 * @param fileContentType (optional) Define the content of the file when extension is not sufficient. Example: a .json file define the format of the file but not its content.
 */
template<class T> std::shared_ptr<T> MediaManager::getMedia(const std::string& filename, const std::map<std::string, std::string>& params, const std::string& fileContentType) {
    //resource already charged ?
    std::string resourceId = filename + "_" + MapSerializer::serialize(params);
    std::shared_ptr<T> resource = ResourceManager::instance().getResource<T>(resourceId);
    if (resource) {
        return resource;
    }

    //resource not already charged
    std::string fileExtension = filename.substr(filename.find_last_of('.') + 1);
    std::string loaderType = fileContentType.empty() ? fileExtension : fileContentType;

    auto it = loadersRegistry.find(loaderType);
    if (it == loadersRegistry.end()) {
        throw std::runtime_error("There isn't loader for this type of file. Filename: " + filename + ", file content type: " + fileContentType);
    }

    auto loader = static_cast<Loader<T>*>(it->second.get());
    resource = loader->loadFromFile(filename, params);
    resource->setId(resourceId);
    resource->setName(filename);

    ResourceManager::instance().addResource(resource);
    return resource;
}
