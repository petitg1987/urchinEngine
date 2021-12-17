#include <i18n/LanguageTranslator.h>

#include <utility>

namespace urchin {

    LanguageTranslator::LanguageTranslator(LabelTranslator& labelTranslator, std::string language) :
            labelTranslator(labelTranslator),
            language(std::move(language)) {

    }

    std::string LanguageTranslator::translate(const std::string& labelKey) const {
        return labelTranslator.translate(language, labelKey);
    }

}