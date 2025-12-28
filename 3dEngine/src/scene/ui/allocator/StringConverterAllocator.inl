template<class T> StringConverterAllocator<T>::StringConverterAllocator() :
        usageCount(new int(1)) {
    for (auto& memorySlot : memorySlots) {
        memorySlot.ptr = static_cast<T*>(operator new(ALLOCATED_SIZE * sizeof(T)));
        memorySlot.used = false;
    }
}

template<class T> StringConverterAllocator<T>::StringConverterAllocator(const StringConverterAllocator<T>& src) {
    memorySlots = src.memorySlots;
    usageCount = src.usageCount;
    (*usageCount)++;
}

template<class T> StringConverterAllocator<T>::~StringConverterAllocator() {
    if (--(*usageCount) == 0) {
        for (auto& memorySlot : memorySlots) {
            ::operator delete(memorySlot.ptr);
        }
        delete usageCount;
    }
}

template<class T> template<class U> StringConverterAllocator<T>::StringConverterAllocator(const StringConverterAllocator<U>& src) {
    static_assert(sizeof(U) == sizeof(T));
    memorySlots = src.memorySlots;
    usageCount = src.usageCount;
    (*usageCount)++;
}

template<class T> T* StringConverterAllocator<T>::allocate(std::size_t n) {
    if (n <= ALLOCATED_SIZE) {
        for (auto& memorySlot : memorySlots) {
            if (!memorySlot.used) {
                memorySlot.used = true;
                return memorySlot.ptr;
            }
        }
    }
    return static_cast<T*>(operator new(n * sizeof(T)));
}

template<class T> void StringConverterAllocator<T>::deallocate(T* p, std::size_t) {
    for (auto& memorySlot : memorySlots) {
        if (memorySlot.ptr == p) {
            memorySlot.used = false;
            return;
        }
    }
    return ::operator delete(p);
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
