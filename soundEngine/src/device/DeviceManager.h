#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class DeviceManager : public Singleton<DeviceManager> {
        public:
            friend class Singleton<DeviceManager>;

        private:
            DeviceManager();
            ~DeviceManager() override;

            std::vector<std::string> retrieveDevices() const;
    };

}
