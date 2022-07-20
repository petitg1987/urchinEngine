#pragma once

#include <optional>
#include <regex>

#include <i18n/LanguageTranslator.h>

namespace urchin {

    class ParameterizedText { //TODO move in i18n package ?
        public:
            ParameterizedText() = default;
            ParameterizedText(std::string, std::vector<std::string>);
            template<class ...T> explicit ParameterizedText(std::string, T... parameters);

            void setText(std::string);
            const std::string& getText() const;
            void setParameters(std::vector<std::string>);
            const std::vector<std::string>& getParameters() const;

            bool hasTranslatableText() const;
            std::string translate(const std::optional<LanguageTranslator>&) const;

        private:
            static std::regex parameterRegex;

            std::string text;
            std::vector<std::string> parameters;
    };

    #include "ParameterizedText.inl"

}
