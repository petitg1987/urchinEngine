#include <UrchinCommon.h>

#include <scene/UI/i18n/I18nService.h>

namespace urchin {

    I18nService::I18nService() :
            DEFAULT_LANGUAGE("en"),
            language(DEFAULT_LANGUAGE) {

    }

    void I18nService::changeLanguage(std::string language) {
        this->language = std::move(language);

        for(auto translatableText : translatableTexts) {
            refreshTranslation(translatableText);
        }
    }

    const std::string& I18nService::getLanguage() const {
        return language;
    }

    void I18nService::add(TranslatableText* translatableText) {
        translatableTexts.insert(translatableText);
        refreshTranslation(translatableText);
    }

    void I18nService::remove(TranslatableText* translatableText) {
        translatableTexts.erase(translatableText);
    }

    void I18nService::refreshTranslation(TranslatableText* translatableText) {
        std::string translatedText = textTranslator.translate(language, translatableText->getTextKey());
        translatableText->updateText(translatedText);
    }

}
