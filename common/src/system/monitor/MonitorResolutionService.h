#pragma once

#include <vector>
#include <pattern//singleton/Singleton.h>

#include <system/monitor/MonitorResolution.h>

namespace urchin {

    class MonitorResolutionService {
        public:
            MonitorResolutionService();

            std::vector<MonitorResolution> getMonitorResolutions() const;

        private:
            void loadMonitorResolutions();
            void filterMonitorResolutions();

            std::vector<MonitorResolution> resolutions;
    };

}
