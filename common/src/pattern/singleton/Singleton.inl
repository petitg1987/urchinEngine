//static
template<class T> T* Singleton<T>::objectT = nullptr;

template<class T> Singleton<T>::~Singleton() {
    objectT = nullptr;
}

template<class T> T& Singleton<T>::instance() {
    if (!objectT) {
        auto newObjectT = std::unique_ptr<T>(new T);
        objectT = newObjectT.get();
        SingletonManager::registerSingleton(typeid(T).name(), std::move(newObjectT));
    }

    return *objectT;
}
