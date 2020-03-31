#ifndef URCHINENGINE_SETTINGSHELPER_H
#define URCHINENGINE_SETTINGSHELPER_H

#include "UrchinCommon.h"

namespace urchin
{

    class SettingsHelper : public Singleton<SettingsHelper>
    {
        public:
            friend class Singleton<SettingsHelper>;

            void saveSetting(const std::string &, const std::string &);
            std::string retrieveSetting(const std::string &, const std::string &) const;

        private:
            SettingsHelper();

            std::unique_ptr<PropertyFileHandler> propertyFileHandler;
    };

}

#endif
