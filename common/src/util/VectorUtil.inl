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
    std::ranges::sort(v);
    v.erase(std::ranges::begin(std::ranges::unique(v)), v.end());
}

template<class T, class... A> std::vector<T> VectorUtil::concatenate(const std::vector<T>& v1, A&&... vr) {
    std::vector<T> result = v1;
    result.reserve(v1.size() + (vr.size() + ...));

    (result.insert(result.end(), vr.begin(), vr.end()), ...);
    return result;
}
