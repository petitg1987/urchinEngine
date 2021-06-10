#include <filesystem>
#include <UrchinCommon.h>

#include <scene/UI/i18n/TextTranslator.h>

namespace urchin {

    TextTranslator::TextTranslator() :
            FILE_PREFIX("text_") ,
            FILE_POSTFIX(".properties") {
        std::string uiTextLocation = ConfigService::instance()->getStringValue("ui.textLocation");

        if (!uiTextLocation.empty()) {
            textFilesDirectoryName = FileSystem::instance()->getResourcesDirectory() + ConfigService::instance()->getStringValue("ui.textLocation");
            loadAvailableLanguages();
        }
    }

    void TextTranslator::loadAvailableLanguages() {
        for (const auto& dir : std::filesystem::directory_iterator(textFilesDirectoryName)) {
            std::string filename = dir.path().filename().u8string();
            if (dir.is_regular_file() && filename.size() > FILE_PREFIX.size() + FILE_POSTFIX.size()) {
                std::size_t endLanguagePosition = filename.find(FILE_POSTFIX);
                std::string language = filename.substr(FILE_PREFIX.size(), endLanguagePosition - FILE_PREFIX.size());
                availableLanguages.emplace_back(language);
            }
        }
    }

    void TextTranslator::checkMissingTranslation() const {
        for(unsigned int i = 0; i < availableLanguages.size(); ++i) {
            std::string firstLanguage = availableLanguages[i];
            auto firstLanguageTexts = retrieveLanguageTexts(firstLanguage);

            std::string secondLanguage = availableLanguages[(i + 1) % availableLanguages.size()];
            auto secondLanguageTexts = retrieveLanguageTexts(secondLanguage);

            std::map<std::string, std::string> textsDiff;
            std::set_difference(firstLanguageTexts.begin(), firstLanguageTexts.end(),
                                secondLanguageTexts.begin(), secondLanguageTexts.end(),
                                std::inserter(textsDiff, textsDiff.begin()));

            if (!textsDiff.empty()) {
                logMissingTranslation(secondLanguage, textsDiff.begin()->first);
                break;
            }
        }
    }

    const std::vector<std::string>& TextTranslator::getAvailableLanguages() const {
        return availableLanguages;
    }

    std::string TextTranslator::translate(const std::string& language, const std::string& textKey) {
        loadLanguageTexts(language);

        auto itFind = loadedLanguageTexts.find(textKey);
        if(itFind != loadedLanguageTexts.end()) {
            return itFind->second;
        }

        logMissingTranslation(language, textKey);
        return "??" + textKey + "??";
    }

    void TextTranslator::loadLanguageTexts(const std::string& language) {
        if (language != loadedLanguage) {
            if (std::count(availableLanguages.begin(), availableLanguages.end(), language)) {
                loadedLanguageTexts = retrieveLanguageTexts(language);
            } else {
                loadedLanguageTexts.clear();
                logMissingLanguage(language);
            }
            loadedLanguage = language;
        }
    }

    std::map<std::string, std::string> TextTranslator::retrieveLanguageTexts(const std::string& language) const {
        return PropertyFileHandler(textFilesDirectoryName + FILE_PREFIX + language + FILE_POSTFIX).loadPropertyFile();
    }

    void TextTranslator::logMissingLanguage(const std::string& language) const {
        static bool missingLanguageLogged = false;
        if (!missingLanguageLogged) {
            Logger::instance()->logError("Translated texts for language '" + language + "' not found");
            missingLanguageLogged = true;
        }
    }

    void TextTranslator::logMissingTranslation(const std::string& language, const std::string& textKey) const {
        static bool missingTranslationLogged = false;
        if (!missingTranslationLogged) {
            Logger::instance()->logError("Translation in '" + language + "' not found for text key '" + textKey + "'");
            missingTranslationLogged = true;
        }
    }

}
