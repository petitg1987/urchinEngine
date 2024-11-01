#pragma once

#include <UrchinCommon.h>

namespace urchin {

    class AudioDevice final : public Singleton<AudioDevice> {
        public:
            friend class Singleton;

            ~AudioDevice() override;

            void enable() const;

        private:
            AudioDevice();

            ALCdevice* device;
            ALCcontext* context;
    };

}
