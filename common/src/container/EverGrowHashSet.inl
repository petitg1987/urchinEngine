template<typename K> EverGrowHashSet<K>::Iterator::Iterator(EverGrowHashSet& set, std::size_t bucketIndex, std::size_t elementIndex) :
        set(set),
        bucketIndex(bucketIndex),
        elementIndex(elementIndex) {

}

template<typename K> K& EverGrowHashSet<K>::Iterator::operator*() const {
    return set.table[bucketIndex][elementIndex];
}

template<typename K> K* EverGrowHashSet<K>::Iterator::operator->() const {
    return &set.table[bucketIndex][elementIndex];
}

template<typename K> EverGrowHashSet<K>::Iterator& EverGrowHashSet<K>::Iterator::operator++() {
    ++elementIndex;
    while (bucketIndex < set.table.size() && elementIndex >= set.table[bucketIndex].size()) {
        ++bucketIndex;
        elementIndex = 0;
    }
    return *this;
}

template<typename K> bool EverGrowHashSet<K>::Iterator::operator==(const Iterator& other) const {
    return &set == &other.set && bucketIndex == other.bucketIndex && elementIndex == other.elementIndex;
}

template<typename K> bool EverGrowHashSet<K>::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

template<typename K> EverGrowHashSet<K>::EverGrowHashSet(std::size_t bucketCount, float loadFactor) :
        numBuckets(bucketCount),
        currentSize(0),
        maxLoadFactor(loadFactor) {
    table.resize(numBuckets);
    for (std::vector<K>& bucket : table) {
        bucket.reserve(2); //reverse 2 slots by bucket to minimize memory allocation
    }
}

template<typename K> bool EverGrowHashSet<K>::insert(const K& key) {
    if (static_cast<float>(currentSize) / numBuckets > maxLoadFactor) {
        rehash();
    }

    std::size_t index = hashFunc(key) % numBuckets;

    for (auto& savedKey : table[index]) {
        if (savedKey == key) {
            return false;
        }
    }

    table[index].push_back(key);
    ++currentSize;
    return true;
}

template<typename K> bool EverGrowHashSet<K>::isExist(const K& key) {
    std::size_t index = hashFunc(key) % numBuckets;

    for (auto& savedKey : table[index]) {
        if (savedKey == key) {
            return true;
        }
    }

    return false;

}

template<typename K> bool EverGrowHashSet<K>::erase(const K& key) {
    std::size_t index = hashFunc(key) % numBuckets;

    for (auto it = table[index].begin(); it != table[index].end(); ++it) {
        if (*it == key) {
            table[index].erase(it);
            --currentSize;
            return true;
        }
    }

    return false;
}

template<typename K> void EverGrowHashSet<K>::clear() {
    for (auto& bucket : table) {
        bucket.clear();
    }
    currentSize = 0;
}

template<typename K> EverGrowHashSet<K>::Iterator EverGrowHashSet<K>::begin() {
    std::size_t bucketIndex = 0;
    while (bucketIndex < table.size() && table[bucketIndex].empty()) {
        ++bucketIndex;
    }
    return Iterator(*this, bucketIndex, 0);
}

template<typename K> EverGrowHashSet<K>::Iterator EverGrowHashSet<K>::end() {
    return Iterator(*this, table.size(), 0);
}

template<typename K> bool EverGrowHashSet<K>::isEmpty() const {
    return currentSize == 0;
}

template<typename K> std::size_t EverGrowHashSet<K>::getSize() const {
    return currentSize;
}

template<typename K> std::size_t EverGrowHashSet<K>::getNumBuckets() const {
    return numBuckets;
}

template<typename K> void EverGrowHashSet<K>::rehash() {
    std::size_t newBucketCount = numBuckets * 2;
    std::vector<std::vector<K>> newTable(newBucketCount);
    for (std::vector<K>& bucket : newTable) {
        bucket.reserve(2); //reverse 2 slots by bucket to minimize memory allocation
    }

    for (std::vector<K>& bucket : table) {
        for (K& savedKey : bucket) {
            std::size_t newIndex = hashFunc(savedKey) % newBucketCount;
            newTable[newIndex].push_back(savedKey);
        }
    }

    table = std::move(newTable);
    numBuckets = newBucketCount;
}
