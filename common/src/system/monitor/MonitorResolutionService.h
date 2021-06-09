#pragma once

#include <vector>

#include <system/monitor/MonitorResolution.h>

namespace urchin {

    class MonitorResolutionService {
        public:
            MonitorResolutionService(bool, bool);

            const std::vector<MonitorResolution>& getSupportedResolutions() const;

            unsigned int findClosestResolutionIndex(const std::string&) const;
            unsigned int findClosestResolutionIndex(const MonitorResolution&) const;

        private:
            void loadMonitorResolutions();
            void filterOutMonitorResolutions();
            bool isHighestFrequency(const MonitorResolution&) const;
            void sortUniqueResolutions();

            const unsigned int LOW_RESOLUTION_HEIGHT;
            bool filterOutLowResolutions;
            bool filterOutLowFrequencies;
            std::vector<MonitorResolution> resolutions;
    };

}
