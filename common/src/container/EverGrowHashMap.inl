template<typename K, typename V> EverGrowHashMap<K, V>::Node::Node(const K& k, const V& v) :
        key(k),
        value(v) {

}

template<typename K, typename V> EverGrowHashMap<K, V>::EverGrowHashMap(std::size_t bucketCount, float loadFactor) :
        numBuckets(bucketCount),
        currentSize(0),
        maxLoadFactor(loadFactor) {
    table.resize(numBuckets);
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

    table[index].emplace_back(key, value); //TODO can create alloc !
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

    throw std::out_of_range("Key not found");
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
    std::vector<std::list<Node>> newTable(newBucketCount);

    for (auto& bucket : table) {
        for (auto& node : bucket) {
            std::size_t newIndex = hashFunc(node.key) % newBucketCount;
            newTable[newIndex].emplace_back(node.key, node.value);
        }
    }

    table = std::move(newTable);
    numBuckets = newBucketCount;
}
