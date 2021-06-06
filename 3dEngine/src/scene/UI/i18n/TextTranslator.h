#pragma once

#include <string>

namespace urchin {

    class TextTranslator {
        public:
            TextTranslator();

            std::string translate(const std::string&, const std::string&);

        private:
            void checkMissingTranslation() const;
            void logMissingTranslation(const std::string&, const std::string&) const;

            void loadLanguageTexts(const std::string&);
            std::map<std::string, std::string> retrieveLanguageTexts(const std::string&) const;

            const std::string MAIN_LANGUAGE;
            const std::string FILE_PREFIX;
            const std::string FILE_POSTFIX;

            std::string textFilesDirectoryName;
            std::string loadedLanguage;
            std::map<std::string, std::string> loadedLanguageTexts;
    };

}
