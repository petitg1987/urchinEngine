#pragma once

#include "i18n/LabelTranslator.h"

namespace urchin {

    class LanguageTranslator {
        public:
            LanguageTranslator(LabelTranslator&, std::string);

            std::string translate(std::string_view) const;

        private:
            LabelTranslator& labelTranslator;
            std::string language;
    };

}