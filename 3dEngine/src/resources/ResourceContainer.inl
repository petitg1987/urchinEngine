template<class T> std::shared_ptr<T> ResourceContainer::getResource(std::string_view resourceId) const {
    std::scoped_lock<std::mutex> lock(mutex);

    auto itFind = resources.find(resourceId);
    if (itFind != resources.end()) {
        return std::dynamic_pointer_cast<T>(itFind->second);
    }
    return std::shared_ptr<T>(nullptr);
}
