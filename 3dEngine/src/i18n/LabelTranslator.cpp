#include <filesystem>
#include <regex>
#include <UrchinCommon.h>

#include <i18n/LabelTranslator.h>

namespace urchin {

    LabelTranslator::LabelTranslator() :
            FILE_PREFIX("labels_") ,
            FILE_POSTFIX(".properties") {
        std::string uiLabelsLocation = ConfigService::instance().getStringValue("ui.labelsLocation");

        if (!uiLabelsLocation.empty()) {
            labelsFilesDirectoryName = FileSystem::instance().getResourcesDirectory() + uiLabelsLocation;
            loadAvailableLanguages();
        }
    }

    void LabelTranslator::loadAvailableLanguages() {
        if (FileUtil::isDirectoryExist(labelsFilesDirectoryName)) {
            for (const auto& dir : std::filesystem::directory_iterator(labelsFilesDirectoryName)) {
                std::string filename = dir.path().filename().string();
                if (dir.is_regular_file() && filename.size() > FILE_PREFIX.size() + FILE_POSTFIX.size()) {
                    std::size_t endLanguagePosition = filename.find(FILE_POSTFIX);
                    std::string language = filename.substr(FILE_PREFIX.size(), endLanguagePosition - FILE_PREFIX.size());
                    availableLanguages.emplace_back(language);
                }
            }
        }
    }

    void LabelTranslator::checkTranslationError() const {
        for (unsigned int i = 0; i < availableLanguages.size(); ++i) {
            std::string firstLanguage = availableLanguages[i];
            auto firstLanguageLabels = retrieveLanguageLabels(firstLanguage);

            std::string secondLanguage = availableLanguages[(i + 1) % availableLanguages.size()];
            auto secondLanguageLabels = retrieveLanguageLabels(secondLanguage);

            for (const auto& [firstLabelKey, firstLabelValue] : firstLanguageLabels) {
                auto itFind = secondLanguageLabels.find(firstLabelKey);
                if (itFind == secondLanguageLabels.end()) {
                    Logger::instance().logError("Translation in '" + secondLanguage + "' not found for label key '" + firstLabelKey + "' but present in '" + firstLanguage + "'");
                    break;
                } else if (i == availableLanguages.size() - 1) {
                    break; //no need comparison between the last and the first language
                }

                std::string secondLabelValue = itFind->second;
                if (firstLabelValue.empty() != secondLabelValue.empty()) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on emptiness for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (std::isspace(firstLabelValue[0]) != std::isspace(secondLabelValue[0])) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on first spacing letter for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (std::ispunct(firstLabelValue[0]) != std::ispunct(secondLabelValue[0])) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on first punctuation letter for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (firstLabelValue.ends_with(' ') != secondLabelValue.ends_with(' ')) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on space ending for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (firstLabelValue.ends_with(':') != secondLabelValue.ends_with(':')) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on colon ending for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (firstLabelValue.ends_with(": ") != secondLabelValue.ends_with(": ")) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on colon+space ending for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (std::ispunct(firstLabelValue[firstLabelValue.size() - 1]) != std::ispunct(secondLabelValue[secondLabelValue.size() - 1])) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on punctuation ending for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (std::ranges::count(firstLabelValue, '\n') != std::ranges::count(secondLabelValue, '\n')) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on line return for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                } else if (StringUtil::countOccurrence(firstLabelValue, "\n ") != StringUtil::countOccurrence(secondLabelValue, "\n ")) {
                    Logger::instance().logError("Translation value for label '" + firstLabelKey + "' are different on line return+space for languages '" + firstLanguage + "' and '" + secondLanguage + "'");
                }
            }
        }
    }

    LabelStatistics LabelTranslator::computeStatistics(const std::string& language) const {
        LabelStatistics labelStatistics{};
        auto labels = retrieveLanguageLabels(language);

        labelStatistics.labelsCount = (unsigned int)labels.size();
        labelStatistics.wordsCount = 0;

        for (const auto &[key, value] : labels) {
            std::string text = value;

            //remove parameters (nothing to translate)
            std::regex paramRegex("\\{[a-zA-Z-]+}");
            text = std::regex_replace(text, paramRegex, " ");

            //remove numbers (nothing to translate)
            std::regex numberRegex("[0-9]+");
            text = std::regex_replace(text, numberRegex, " ");

            //remove symbols
            std::regex symbolRegex(R"(\{|\}|\(|\)|\/|<|>|-|:|_|,|;|\.|\?|!|')");
            text = std::regex_replace(text, symbolRegex, " ");

            //handle multi-lines
            StringUtil::replaceAll(text, "\n", " ");

            //remove double spaces introduced by above replacements
            for (std::size_t i = 0; i < 6; ++i) {
                StringUtil::replaceAll(text, "  ", " ");
            }

            //remove start / end spaces
            std::regex beginSpaceRegex("^ (.*)$");
            text = std::regex_replace(text, beginSpaceRegex, "$1");
            std::regex endSpaceRegex("^(.*) $");
            text = std::regex_replace(text, endSpaceRegex, "$1");

            labelStatistics.wordsCount += (unsigned int)StringUtil::split(text, ' ').size();
        }

        return labelStatistics;
    }

    const std::vector<std::string>& LabelTranslator::getAvailableLanguages() const {
        return availableLanguages;
    }

    const std::map<std::string, std::string, std::less<>>& LabelTranslator::getLanguageLabels(const std::string& language) {
        loadLanguageLabels(language);
        return loadedLanguageLabels;
    }

    std::string LabelTranslator::translate(const std::string& language, std::string_view labelKey) {
        loadLanguageLabels(language);

        if (labelKey.empty()) {
            return "";
        }

        auto itFind = loadedLanguageLabels.find(labelKey);
        if (itFind != loadedLanguageLabels.end()) {
            return itFind->second;
        }

        logMissingTranslation(language, std::string(labelKey));
        return "??" + std::string(labelKey) + "??";
    }

    void LabelTranslator::loadLanguageLabels(const std::string& language) {
        if (language != loadedLanguage) {
            if (std::ranges::count(availableLanguages, language)) {
                loadedLanguageLabels = retrieveLanguageLabels(language);
            } else {
                loadedLanguageLabels.clear();
                logMissingLanguage(language);
            }
            loadedLanguage = language;
        }
    }

    std::map<std::string, std::string, std::less<>> LabelTranslator::retrieveLanguageLabels(const std::string& language) const {
        return PropertyFileHandler(labelsFilesDirectoryName + FILE_PREFIX + language + FILE_POSTFIX).loadPropertyFile();
    }

    void LabelTranslator::logMissingLanguage(const std::string& language) const {
        static bool missingLanguageLogged = false;
        if (!missingLanguageLogged) {
            Logger::instance().logError("Translated labels for language '" + language + "' not found");
            missingLanguageLogged = true;
        }
    }

    void LabelTranslator::logMissingTranslation(const std::string& language, const std::string& labelKey) const {
        static bool missingTranslationLogged = false;
        if (!missingTranslationLogged) {
            Logger::instance().logError("Translation in '" + language + "' not found for label key '" + labelKey + "'");
            missingTranslationLogged = true;
        }
    }

}
