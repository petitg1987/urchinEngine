#include <string>
#include <vector>

#include <i18n/TranslatableLabel.h>
#include <i18n/LabelTranslator.h>

namespace urchin {

    class Translator {

    };

    class I18nService {
        public:
            I18nService();

            void changeLanguage(std::string);
            const std::string& getLanguage() const;

            void add(TranslatableLabel*);
            void remove(TranslatableLabel*);

        private:
            void refreshTranslation(TranslatableLabel*);

            const std::string DEFAULT_LANGUAGE;

            std::string language;
            std::set<TranslatableLabel*> translatableLabels;
            LabelTranslator labelTranslator;
    };

}
