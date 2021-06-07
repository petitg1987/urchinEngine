#ifdef _WIN32
    #include <windows.h>
#else
    #include <X11/Xlib.h>
    #include <X11/extensions/Xrandr.h>
#endif

#include <system/monitor/MonitorResolutionService.h>

namespace urchin {

    MonitorResolutionService::MonitorResolutionService() {
        loadMonitorResolutions();
        filterMonitorResolutions();
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
                int numRates;
                short* rates = XRRRates(display, 0, screenSizeIndex, &numRates);
                for (int rateIndex = 0; rateIndex < numRates; ++rateIndex) {
                    resolutions.emplace_back(MonitorResolution((unsigned int)xrrScreenSize[screenSizeIndex].width,
                                                               (unsigned int)xrrScreenSize[screenSizeIndex].height,
                                                               (unsigned int)rates[rateIndex]));
                }
            }
            XCloseDisplay(display);
        #endif
    }

    std::vector<MonitorResolution> MonitorResolutionService::getMonitorResolutions() const {
        return resolutions;
    }

    void MonitorResolutionService::filterMonitorResolutions() {
        //TODO ...
    }

}
