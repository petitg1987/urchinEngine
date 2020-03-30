#include "PreferredPathHelper.h"

#include <QApplication>
#include <QtCore/QStandardPaths>

namespace urchin
{

    PreferredPathHelper::PreferredPathHelper()
    {
        std::string applicationDirPath = QCoreApplication::applicationDirPath().toStdString();
        std::string preferredPropertiesPath = applicationDirPath + "/save/preferredPaths.properties";
        propertyFileHandler = std::make_unique<PropertyFileHandler>(preferredPropertiesPath);
    }

    void PreferredPathHelper::savePreferredPath(const std::string &pathId, const std::string &pathValue)
    {
        std::map<std::string, std::string> properties = propertyFileHandler->loadPropertyFile();
        properties.insert(std::pair<std::string, std::string>(pathId, pathValue));

        propertyFileHandler->savePropertyFile(properties);
    }

    std::string PreferredPathHelper::retrievePreferredPath(const std::string &pathId) const
    {
        std::map<std::string, std::string> properties = propertyFileHandler->loadPropertyFile();
        const auto &itPathValue = properties.find(pathId);
        if(itPathValue != properties.end())
        {
            return itPathValue->second;
        }
        return "";
    }



}
