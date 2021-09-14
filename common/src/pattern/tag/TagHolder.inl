/**
 * @param result [out] All resources having the requested tag
 */
template<class T> void TagHolder::findByTag(const std::string& tag, std::vector<T>& result) const {
    auto itTaggable = tagsMap.equal_range(tag);
    for (auto it = itTaggable.first; it != itTaggable.second; ++it) {
        result.push_back(static_cast<T>(it->second));
    }
}
