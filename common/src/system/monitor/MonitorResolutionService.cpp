#include <iostream>
#include <algorithm>
#include <set>
#include <cassert>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <X11/Xlib.h>
    #include <X11/extensions/Xrandr.h>
#endif

#include <system/monitor/MonitorResolutionService.h>

namespace urchin {

    MonitorResolutionService::MonitorResolutionService(bool filterOutLowResolutions, bool filterOutLowFrequencies) :
            LOW_RESOLUTION_HEIGHT(719),
            filterOutLowResolutions(filterOutLowResolutions),
            filterOutLowFrequencies(filterOutLowFrequencies) {
        loadMonitorResolutions();
    }

    void MonitorResolutionService::loadMonitorResolutions() {
        #ifdef _WIN32
            DWORD iModeNum = 0;
            DEVMODE dmi{};
            dmi.dmSize = sizeof(dmi);
            while (EnumDisplaySettings(NULL, iModeNum++, &dmi) != 0) {
                resolutions.emplace_back(MonitorResolution((unsigned int)dmi.dmPelsWidth, (unsigned int)dmi.dmPelsHeight, (unsigned int)dmi.dmDisplayFrequency));
            }
        #else
            int numScreenSizes;
            Display* display = XOpenDisplay(nullptr);
            XRRScreenSize* xrrScreenSize = XRRSizes(display, 0, &numScreenSizes);
            for (int screenSizeIndex = 0; screenSizeIndex < numScreenSizes; ++screenSizeIndex) {
                int numFrequencies;
                short* frequencies = XRRRates(display, 0, screenSizeIndex, &numFrequencies);
                for (int frequencyIndex = 0; frequencyIndex < numFrequencies; ++frequencyIndex) {
                    resolutions.emplace_back(MonitorResolution((unsigned int)xrrScreenSize[screenSizeIndex].width,
                                                               (unsigned int)xrrScreenSize[screenSizeIndex].height,
                                                               (unsigned int)frequencies[frequencyIndex]));
                }
            }
            XCloseDisplay(display);
        #endif

        filterOutMonitorResolutions();
        sortResolutions();
    }

    void MonitorResolutionService::filterOutMonitorResolutions() {
        std::vector<MonitorResolution> filteredResolutions;
        filteredResolutions.reserve(20); //estimated memory size

        for (auto& resolution : resolutions) {
            bool validResolution = !filterOutLowResolutions || resolution.getHeight() > LOW_RESOLUTION_HEIGHT;
            bool validFrequency = !filterOutLowFrequencies || isHighestFrequency(resolution);

            if (validResolution && validFrequency) {
                filteredResolutions.emplace_back(resolution);
            }
        }

        this->resolutions = filteredResolutions;
    }

    bool MonitorResolutionService::isHighestFrequency(const MonitorResolution& monitorResolution) const {
        return std::all_of(resolutions.begin(), resolutions.end(), [&monitorResolution](const auto& res){
            return res.getWidth() != monitorResolution.getWidth() || res.getHeight() != monitorResolution.getHeight() || res.getFrequency() <= monitorResolution.getFrequency();
        });
    }

    void MonitorResolutionService::sortResolutions() {
        assert(!resolutions.empty());
        sort(resolutions.begin(), resolutions.end(), [](const MonitorResolution& lhs, const MonitorResolution& rhs) {
            if(lhs.getWidth() == rhs.getWidth()) {
                if(lhs.getHeight() == rhs.getHeight()) {
                    return lhs.getFrequency() > rhs.getFrequency();
                }
                return lhs.getHeight() > rhs.getHeight();
            }
            return lhs.getWidth() > rhs.getWidth();
        });
    }

    const std::vector<MonitorResolution>& MonitorResolutionService::getSupportedResolutions() const {
        return resolutions;
    }

    unsigned int MonitorResolutionService::findClosestResolutionIndex(const std::string& monitorResolutionId) const {
        for(unsigned int i = 0; i < resolutions.size(); ++i) {
            if(resolutions[i].getId() == monitorResolutionId) {
                return i;
            }
        }

        //resolution not found, cause: user changed of monitor or user modified manually the configuration file
        return findClosestResolutionIndex(MonitorResolution(monitorResolutionId));
    }

    unsigned int MonitorResolutionService::findClosestResolutionIndex(const MonitorResolution& monitorResolution) const {
        constexpr float MONITOR_SIZE_WEIGHT = 2.0f;
        unsigned int closestResolutionIndex = 0;
        float bestScore = 0.0f;

        for(unsigned int i = 0; i < resolutions.size(); ++i) {
            float widthScore = std::min((float)resolutions[i].getWidth(), (float)monitorResolution.getWidth()) /
                    std::max((float)resolutions[i].getWidth(), (float)monitorResolution.getWidth());
            float heightScore = std::min((float)resolutions[i].getHeight(), (float)monitorResolution.getHeight()) /
                    std::max((float)resolutions[i].getHeight(), (float)monitorResolution.getHeight());
            float frequencyScore = std::min((float)resolutions[i].getFrequency(), (float)monitorResolution.getFrequency()) /
                    std::max((float)resolutions[i].getFrequency(), (float)monitorResolution.getFrequency());
            float score = widthScore * MONITOR_SIZE_WEIGHT + heightScore * MONITOR_SIZE_WEIGHT + frequencyScore;

            if(score > bestScore) {
                bestScore = score;
                closestResolutionIndex = i;
            }
        }

        return closestResolutionIndex;
    }

}
