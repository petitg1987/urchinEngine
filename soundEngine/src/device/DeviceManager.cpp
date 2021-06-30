#include <AL/alc.h>
#include <stdexcept>

#include <device/DeviceManager.h>

namespace urchin {

    /**
     * Initialize OpenAL: create context and make it current
     */
    DeviceManager::DeviceManager() {
        ALCdevice* device = alcOpenDevice(nullptr);
        if (!device) {
            auto devices = retrieveDevices();
            if(!devices.empty()) {
                std::string firstDevice = devices[0];
                Logger::instance()->logInfo("No default audio device found. Use the first audio device: " + firstDevice);
                device = alcOpenDevice(firstDevice.c_str());
            }
        }
        if(!device) {
            throw std::runtime_error("Impossible to found sound device.");
        }

        ALCcontext* context = alcGetCurrentContext();
        if (!context) {
            context = alcCreateContext(device, nullptr);
            if (!context) {
                throw std::runtime_error("Impossible to create sound context.");
            }
        }

        if (!alcMakeContextCurrent(context)) {
            throw std::runtime_error("Impossible to make context current.");
        }
    }

    /**
    * Shutdown OpenAL: destroy context and close device
    */
    DeviceManager::~DeviceManager() {
        ALCcontext* context = alcGetCurrentContext();
        ALCdevice *device  = alcGetContextsDevice(context);

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);

        alcCloseDevice(device);
    }

    std::vector<std::string> DeviceManager::retrieveDevices() const {
        std::vector<std::string> devices;
        const ALCchar* devicesList = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        if (devicesList) {
            while (strlen(devicesList) > 0) {
                devices.emplace_back(devicesList);
                devicesList += strlen(devicesList) + 1;
            }
        }
        return devices;
    }
}
