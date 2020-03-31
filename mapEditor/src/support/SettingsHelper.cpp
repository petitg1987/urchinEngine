#include "SettingsHelper.h"

#include <QApplication>
#include <QtCore/QStandardPaths>

namespace urchin
{

    SettingsHelper::SettingsHelper()
    {
        std::string applicationDirPath = QCoreApplication::applicationDirPath().toStdString();
        std::string settingsPropertiesPath = applicationDirPath + "/save/settings.properties";
        propertyFileHandler = std::make_unique<PropertyFileHandler>(settingsPropertiesPath);
    }

    void SettingsHelper::saveSetting(const std::string &settingId, const std::string &settingValue)
    {
        std::map<std::string, std::string> properties = propertyFileHandler->loadPropertyFile();
        properties[settingId] = settingValue;

        propertyFileHandler->savePropertyFile(properties);
    }

    std::string SettingsHelper::retrieveSetting(const std::string &settingId, const std::string &defaultValue) const
    {
        std::map<std::string, std::string> properties = propertyFileHandler->loadPropertyFile();
        const auto &it = properties.find(settingId);
        if(it != properties.end())
        {
            return it->second;
        }
        return defaultValue;
    }



}
