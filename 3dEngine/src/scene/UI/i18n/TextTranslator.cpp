#include <UrchinCommon.h>

#include <scene/UI/i18n/TextTranslator.h>

namespace urchin {

    TextTranslator::TextTranslator() {
        textDirectoryName = FileSystem::instance()->getResourcesDirectory() + ConfigService::instance()->getStringValue("ui.textLocation");
    }

    std::string TextTranslator::translate(const std::string& language, const std::string& textKey) {
        std::map<std::string, std::string> languageTexts = PropertyFileHandler(textDirectoryName + "text_" + language + ".properties").loadPropertyFile(); //TODO add cache
        auto itFind = languageTexts.find(textKey);
        if(itFind != languageTexts.end()) {
            return itFind->second;
        }

        //TODO add warning !
        return "@" + textKey;
    }

}
