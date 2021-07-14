/**
 * Similar to std::vector.erase() but doesn't keep vector order and is more faster.
 */
template<class T> void VectorUtil::erase(std::vector<T>& v, std::size_t eraseIndex) {
    std::swap(v[eraseIndex], v.back());
    v.pop_back();
}

/**
 * Similar to std::vector.erase() but doesn't keep vector order and is more faster.
 */
template<class T> void VectorUtil::erase(std::vector<T>& v, typename std::vector<T>::iterator it) {
    std::swap(*it, v.back());
    v.pop_back();
}

template<class T> void VectorUtil::removeDuplicates(std::vector<T>& v) {
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
}

template<class T> std::vector<T> VectorUtil::merge(const std::vector<T>& vectorA, const std::vector<T>& vectorB) {
    std::vector<T> result = vectorA;
    result.insert(result.end(), vectorB.begin(), vectorB.end());
    return result;
}
