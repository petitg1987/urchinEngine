template<class T> std::string EntityControllerUtil::determineNewCloneName(const std::string& originalEntityName, const std::vector<std::string>& originalGroupHierarchy,
        const std::list<const T*>& entities) {
    std::regex regex("([0-9]+)$");
    std::smatch matches;
    if (std::regex_search(originalEntityName, matches, regex)) {
        std::string endCounterStr = matches[1].str();
        int endCounter = TypeConverter::toInt(endCounterStr);

        int newCounter = endCounter;
        std::string newName;
        do {
            std::string newCounterStr = std::to_string(++newCounter);
            std::size_t numMissingZeros = (std::size_t)std::max(0, (int)endCounterStr.size() - (int)newCounterStr.size());

            newName = originalEntityName.substr(0, originalEntityName.size() - endCounterStr.size()) + std::string(numMissingZeros, '0') + newCounterStr;
        } while (std::ranges::any_of(entities, [&newName, &originalGroupHierarchy](const T* t) {
            return t->getName() == newName && t->getGroupHierarchy() == originalGroupHierarchy;
        }));

        return newName;
    }

    return originalEntityName;
}
