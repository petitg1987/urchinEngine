//static
template<class T> T* Singleton<T>::objectT = nullptr;

template<class T> Singleton<T>::~Singleton() {
    objectT = nullptr;
}

template<class T> T* Singleton<T>::instance() {
    if (!objectT) {
        objectT = new T;
        SingletonManager::addSingleton(typeid(T).name(), objectT);
    }

    #ifndef NDEBUG
        static std::map<std::string, std::thread::id> threadUsageMap;
        auto itFind = threadUsageMap.find(typeid(T).name());
        if (itFind != threadUsageMap.end() && std::this_thread::get_id() != itFind->second) {
            throw std::runtime_error("Singleton instance method called from different threads for " + std::string(typeid(T).name()) + ". Consider to use ThreadSafeSingleton instead.");
        } else if (itFind == threadUsageMap.end()) {
            threadUsageMap.insert(std::pair<std::string, std::thread::id>(typeid(T).name(), std::this_thread::get_id()));
        }
    #endif

    return objectT;
}
