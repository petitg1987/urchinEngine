/**
 * Similar to std::vector.erase() but doesn't keep vector order and is more faster.
 */
template<class T> void VectorEraser::erase(std::vector<T>& v, std::size_t eraseIndex) {
    std::swap(v[eraseIndex], v.back());
    v.pop_back();
}

/**
 * Similar to std::vector.erase() but doesn't keep vector order and is more faster.
 */
template<class T> void VectorEraser::erase(std::vector<T>& v, typename std::vector<T>::iterator it) {
    std::swap(*it, v.back());
    v.pop_back();
}
