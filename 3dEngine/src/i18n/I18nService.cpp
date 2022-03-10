#include <UrchinCommon.h>

#include <i18n/I18nService.h>
#include <i18n/LanguageTranslator.h>

namespace urchin {

    I18nService::I18nService() :
            DEFAULT_LANGUAGE("en"),
            language(DEFAULT_LANGUAGE),
            labelTranslator(LabelTranslator()) {
        labelTranslator.checkMissingTranslation();
    }

    void I18nService::changeLanguage(std::string language) {
        this->language = std::move(language);

        for (auto* translatableLabel : translatableLabels) {
            refreshTranslation(translatableLabel);
        }
    }

    const std::string& I18nService::getLanguage() const {
        return language;
    }

    LanguageTranslator I18nService::getLanguageTranslator() {
        return LanguageTranslator(labelTranslator, language);
    }

    void I18nService::add(TranslatableLabel* translatableLabel) {
        translatableLabels.insert(translatableLabel);
        refreshTranslation(translatableLabel);
    }

    void I18nService::remove(TranslatableLabel* translatableLabel) {
        translatableLabels.erase(translatableLabel);
    }

    void I18nService::refreshTranslation(TranslatableLabel* translatableLabel) {
        translatableLabel->refreshTranslation(getLanguageTranslator());
    }

}
