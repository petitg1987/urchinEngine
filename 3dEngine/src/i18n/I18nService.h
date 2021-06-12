#include <string>
#include <vector>

#include <i18n/TranslatableText.h>
#include <i18n/TextTranslator.h>

namespace urchin {

    class I18nService {
        public:
            I18nService();

            void changeLanguage(std::string);
            const std::string& getLanguage() const;

            void add(TranslatableText*);
            void remove(TranslatableText*);

        private:
            void refreshTranslation(TranslatableText*);

            const std::string DEFAULT_LANGUAGE;

            std::string language;
            std::set<TranslatableText*> translatableTexts;
            std::unique_ptr<TextTranslator> textTranslator;
    };

}
