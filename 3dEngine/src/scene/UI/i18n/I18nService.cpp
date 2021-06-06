#include <UrchinCommon.h>

#include <scene/UI/i18n/I18nService.h>

namespace urchin {

    I18nService::I18nService() :
            language("en") {

    }

    void I18nService::changeLanguage(std::string language) {
        this->language = std::move(language);

        for(auto translatableText : translatableTexts) {
            translatableText->refreshTranslatableText();
        }
    }

    const std::string& I18nService::getLanguage() const {
        return language;
    }

    void I18nService::add(TranslatableText* translatableText) {
        translatableTexts.insert(translatableText);
        translatableText->refreshTranslatableText();
    }

    void I18nService::remove(TranslatableText* translatableText) {
        translatableTexts.erase(translatableText);
    }

    std::string I18nService::translate(const std::string& textKey) {
        return textTranslator.translate(language, textKey);
    }

}
