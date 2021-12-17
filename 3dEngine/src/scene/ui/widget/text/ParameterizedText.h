#pragma once

namespace urchin {

    class ParameterizedText {
        public:
            template<class ...T> explicit ParameterizedText(std::string, T... parameters);

            const std::string& getText() const;
            const std::vector<std::string>& getParameters() const;

        private:
            std::string text;
            std::vector<std::string> parameters;
    };

    #include "ParameterizedText.inl"

}
