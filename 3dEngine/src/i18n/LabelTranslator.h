#pragma once

#include <string>

namespace urchin {

    struct LabelStatistics {
        unsigned int labelsCount;
        unsigned int wordsCount;
    };

    class LabelTranslator {
        public:
            static constexpr char NO_LABEL_SURROUND[] = "??";

            LabelTranslator();

            void checkTranslationError() const;

            LabelStatistics computeStatistics(const std::string&) const;

            const std::vector<std::string>& getAvailableLanguages() const;
            const std::map<std::string, std::string, std::less<>>& getLanguageLabels(const std::string&);
            std::string translate(const std::string&, std::string_view);

        private:
            void loadAvailableLanguages();

            void loadLanguageLabels(const std::string&);
            std::map<std::string, std::string, std::less<>> retrieveLanguageLabels(const std::string&) const;
            void logMissingLanguage(const std::string&) const;
            void logMissingTranslation(const std::string&, const std::string&) const;

            bool isFirstLetterUpper(std::string_view) const;

            const std::string FILE_PREFIX;
            const std::string FILE_POSTFIX;

            std::string labelsFilesDirectoryName;
            std::vector<std::string> availableLanguages;

            std::string loadedLanguage;
            std::map<std::string, std::string, std::less<>> loadedLanguageLabels;
    };

}
