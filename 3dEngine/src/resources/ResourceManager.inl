template<class T> std::shared_ptr<T> ResourceManager::getResource(const std::string& resourceId) const {
    auto itFind = resources.find(resourceId);
    if (itFind != resources.end()) {
        return std::dynamic_pointer_cast<T>(itFind->second.lock());
    }

    return nullptr;
}
