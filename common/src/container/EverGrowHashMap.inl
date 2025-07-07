template<typename K, typename V> EverGrowHashMap<K, V>::Iterator::Iterator(EverGrowHashMap& map, std::size_t bucketIndex, std::size_t elementIndex) :
        map(map),
        bucketIndex(bucketIndex),
        elementIndex(elementIndex) {

}

template<typename K, typename V> typename EverGrowHashMap<K, V>::Node& EverGrowHashMap<K, V>::Iterator::operator*() const {
    return map.table[bucketIndex][elementIndex];
}

template<typename K, typename V> typename EverGrowHashMap<K, V>::Node* EverGrowHashMap<K, V>::Iterator::operator->() const {
    return &map.table[bucketIndex][elementIndex];
}

template<typename K, typename V> typename EverGrowHashMap<K, V>::Iterator& EverGrowHashMap<K, V>::Iterator::operator++() {
    ++elementIndex;
    while (bucketIndex < map.table.size() && elementIndex >= map.table[bucketIndex].size()) {
        ++bucketIndex;
        elementIndex = 0;
    }
    return *this;
}

template<typename K, typename V> bool EverGrowHashMap<K, V>::Iterator::operator==(const Iterator& other) const {
    return &map == &other.map && bucketIndex == other.bucketIndex && elementIndex == other.elementIndex;
}

template<typename K, typename V> bool EverGrowHashMap<K, V>::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

template<typename K, typename V> EverGrowHashMap<K, V>::Node::Node(const K& key, const V& value) :
        key(key),
        value(value) {

}

template<typename K, typename V> EverGrowHashMap<K, V>::EverGrowHashMap(std::size_t bucketCount, float loadFactor) :
        numBuckets(bucketCount),
        currentSize(0),
        maxLoadFactor(loadFactor) {
    table.resize(numBuckets);
    for (std::vector<Node>& bucket : table) {
        bucket.reserve(2); //reverse 2 slots by bucket to minimize memory allocation
    }
}

template<typename K, typename V> void EverGrowHashMap<K, V>::insert(const K& key, const V& value) {
    if (static_cast<float>(currentSize) / numBuckets > maxLoadFactor) {
        rehash();
    }

    std::size_t index = hashFunc(key) % numBuckets;

    for (auto& node : table[index]) {
        if (node.key == key) {
            node.value = value;
            return;
        }
    }

    table[index].emplace_back(key, value);
    ++currentSize;
}

template<typename K, typename V> V* EverGrowHashMap<K, V>::find(const K& key) {
    std::size_t index = hashFunc(key) % numBuckets;

    for (auto& node : table[index]) {
        if (node.key == key) {
            return &(node.value);
        }
    }

    return nullptr;
}

template<typename K, typename V> V EverGrowHashMap<K, V>::at(const K& key) const {
    std::size_t index = hashFunc(key) % numBuckets;

    for (const auto& node : table[index]) {
        if (node.key == key) {
            return node.value;
        }
    }

    throw std::out_of_range("Key not found in EverGrowHashMap");
}

template<typename K, typename V> bool EverGrowHashMap<K, V>::erase(const K& key) {
    std::size_t index = hashFunc(key) % numBuckets;

    for (auto it = table[index].begin(); it != table[index].end(); ++it) {
        if (it->key == key) {
            table[index].erase(it);
            --currentSize;
            return true;
        }
    }

    return false;
}

template<typename K, typename V> void EverGrowHashMap<K, V>::clear() {
    for (auto& bucket : table) {
        bucket.clear();
    }
    currentSize = 0;
}

template<typename K, typename V> typename EverGrowHashMap<K, V>::Iterator EverGrowHashMap<K, V>::begin() {
    std::size_t bucketIndex = 0;
    while (bucketIndex < table.size() && table[bucketIndex].empty()) {
        ++bucketIndex;
    }
    return Iterator(*this, bucketIndex, 0);
}

template<typename K, typename V> typename EverGrowHashMap<K, V>::Iterator EverGrowHashMap<K, V>::end() {
    return Iterator(*this, table.size(), 0);
}

template<typename K, typename V> bool EverGrowHashMap<K, V>::isEmpty() const {
    return currentSize == 0;
}

template<typename K, typename V> std::size_t EverGrowHashMap<K, V>::getSize() const {
    return currentSize;
}

template<typename K, typename V> std::size_t EverGrowHashMap<K, V>::getNumBuckets() const {
    return numBuckets;
}

template<typename K, typename V> void EverGrowHashMap<K, V>::rehash() {
    std::size_t newBucketCount = numBuckets * 2;
    std::vector<std::vector<Node>> newTable(newBucketCount);
    for (std::vector<Node>& bucket : newTable) {
        bucket.reserve(2); //reverse 2 slots by bucket to minimize memory allocation
    }

    for (std::vector<Node>& bucket : table) {
        for (Node& node : bucket) {
            std::size_t newIndex = hashFunc(node.key) % newBucketCount;
            newTable[newIndex].emplace_back(node.key, node.value);
        }
    }

    table = std::move(newTable);
    numBuckets = newBucketCount;
}
