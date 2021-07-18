#include <AL/alc.h>
#include <stdexcept>

#include <device/AudioDevice.h>

namespace urchin {

    AudioDevice::AudioDevice() :
            device(nullptr),
            context(nullptr) {
        device = alcOpenDevice(nullptr);
        if (!device) {
            throw std::runtime_error("Impossible to find the sound device");
        }

        context = alcGetCurrentContext();
        if (!context) {
            context = alcCreateContext(device, nullptr);
            if (!context) {
                throw std::runtime_error("Impossible to create the sound context");
            }
        }
    }

    AudioDevice::~AudioDevice() {
        if (context) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
        }

        if (device) {
            alcCloseDevice(device);
        }
    }

    void AudioDevice::enable(bool enable) {
        ALCcontext *contextToEnable = enable ? context : nullptr;
        if (!alcMakeContextCurrent(contextToEnable)) {
            throw std::runtime_error("Impossible to make context current");
        }
    }

}
