#pragma once

#include <vector>

#include <system/monitor/MonitorResolution.h>

namespace urchin {

    class MonitorResolutionService {
        public:
            MonitorResolutionService(bool, bool);

            const std::vector<MonitorResolution>& getSupportedResolutions() const;
            std::vector<MonitorResolution> getSupportedResolutions(const std::string&) const;

            const std::vector<std::string>& getSupportedAspectRatios() const;

        private:
            void loadMonitorResolutions();
            void filterOutMonitorResolutions();
            bool isHighestFrequency(MonitorResolution) const;
            void sortResolutions();

            void loadMonitorAspectRatios();

            const unsigned int LOW_RESOLUTION_HEIGHT;
            bool filterOutLowResolutions;
            bool filterOutLowFrequencies;
            std::vector<MonitorResolution> resolutions;
            std::vector<std::string> aspectRatios;
    };

}
