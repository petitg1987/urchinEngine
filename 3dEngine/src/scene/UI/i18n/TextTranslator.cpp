#include <filesystem>
#include <UrchinCommon.h>

#include <scene/UI/i18n/TextTranslator.h>

namespace urchin {

    TextTranslator::TextTranslator() :
            mainLanguage("en"),
            filePrefix("text_") ,
            filePostfix(".properties") {
        textFilesDirectoryName = FileSystem::instance()->getResourcesDirectory() + ConfigService::instance()->getStringValue("ui.textLocation");
        checkMissingTranslation();
    }

    void TextTranslator::checkMissingTranslation() const {
        auto mainLanguageTexts = retrieveLanguageTexts(mainLanguage);
        for (const auto& dir : std::filesystem::directory_iterator(textFilesDirectoryName)) {
            std::string filename = dir.path().filename().u8string();
            if (dir.is_regular_file() && filename.size() == filePrefix.size() + 2 + filePostfix.size()) {
                std::string language = filename.substr(filePrefix.size(), 2);
                if(language != mainLanguage) {
                    auto languageTexts = retrieveLanguageTexts(language);

                    std::map<std::string, std::string> textsDiff;
                    std::set_difference(mainLanguageTexts.begin(), mainLanguageTexts.end(), languageTexts.begin(), languageTexts.end(), std::inserter(textsDiff, textsDiff.begin()));

                    if (!textsDiff.empty()) {
                        logMissingTranslation(language, textsDiff.begin()->first);
                        break;
                    }
                }
            }
        }
    }

    void TextTranslator::logMissingTranslation(const std::string& language, const std::string& textKey) const {
        static bool missingTranslationLogged = false;
        if (!missingTranslationLogged) {
            Logger::instance()->logWarning("Translation in '" + language + "' not found for text key '" + textKey + "'");
            missingTranslationLogged = true;
        }
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
            loadedLanguageTexts = retrieveLanguageTexts(language);
            loadedLanguage = language;
        }
    }

    std::map<std::string, std::string> TextTranslator::retrieveLanguageTexts(const std::string& language) const {
        return PropertyFileHandler(textFilesDirectoryName + filePrefix + language + filePostfix).loadPropertyFile();
    }

}
