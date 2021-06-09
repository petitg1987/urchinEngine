#pragma once

namespace urchin {

    class MapUtil {
        public:
            template<template<class, class, class...> class C, typename K, typename V, typename... Args> static V getWithDefault(const C<K, V, Args...>&, K const&, const V&);
    };

    #include "MapUtil.inl"

}
