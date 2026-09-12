#include "i18n/LanguageTranslator.h"

#include <utility>

namespace urchin {

    LanguageTranslator::LanguageTranslator(LabelTranslator& labelTranslator, std::string language) :
            labelTranslator(labelTranslator),
            language(std::move(language)) {

    }

    bool LanguageTranslator::isTranslationExist(std::string_view labelKey) const {
        return labelTranslator.getLanguageLabels(language).contains(labelKey);
    }

    std::string LanguageTranslator::translate(std::string_view labelKey) const {
        return labelTranslator.translate(language, labelKey);
    }

}