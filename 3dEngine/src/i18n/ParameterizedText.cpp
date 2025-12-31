#include "i18n/ParameterizedText.h"

namespace urchin {

    //static
    const std::regex ParameterizedText::PARAMETER_REGEX = std::regex("\\{[a-zA-Z-]+\\}", std::regex_constants::optimize);

    ParameterizedText::ParameterizedText(std::string text, std::vector<std::string> parameters) :
            text(std::move(text)),
            parameters(std::move(parameters)) {

    }

    void ParameterizedText::setText(std::string text) {
        this->text = std::move(text);
    }

    const std::string& ParameterizedText::getText() const {
        return text;
    }

    void ParameterizedText::setParameters(std::vector<std::string> parameters) {
        this->parameters = std::move(parameters);
    }

    const std::vector<std::string>& ParameterizedText::getParameters() const {
        return parameters;
    }

    bool ParameterizedText::hasTranslatableText() const {
        if (!text.empty() && text[0] == TRANSLATABLE_TEXT_PREFIX) {
            return true;
        }

        if (!parameters.empty()) {
            bool hasTranslatableParameters = std::ranges::any_of(parameters, [](const std::string& parameter) {
                return !parameter.empty() && parameter[0] == TRANSLATABLE_TEXT_PREFIX;
            });
            if (hasTranslatableParameters) {
                return true;
            }
        }

        return false;
    }

    std::string ParameterizedText::translate(const std::optional<LanguageTranslator>& languageTranslator) const {
        std::string evaluatedText;

        if (!text.empty() && text[0] == TRANSLATABLE_TEXT_PREFIX) {
            if (!languageTranslator.has_value()) {
                throw std::runtime_error("Language translator missing for text: " + text);
            }
            evaluatedText = languageTranslator->translate(std::string_view(text).substr(1));
        } else {
            evaluatedText = text;
        }

        for (const std::string& parameter : parameters) {
            std::string paramValue;
            if (!parameter.empty() && parameter[0] == TRANSLATABLE_TEXT_PREFIX) {
                if (!languageTranslator.has_value()) {
                    throw std::runtime_error("Language translator missing for text: " + text);
                }
                paramValue = languageTranslator->translate(std::string_view(parameter).substr(1));
            } else {
                paramValue = parameter;
            }

            evaluatedText = std::regex_replace(evaluatedText, PARAMETER_REGEX, paramValue, std::regex_constants::format_first_only);
        }

        return evaluatedText;
    }

}
