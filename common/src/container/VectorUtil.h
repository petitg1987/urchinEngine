#pragma once

#include <vector>
#include <algorithm>

namespace urchin {

    class VectorUtil {
        public:
            template<class T> static void erase(std::vector<T>&, std::size_t);
            template<class T> static void erase(std::vector<T>&, typename std::vector<T>::iterator);

            template<class T> static void removeDuplicates(std::vector<T>&);

            template<class T, class... A> static std::vector<T> concatenate(const std::vector<T>&, A&&...);
            template<class T> static std::vector<T> concatenate(const std::vector<T>&, T);
            template<class T> static std::vector<T*> concatenate(std::span<T* const>, T*);
    };

    #include "VectorUtil.inl"

}
