#pragma once

#include <string>

namespace urchin {

    class TextTranslator {
        public:
            TextTranslator();

            void checkMissingTranslation() const;

            const std::vector<std::string>& getAvailableLanguages() const;
            std::string translate(const std::string&, const std::string&);

        private:
            void loadAvailableLanguages();

            void loadLanguageTexts(const std::string&);
            std::map<std::string, std::string> retrieveLanguageTexts(const std::string&) const;
            void logMissingLanguage(const std::string&) const;
            void logMissingTranslation(const std::string&, const std::string&) const;

            const std::string FILE_PREFIX;
            const std::string FILE_POSTFIX;

            std::string textFilesDirectoryName;
            std::vector<std::string> availableLanguages;

            std::string loadedLanguage;
            std::map<std::string, std::string> loadedLanguageTexts;
    };

}
