#pragma once

#include <optional>
#include <regex>

#include <i18n/LanguageTranslator.h>

namespace urchin {

    class ParameterizedText {
        public:
            static constexpr char TRANSLATABLE_TEXT_PREFIX = '~';

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
            static const std::regex PARAMETER_REGEX;

            std::string text;
            std::vector<std::string> parameters;
    };

    constexpr auto i18n = [](const std::string& text) { return ParameterizedText::TRANSLATABLE_TEXT_PREFIX + text; };

    #include "ParameterizedText.inl"

}
