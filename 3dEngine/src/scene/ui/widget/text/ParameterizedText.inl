template<class ...T> ParameterizedText::ParameterizedText(std::string text, T...parameters) :
        text(std::move(text)) {
    (this->parameters.push_back(std::string(std::forward<T>(parameters))), ...);
    if (this->parameters.empty()) {
        throw std::runtime_error("At least one parameter is expected for a parameterized text");
    }
}
