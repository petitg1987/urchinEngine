#include <UrchinCommon.h>

#include <util/CheckState.h>
using namespace urchin;

namespace urchin {

    void CheckState::checkContext(ALCdevice* device, std::string_view action) {
        ALCenum err;
        while ((err = alcGetError(device)) != ALC_NO_ERROR) {
            Logger::instance().logError("OpenAL context error detected: '" + std::to_string(err) + "' while executing action: " + std::string(action));
        }
    }

}
