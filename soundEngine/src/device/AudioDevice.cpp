#include <stdexcept>

#include <device/AudioDevice.h>
#include <util/CheckState.h>

namespace urchin {

    AudioDevice& AudioDevice::instance() {
        static AudioDevice instance;
        return instance;
    }

    AudioDevice::AudioDevice() :
            device(nullptr),
            context(nullptr) {
        Logger::instance().logInfo("Creating an audio device");

        device = alcOpenDevice(nullptr);
        if (!device) {
            throw std::runtime_error("Impossible to find the sound device");
        }

        context = alcGetCurrentContext();
        if (!context) {
            context = alcCreateContext(device, nullptr);
            CheckState::checkContext(device, "create context");
            if (!context) {
                throw std::runtime_error("Impossible to create the sound context");
            }
        }

        Logger::instance().logInfo("Audio device created");
    }

    AudioDevice::~AudioDevice() {
        if (context) {
            alcMakeContextCurrent(nullptr);
            CheckState::checkContext(device, "reset current context");

            alcDestroyContext(context);
            CheckState::checkContext(device, "destroy context");
        }

        if (device && !alcCloseDevice(device)) {
            Logger::instance().logWarning("Failed to close OpenAL device");
        }
    }

    void AudioDevice::enable() const {
        bool makeContextCurrentRes = alcMakeContextCurrent(context);
        CheckState::checkContext(device, "make context current");
        if (!makeContextCurrentRes) {
            throw std::runtime_error("Impossible to make context current");
        }
    }

}
