#include <string>
#include <vector>

#include <scene/UI/i18n/TranslatableText.h>
#include <scene/UI/i18n/TextTranslator.h>

namespace urchin {

    class I18nService {
        public:
            I18nService();

            void changeLanguage(std::string);
            const std::string& getLanguage() const;

            void add(TranslatableText*);
            void remove(TranslatableText*);

            std::string translate(const std::string&);

        private:
            std::string language;
            std::set<TranslatableText*> translatableTexts;
            TextTranslator textTranslator;
    };

}
