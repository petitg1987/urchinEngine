#pragma once

#include <string>

#include <i18n/LanguageTranslator.h>

namespace urchin {

    class TranslatableLabel {
        public:
            virtual ~TranslatableLabel() = default;

            virtual void refreshTranslation(const LanguageTranslator&&) = 0;
    };

}
