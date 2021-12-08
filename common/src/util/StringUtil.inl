template<class T> std::string StringUtil::toString(const T& printableObject) {
    std::stringstream ss;
    ss << printableObject;
    return ss.str();
}
