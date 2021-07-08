//static
template<class T> T* Singleton<T>::objectT = nullptr;

template<class T> Singleton<T>::~Singleton() {
    objectT = nullptr;
}

template<class T> T* Singleton<T>::instance() {
    if (!objectT) {
        objectT = new T;
        SingletonManager::registerSingleton(typeid(T).name(), objectT);
    }

    return objectT;
}
