#pragma once

#include <unordered_map>
#include <list>
#include <optional>

namespace urchin {

    template<class K, class V> class LRUCache {
        public:
            explicit LRUCache(std::size_t);

            bool put(const K&, const V&);
            std::optional<V> get(const K&);
            void erase(const K&);
            std::size_t getCacheSize() const;

        private:
            std::size_t maxSize;

            std::list<std::pair<K, V>> cacheList;
            std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> cacheMap;
    };

    #include "LRUCache.inl"

}