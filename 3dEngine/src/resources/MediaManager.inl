template<class T> T* MediaManager::getMedia(const std::string &filename) {
    //resource already charged ?
    T *resource = ResourceManager::instance()->getResource<T>(filename);
    if(resource) {
        return resource;
    }

    //resource not already charged
    std::string extension = filename.substr(filename.find_last_of('.')+1);

    std::map<std::string, LoaderInterface*>::const_iterator it = loadersRegistry.find(extension);
    if(it==loadersRegistry.end()) {
        throw std::runtime_error("There isn't loader for this type of file, filename: " + filename + ".");
    }

    Loader<T> *loader = static_cast<Loader<T>*>(it->second);
    resource = loader->loadFromFile(filename);

    ResourceManager::instance()->addResource(filename, resource);
    return resource;
}
