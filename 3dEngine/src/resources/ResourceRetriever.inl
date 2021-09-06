/**
 * @param filename Resource filename
 * @param params (optional) Parameter to load the resource
 * @param fileContentType (optional) Define the content of the file when extension is not sufficient. Example: a .json file define the format of the file but not its content.
 */
template<class T> std::shared_ptr<T> ResourceRetriever::getResource(const std::string& filename, const std::map<std::string, std::string>& params) {
    //resource already charged ?
    std::string resourceId = filename + "_" + MapSerializer::serialize(params);
    std::shared_ptr<T> resource = ResourceContainer::instance().getResource<T>(resourceId);
    if (resource) {
        return resource;
    }

    //resource not already charged
    std::string resourceType = std::string(typeid(T).name());
    auto itFind = loadersRegistry.find(resourceType);

    if (itFind == loadersRegistry.end()) {
        std::string fileExtension = filename.substr(filename.find_last_of('.') + 1);
        itFind = loadersRegistry.find(fileExtension);

        if (itFind == loadersRegistry.end()) {
            throw std::runtime_error("There isn't loader for this type of file. Resource type: " + resourceType + ", filename: " + filename);
        }
    }

    auto loader = static_cast<Loader<T>*>(itFind->second.get());
    resource = loader->loadFromFile(filename, params);
    resource->setId(resourceId);
    resource->setName(filename);

    ResourceContainer::instance().addResource(resource);
    return resource;
}
