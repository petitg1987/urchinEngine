/**
 * @param result [out] All resources having the requested tag
 */
template<class T> void TagHolder::findByTag(std::string_view tag, std::vector<T>& result) const {
    const auto& [lowerBound, upperBound] = tagsMap.equal_range(tag);
    for (auto it = lowerBound; it != upperBound; ++it) {
        result.push_back(static_cast<T>(it->second));
    }
}

template<class T> T* TagHolder::findFirstByTag(std::string_view tag) const {
    const auto& it = tagsMap.find(tag);
    if (it != tagsMap.end()) {
        return dynamic_cast<T*>(it->second);
    }
    return nullptr;
}
