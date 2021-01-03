template<class T> T* ResourceManager::getResource(const std::string& resourceId) const {
    auto it = mResources.find(resourceId);
    if (it != mResources.end()) {
        it->second->addRef();
        return static_cast<T*>(it->second);
    }

    return nullptr;
}
