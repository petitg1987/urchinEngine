template<class ...T> ParameterizedText::ParameterizedText(std::string text, T&&...parameters) :
        text(std::move(text)) {
    (this->parameters.push_back(std::string(std::forward<T>(parameters))), ...);
}
