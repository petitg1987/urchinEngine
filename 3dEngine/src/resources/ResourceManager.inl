template<class T> std::shared_ptr<T> ResourceManager::getResource(const std::string& resourceId) const {
    auto itFind = resources.find(resourceId);
    if (itFind != resources.end()) {
        std::shared_ptr<Resource> resource = itFind->second.lock();
        if(resource) {
            return std::dynamic_pointer_cast<T>(resource);
        }
    }

    return std::shared_ptr<T>(nullptr);
}
