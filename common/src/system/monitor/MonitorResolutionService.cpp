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
        loadMonitorAspectRatios();
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

        for (auto& resolution : resolutions) {
            bool validResolution = !filterOutLowResolutions || resolution.getHeight() > LOW_RESOLUTION_HEIGHT;
            bool validFrequency = !filterOutLowFrequencies || isHighestFrequency(resolution);

            if (validResolution && validFrequency) {
                filteredResolutions.emplace_back(resolution);
            }
        }

        this->resolutions = filteredResolutions;
    }

    bool MonitorResolutionService::isHighestFrequency(MonitorResolution monitorResolution) const {
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

    void MonitorResolutionService::loadMonitorAspectRatios() {
        assert(!resolutions.empty());
        std::set<std::string> uniqueAspectRatios;
        for(auto& resolution : resolutions) {
            uniqueAspectRatios.insert(resolution.retrieveAspectRatio());
        }
        std::copy(uniqueAspectRatios.begin(), uniqueAspectRatios.end(), std::back_inserter(aspectRatios));
    }

    const std::vector<MonitorResolution>& MonitorResolutionService::getSupportedResolutions() const {
        return resolutions;
    }

    std::vector<MonitorResolution> MonitorResolutionService::getSupportedResolutions(const std::string& aspectRatio) const {
        std::vector<MonitorResolution> aspectRatioResolutions;

        for (auto& resolution : resolutions) {
            if(resolution.retrieveAspectRatio() == aspectRatio) {
                aspectRatioResolutions.emplace_back(resolution);
            }
        }

        return aspectRatioResolutions;
    }

    const std::vector<std::string>& MonitorResolutionService::getSupportedAspectRatios() const {
        return aspectRatios;
    }

}
