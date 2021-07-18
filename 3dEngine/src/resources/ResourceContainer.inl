template<class T> std::shared_ptr<T> ResourceContainer::getResource(const std::string& resourceId) const {
    auto itFind = resources.find(resourceId);
    if (itFind != resources.end() && !itFind->second.expired()) {
        std::shared_ptr<Resource> resource = itFind->second.lock();
        return std::dynamic_pointer_cast<T>(resource);
    }

    return std::shared_ptr<T>(nullptr);
}
