#include <functional>
#include <stdexcept>
#include <cmath>
#ifdef _WIN32
    #include <windows.h>
    #include <set>
#else
    #include <sys/utsname.h>
    #include <sys/sysinfo.h>
    #include <zconf.h>
    #include <algorithm>
    #include <system/CommandExecutor.h>
    #include <util/TypeConverter.h>
    #include <gnu/libc-version.h>
#endif

#include <system/SystemInfo.h>
#include <util/StringUtil.h>
#include <util/FileUtil.h>
#include <logger/Logger.h>

namespace urchin {

    std::string SystemInfo::retrieveOsInfo() {
        #ifdef _WIN32
            return "Windows";
        #else
            struct utsname utsName = {};
            if (!uname(&utsName)) {
                return std::string(utsName.sysname) + ", " + std::string(utsName.release) + ", " + std::string(utsName.version);
            }
            return "Linux";
        #endif
    }

    unsigned int SystemInfo::retrieveCpuCores() {
        #ifdef _WIN32
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            return (unsigned int)sysInfo.dwNumberOfProcessors;
        #else
            std::string numCpuCores = CommandExecutor::execute(R"(grep --max-count=1 "cpu cores" /proc/cpuinfo | cut -d ":" -f 2)");
            StringUtil::ltrim(numCpuCores);
            if (TypeConverter::isUnsignedInt(numCpuCores)) {
                return TypeConverter::toUnsignedInt(numCpuCores);
            } else {
                return 0;
            }
        #endif
    }

    uint64_t SystemInfo::retrieveTotalMemory() {
        #ifdef _WIN32
            MEMORYSTATUSEX statex;
            statex.dwLength = sizeof(statex);
            if (GlobalMemoryStatusEx(&statex)) {
                return (uint64_t)statex.ullTotalPhys;
            }
            return 0L;
        #else
            struct sysinfo sysInfo = {};
            if (!sysinfo(&sysInfo)) {
                return ((uint64_t)sysInfo.totalram) * ((uint64_t)sysInfo.mem_unit);
            }
            return 0L;
        #endif
    }

    std::string SystemInfo::retrieveGraphicsCardNames() {
        std::vector<std::string> graphicsCardNames;
        #ifdef _WIN32
            std::set<std::string> setGraphicsCardNames;
            for (int i = 0; i < 32; i++) {
                DISPLAY_DEVICE displayDevice = {};
                displayDevice.cb = sizeof(displayDevice);
                BOOL result = EnumDisplayDevices(nullptr, i, &displayDevice, EDD_GET_DEVICE_INTERFACE_NAME);
                if (!result) {
                    break;
                }
                setGraphicsCardNames.insert(std::string(displayDevice.DeviceString));
            }
            graphicsCardNames.insert(graphicsCardNames.begin(), setGraphicsCardNames.begin(), setGraphicsCardNames.end());
        #else
            std::string graphicCardNames = CommandExecutor::execute(R"(lspci | grep -oE "VGA.*" | cut -d ":" -f 2)");
            StringUtil::split(graphicCardNames, '\n', graphicsCardNames);
        #endif

        std::string graphicsCardNamesList;
        for (auto& graphicsCardName : graphicsCardNames) {
            StringUtil::trim(graphicsCardName);
            graphicsCardNamesList += graphicsCardName + ", ";
        }
        if (graphicsCardNamesList.size() >= 2) {
            graphicsCardNamesList = graphicsCardNamesList.substr(0, graphicsCardNamesList.size() - 2);
        }
        return graphicsCardNamesList;
    }

    std::string SystemInfo::retrieveLibcVersion() {
        #ifdef _WIN32
            return "[libgcc_s_seh-1.dll]";
        #else
            return std::string(gnu_get_libc_version());
        #endif
    }

    std::string SystemInfo::homeDirectory() {
        std::string homeDirectory = getEnvVariable("HOME");
        if (homeDirectory.empty()) {
            homeDirectory = getEnvVariable("USERPROFILE");
            if (homeDirectory.empty()) {
                homeDirectory = getEnvVariable("HOMEDRIVE") + getEnvVariable("HOMEPATH");
            }
        }

        if (homeDirectory.empty()) {
            throw std::runtime_error("Impossible to find home directory. Please define env. variable 'HOME' or 'USERPROFILE' or 'HOMEDRIVE' + 'HOMEPATH'.");
        }
        return homeDirectory + FileUtil::directorySeparator();
    }

    std::string SystemInfo::userDataDirectory() {
        #ifdef _WIN32
            std::string userDataDirectory = std::getenv("LOCALAPPDATA");
            if (!userDataDirectory.empty()) {
                return userDataDirectory + "\\";
            }
            return homeDirectory() + R"(AppData\Local\)";
        #else
            return homeDirectory() + ".local" + FileUtil::directorySeparator() + "share" + FileUtil::directorySeparator();
        #endif
    }

    std::string SystemInfo::executableDirectory() {
        constexpr unsigned int PATH_BUFFER_SIZE = 2048;
        std::array<char, PATH_BUFFER_SIZE> buffer = {};

        #ifdef _WIN32
            GetModuleFileName(nullptr, buffer.data(), buffer.size());
            std::string executablePath(buffer.data());
            return FileUtil::getDirectory(executablePath);
        #else
            ssize_t readSize = readlink("/proc/self/exe", buffer.data(), buffer.size());
            if (readSize > 0) {
                std::string executablePath(buffer.data(), (unsigned long)readSize);
                return FileUtil::getDirectory(executablePath);
            }
            throw std::runtime_error("Cannot read /proc/self/exe on Linux system.");
        #endif
    }

    std::string SystemInfo::getEnvVariable(const std::string& variableName) {
        const char* charValue = std::getenv(variableName.c_str());
        if (charValue == nullptr) {
            return "";
        }
        return std::string(charValue);
    }

    SystemInfo::KeyboardLayout SystemInfo::keyboardLayout() {
        #ifdef _WIN32
            CHAR layoutIdChar[KL_NAMELENGTH];
            GetKeyboardLayoutNameA(layoutIdChar);
            std::string layoutId(layoutIdChar);
            //layout ID list: https://docs.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values
            if (StringUtil::insensitiveEquals(layoutId, "0000080c") /* Belgian French */ ||
                    StringUtil::insensitiveEquals(layoutId, "00000813") /* Belgian (Period) */ ||
                    StringUtil::insensitiveEquals(layoutId, "0001080c") /* Belgian (Comma) */ ||
                    StringUtil::insensitiveEquals(layoutId, "0000040c") /* French */) {
                return KeyboardLayout::AZERTY;
            }
            return KeyboardLayout::DEFAULT;
        #else
            const std::string LAYOUT_LABEL = "layout:";
            std::string xKeyboardInfo = CommandExecutor::execute("setxkbmap -query"); //only work with X11
            std::size_t layoutBeginLocation = xKeyboardInfo.find(LAYOUT_LABEL);
            if (layoutBeginLocation != std::string::npos) {
                layoutBeginLocation += LAYOUT_LABEL.size();
                std::size_t layoutEndLocation = xKeyboardInfo.find('\n', layoutBeginLocation);
                std::size_t searchLength = layoutEndLocation != std::string::npos ? (layoutEndLocation - layoutBeginLocation) : xKeyboardInfo.size() - layoutBeginLocation;
                std::string layout = xKeyboardInfo.substr(layoutBeginLocation, searchLength);
                StringUtil::trim(layout);
                if (StringUtil::insensitiveEquals(layout, "fr") || StringUtil::insensitiveEquals(layout, "be")) {
                    return KeyboardLayout::AZERTY;
                }
            }
            return KeyboardLayout::DEFAULT;
        #endif
    }

    std::string SystemInfo::userLanguage() {
        try {
            #ifdef _WIN32
                char locale[10];
		        int localeLength = GetLocaleInfo(GetSystemDefaultUILanguage(), LOCALE_SISO639LANGNAME, locale, sizeof(locale));
		        std::string userLocale(locale, (unsigned long)localeLength);
                if (!userLocale.empty() && (int)userLocale[userLocale.size() - 1] == 0) {
                    userLocale.resize(userLocale.size() - 1);
                }
                Logger::instance().logInfo("User locale retrieved: " + userLocale);
                return userLocale;
            #else
                std::string userLocale = std::locale("").name();
                Logger::instance().logInfo("User locale retrieved: " + userLocale);

                const std::string LC_CTYPE_STR = "LC_CTYPE=";
                if (std::ranges::count(userLocale, '_') == 1) { //format is probably: "en_US.UTF8"
                    std::size_t pos = userLocale.find('_');
                    if (pos == std::string::npos) {
                        return "";
                    }
                    return userLocale.substr(0, pos);
                } else if (std::size_t cTypePos = userLocale.find(LC_CTYPE_STR); cTypePos != std::string::npos) { //format is probably: "LC_CTYPE=en_US.UTF-8;LC_NUMERIC=..."
                    std::size_t cTypeValuePos = cTypePos + LC_CTYPE_STR.size();
                    if (cTypeValuePos + 2 >= userLocale.size()) {
                        return "";
                    }
                    return userLocale.substr(cTypeValuePos, 2);
                }
                return "";
            #endif
        } catch (const std::runtime_error&) {
            return "";
        }
    }

}
