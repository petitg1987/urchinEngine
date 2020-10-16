//static
template<class T> T *Singleton<T>::objectT = nullptr;

template<class T> Singleton<T>::Singleton() {

}

template<class T> Singleton<T>::~Singleton() {
    objectT = nullptr;
}

template<class T> T* Singleton<T>::instance() {
    if (!objectT) {
        objectT = static_cast<T*>(SingletonManager::getSingleton(typeid(T).name()));
        if (!objectT) {
            objectT = new T;
            SingletonManager::addSingleton(typeid(T).name(), objectT);
        }
    }

    return objectT;
}
