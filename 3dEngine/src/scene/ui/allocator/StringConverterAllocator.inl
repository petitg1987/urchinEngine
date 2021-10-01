template<class T> StringConverterAllocator<T>::StringConverterAllocator() {
    for (auto& memorySlot : memorySlots) {
        auto deleter = [](T* ptr){::operator delete(ptr, ALLOCATED_SIZE * sizeof(T));};
        memorySlot.ptr = std::shared_ptr<T>(reinterpret_cast<T*>(::operator new(ALLOCATED_SIZE * sizeof(T))), deleter);
        memorySlot.used = false;
    }
}

template<class T> StringConverterAllocator<T>::StringConverterAllocator(const StringConverterAllocator<T>& src) {
    memorySlots = src.memorySlots;
}

template<class T> template<class U> StringConverterAllocator<T>::StringConverterAllocator(const StringConverterAllocator<U>& src) {
    static_assert(sizeof(U) == sizeof(T));
    memorySlots = src.memorySlots;
}

template<class T> T* StringConverterAllocator<T>::allocate(std::size_t n) {
    if (n <= ALLOCATED_SIZE) {
        for (auto& memorySlot : memorySlots) {
            if (!memorySlot.used) {
                memorySlot.used = true;
                return memorySlot.ptr.get();
            }
        }
    }
    return reinterpret_cast<T*>(::operator new(n * sizeof(T)));
}

template<class T> void StringConverterAllocator<T>::deallocate(T* p, std::size_t n) {
    for (auto& memorySlot : memorySlots) {
        if (memorySlot.ptr.get() == p) {
            memorySlot.used = false;
            return;
        }
    }
    return ::operator delete(p, n * sizeof(T));
}

template<class T> bool operator== (const StringConverterAllocator<T>& a, const StringConverterAllocator<T>& b) {
    for (std::size_t i = 0; i < a.memorySlots.size(); ++i) {
        if (a.memorySlots[i].ptr != b.memorySlots[i].ptr) {
            return false;
        }
    }
    return true;
}

template<class T> bool operator!= (const StringConverterAllocator<T>& a, const StringConverterAllocator<T>& b) {
    return !(operator==(a, b));
}
