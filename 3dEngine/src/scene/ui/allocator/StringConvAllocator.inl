template<class T> StringConvAllocator<T>::StringConvAllocator() {
    for (auto& memorySlot : memorySlots) {
        auto deleter = [](T* ptr){::operator delete(ptr, ALLOCATED_SIZE * sizeof(T));};
        memorySlot.ptr = std::shared_ptr<T>(reinterpret_cast<T*>(::operator new(ALLOCATED_SIZE * sizeof(T))), deleter);
        memorySlot.used = false;
    }
}

template<class T> template<class U> StringConvAllocator<T>::StringConvAllocator(const StringConvAllocator<U>& src) {
    assert(sizeof(U) == sizeof(T));
    for (std::size_t i = 0; i < memorySlots.size(); ++i) {
        memorySlots[i].ptr = src.memorySlots[i].ptr;
        memorySlots[i].used = src.memorySlots[i].used;
    }
}

template<class T> T* StringConvAllocator<T>::allocate(std::size_t n) {
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

template<class T> void StringConvAllocator<T>::deallocate(T* p, std::size_t n) {
    for (auto& memorySlot : memorySlots) {
        if (memorySlot.ptr.get() == p) {
            memorySlot.used = false;
            return;
        }
    }
    return ::operator delete(p, n * sizeof(T));
}
