/**
 * @param result [out] All resources having the requested tag
 */
template<class T> void TagHolder::findByTag(const std::string& tag, std::vector<T>& result) const {
    const auto& [lowerBound, upperBound] = tagsMap.equal_range(tag);
    for (auto it = lowerBound; it != upperBound; ++it) {
        result.push_back(static_cast<T>(it->second));
    }
}
