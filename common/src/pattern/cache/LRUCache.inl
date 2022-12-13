template<class K, class V> LRUCache<K, V>::LRUCache(std::size_t maxSize) :
        maxSize(maxSize) {

}

template<class K, class V> bool LRUCache<K, V>::put(const K& key, const V& value) {
    if (cacheMap.contains(key)) {
        return false;
    }

    if (cacheList.size() == maxSize) {
        cacheMap.erase(cacheList.back().first);
        cacheList.pop_back();
    }

    cacheList.emplace_front(key, value);
    cacheMap.try_emplace(key, cacheList.begin());
    return true;
}

template<class K, class V> std::optional<V> LRUCache<K, V>::get(const K& key) {
    auto itFind = cacheMap.find(key);
    if (itFind == cacheMap.end()) {
        return std::nullopt;
    }

    cacheList.splice(cacheList.begin(), cacheList, itFind->second);
    return itFind->second->second;
}

template<class K, class V> void LRUCache<K, V>::erase(const K& key) {
    auto itFind = cacheMap.find(key);
    if (itFind == cacheMap.end()) {
        return;
    }

    cacheList.erase(itFind->second);
    cacheMap.erase(itFind);
}

template<class K, class V> std::size_t LRUCache<K, V>::getCacheSize() const {
    return cacheList.size();
}
