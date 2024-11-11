#include <QApplication>

#include <StateSaveHelper.h>

namespace urchin {

    StateSaveHelper& StateSaveHelper::instance() {
        static StateSaveHelper instance;
        return instance;
    }

    StateSaveHelper::StateSaveHelper() {
        std::string saveDirPath = QCoreApplication::applicationDirPath().toStdString() + "/save/";
        if (!FileUtil::isDirectoryExist(saveDirPath)) {
            FileUtil::createDirectory(saveDirPath);
        }

        std::string propertiesFilePath = saveDirPath + "states.properties";
        if (!FileUtil::isFileExist(propertiesFilePath)) {
            FileUtil::createEmptyFile(propertiesFilePath);
        }

        propertyFileHandler = std::make_unique<PropertyFileHandler>(propertiesFilePath);
    }

    void StateSaveHelper::saveState(const std::string& stateId, const std::string& stateValue) const {
        std::map<std::string, std::string, std::less<>> properties = propertyFileHandler->loadPropertyFile();
        properties[stateId] = stateValue;

        propertyFileHandler->savePropertyFile(properties);
    }

    std::string StateSaveHelper::retrieveState(const std::string& stateId, const std::string& defaultValue) const {
        std::map<std::string, std::string, std::less<>> properties = propertyFileHandler->loadPropertyFile();
        return MapUtil::getWithDefault(properties, stateId, defaultValue);
    }

}
