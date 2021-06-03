#include <functional>
#include <stdexcept>
#ifdef _WIN32
    #include <windows.h>
    #include <cstdio>
    #include <set>
    #include <intrin.h>
    #define GetProcAddress (void *)GetProcAddress
#else
    #include <sys/utsname.h>
    #include <sys/sysinfo.h>
    #include <zconf.h>
    #include <system/CommandExecutor.h>
    #include <util/TypeConverter.h>
#endif

#include <system/SystemInfo.h>
#include <util/StringUtil.h>
#include <util/FileUtil.h>

namespace urchin {

    std::string SystemInfo::retrieveOsInfo() {
        #ifdef _WIN32
            typedef NTSTATUS (WINAPI fRtlGetVersion) (PRTL_OSVERSIONINFOEXW);
            unsigned long osVersion;
            OSVERSIONINFOEXW osInfo;
            auto *RtlGetVersion = reinterpret_cast<fRtlGetVersion *>(GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion"));
            if (RtlGetVersion != nullptr) {
                osInfo.dwOSVersionInfoSize = sizeof(osInfo);
                RtlGetVersion(&osInfo);
                osVersion = osInfo.dwMajorVersion;
                return "Windows " + std::to_string(osVersion);
            }
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
            return TypeConverter::toUnsignedInt(numCpuCores);
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
                return (uint64_t)sysInfo.totalram;
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
        for (auto graphicsCardName : graphicsCardNames) {
            StringUtil::trim(graphicsCardName);
            graphicsCardNamesList += graphicsCardName + ", ";
        }
        if (graphicsCardNamesList.size() >= 2) {
            graphicsCardNamesList = graphicsCardNamesList.substr(0, graphicsCardNamesList.size() - 2);
        }
        return graphicsCardNamesList;
    }

    std::string SystemInfo::systemHash() {
        return retrieveCpuHash() + "-" + std::to_string(std::hash<std::string>{}(retrieveGraphicsCardNames()));
    }

    std::string SystemInfo::retrieveCpuHash() {
        #ifdef _WIN32
            int cpuInfo[4] = {0, 0, 0, 0};
            __cpuid(cpuInfo, 0);
            unsigned int hash = 0;
            auto* ptr = (uint16_t*)(&cpuInfo[0]);
            for (std::size_t i = 0; i < 8; i++) {
                hash += ptr[i];
            }
            return std::to_string(hash);
        #else
            unsigned int cpuInfo[4] = {0, 0, 0, 0};
            unsigned int ax = 0;
            __asm __volatile (
                "movl %%ebx, %%esi\n\t"
                "cpuid\n\t"
                "xchgl %%ebx, %%esi" :
                "=a" (cpuInfo[0]),
                "=S" (cpuInfo[1]),
                "=c" (cpuInfo[2]),
                "=d" (cpuInfo[3]) :
                "0" (ax)
            );

            unsigned int hash = 0;
            unsigned int* ptr = &cpuInfo[0];
            for (unsigned int i = 0; i < 4; i++) {
                hash += (ptr[i] & (unsigned int)0xFFFF) + (ptr[i] >> 16u);
            }
            return std::to_string(hash);
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

        #ifdef _WIN32
            return homeDirectory + "\\";
        #else
            return homeDirectory + "/";
        #endif
    }

    std::string SystemInfo::userDataDirectory() {
        #ifdef _WIN32
            std::string userDataDirectory = getenv("LOCALAPPDATA");
            if (!userDataDirectory.empty()) {
                return userDataDirectory + "\\";
            }
            return homeDirectory() + R"(\AppData\Local\)";
        #else
            return homeDirectory();
        #endif
    }

    std::string SystemInfo::executableDirectory() {
        unsigned int pathBufferSize = 2048;

        #ifdef _WIN32
            char* buffer = new char[pathBufferSize];
            GetModuleFileName(nullptr, buffer, pathBufferSize);
            std::string executablePath(buffer);
            delete[] buffer;
            return FileUtil::getDirectory(executablePath);
        #else
            char* buffer = new char[pathBufferSize];
            ssize_t readSize = readlink("/proc/self/exe", buffer, pathBufferSize);
            if (readSize > 0) {
                std::string executablePath(buffer, (unsigned long)readSize);
                delete[] buffer;
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


}
