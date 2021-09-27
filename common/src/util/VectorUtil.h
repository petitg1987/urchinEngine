#pragma once

#include <vector>
#include <algorithm>

namespace urchin {

    class VectorUtil {
        public:
            template<class T> static void erase(std::vector<T>&, std::size_t);
            template<class T> static void erase(std::vector<T>&, typename std::vector<T>::iterator);

            template<class T> static void removeDuplicates(std::vector<T>&);

            template<class T, class Compare> static void insertSorted(std::vector<T>&, T&, Compare);

            template<class T> static std::vector<T> merge(const std::vector<T>&, const std::vector<T>&);
    };

    #include "VectorUtil.inl"

}
