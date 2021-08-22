#include <UrchinCommon.h>

#include <i18n/I18nService.h>

namespace urchin {

    I18nService::I18nService() :
            DEFAULT_LANGUAGE("en"),
            language(DEFAULT_LANGUAGE),
            labelTranslator(std::make_unique<LabelTranslator>()) {
        labelTranslator->checkMissingTranslation();
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

    void I18nService::add(TranslatableLabel* translatableLabel) {
        translatableLabels.insert(translatableLabel);
        refreshTranslation(translatableLabel);
    }

    void I18nService::remove(TranslatableLabel* translatableLabel) {
        translatableLabels.erase(translatableLabel);
    }

    void I18nService::refreshTranslation(TranslatableLabel* translatableLabel) {
        std::string translatedLabel = labelTranslator->translate(language, translatableLabel->getLabelKey());
        translatableLabel->updateLabel(translatedLabel);
    }

}
