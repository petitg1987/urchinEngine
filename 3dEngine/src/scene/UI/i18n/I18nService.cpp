#include <UrchinCommon.h>

#include <scene/UI/i18n/I18nService.h>

#define DEFAULT_LANGUAGE "EN"

namespace urchin {

    I18nService::I18nService() :
            language(DEFAULT_LANGUAGE) {

    }

    void I18nService::changeLanguage(std::string language) {
        this->language = std::move(language);
    }

    const std::string& I18nService::getLanguage() const {
        return language;
    }

    void I18nService::add(TranslatableText* translatableText) {
        translatableTexts.emplace_back(translatableText);
        translatableText->refreshTranslatableText();
    }

    void I18nService::remove(TranslatableText* translatableText) {
        auto itFind = find(translatableTexts.begin(), translatableTexts.end(), translatableText);
        VectorUtil::erase(translatableTexts, itFind);
    }

    std::string I18nService::translate(const std::string& textKey) {
        return textTranslator.translate(language, textKey);
    }

}
