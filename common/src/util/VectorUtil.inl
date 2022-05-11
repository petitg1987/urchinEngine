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
    result.reserve(v1.size() + (std::size(vr) + ...));

    (result.insert(result.end(), std::begin(vr), std::end(vr)), ...);
    return result;
}

template<class T> std::vector<T> VectorUtil::concatenate(const std::vector<T>& v1, T value) {
    std::vector<T> result = v1;
    result.push_back(value);
    return result;
}

template<class T> std::vector<T*> VectorUtil::concatenate(std::span<T* const> v1, T* value) {
    std::vector<T*> result(v1.begin(), v1.end());
    result.push_back(value);
    return result;
}
