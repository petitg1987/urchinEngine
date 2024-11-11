#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class StateSaveHelper {
        public:
            static StateSaveHelper& instance();

            static const std::string PREFERRED_PATH_MAP;
            static const std::string CAMERA_POSITION;

            void saveState(const std::string&, const std::string&) const;
            std::string retrieveState(const std::string&, const std::string&) const;

        private:
            StateSaveHelper();

            std::unique_ptr<PropertyFileHandler> propertyFileHandler;
    };

}
