#ifndef URCHINENGINE_DEVICEMANAGER_H
#define URCHINENGINE_DEVICEMANAGER_H

#include "UrchinCommon.h"

namespace urchin {

    class DeviceManager : public Singleton<DeviceManager> {
        public:
            friend class Singleton<DeviceManager>;

        private:
            DeviceManager();
            ~DeviceManager() override;
    };

}

#endif
