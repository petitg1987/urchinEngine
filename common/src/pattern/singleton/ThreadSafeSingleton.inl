//static
template<class T> std::atomic<T*> ThreadSafeSingleton<T>::objectT = nullptr;
template<class T> std::mutex ThreadSafeSingleton<T>::mutexInstanceCreation;

template<class T> ThreadSafeSingleton<T>::~ThreadSafeSingleton() {
    objectT.store(nullptr, std::memory_order_release);
}

template<class T> T* ThreadSafeSingleton<T>::instance() {
    T* singletonInstance = objectT.load(std::memory_order_acquire);
    if (!singletonInstance) {
        std::lock_guard<std::mutex> lock(mutexInstanceCreation);
        singletonInstance = objectT.load(std::memory_order_relaxed);
        if (!singletonInstance) {
            singletonInstance = new T;
            SingletonManager::addSingleton(typeid(T).name(), singletonInstance);
            objectT.store(singletonInstance, std::memory_order_release);
        }
    }

    return singletonInstance;
}
