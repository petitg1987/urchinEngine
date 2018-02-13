#include <AL/alc.h>
#include <stdexcept>

#include "DeviceManager.h"

namespace urchin
{

    /**
     * Initialize OpenAL: create context and make it current
     */
    DeviceManager::DeviceManager()
    {
        ALCdevice *device = alcOpenDevice(nullptr);
        if (!device)
        {
            throw std::runtime_error("Impossible to found sound device.");
        }

        ALCcontext *context = alcGetCurrentContext();
        if(!context)
        {
            context = alcCreateContext(device, nullptr);
            if (!context)
            {
                throw std::runtime_error("Impossible to create sound context.");
            }
        }

        if (!alcMakeContextCurrent(context))
        {
            throw std::runtime_error("Impossible to make context current.");
        }
    }

    /**
    * Shutdown OpenAL: destroy context and close device
    */
    DeviceManager::~DeviceManager()
    {
        ALCcontext *context = alcGetCurrentContext();
        ALCdevice *device  = alcGetContextsDevice(context);

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);

        alcCloseDevice(device);
    }

}
