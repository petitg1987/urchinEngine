#pragma once

#include "i18n/LabelTranslator.h"

namespace urchin {

    class LanguageTranslator {
        public:
            LanguageTranslator(LabelTranslator&, std::string);

            bool isTranslationExist(std::string_view) const;
            std::string translate(std::string_view) const;

        private:
            LabelTranslator& labelTranslator;
            std::string language;
    };

}