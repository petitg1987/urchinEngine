template<template<class, class, class...> class C, typename K, typename V, typename... Args> V MapUtil::getWithDefault(const C<K, V, Args...>& map, K const& key, const V& defaultValue) {
    typename C<K, V, Args...>::const_iterator it = map.find(key);
    if (it == map.end())
        return defaultValue;
    return it->second;
}