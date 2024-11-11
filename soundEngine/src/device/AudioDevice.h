#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class AudioDevice {
        public:
            static AudioDevice& instance();
            ~AudioDevice();

            void enable() const;

        private:
            AudioDevice();

            ALCdevice* device;
            ALCcontext* context;
    };

}
