#include <AL/al.h>
#include <UrchinCommon.h>

#include <util/CheckState.h>
using namespace urchin;

void CheckState::check(std::string_view action) {
    static unsigned int numErrorsLogged = 0;
    ALenum err;
    while ((err = alGetError()) != AL_NO_ERROR && numErrorsLogged < MAX_ERRORS_LOG) {
        numErrorsLogged++;
        Logger::instance().logError("OpenAL error detected: '" + std::to_string(err) + "' while executing action: " + std::string(action));
    }
}

void CheckState::checkContext(ALCdevice* device, std::string_view action) {
    ALCenum err;
    while ((err = alcGetError(device)) != ALC_NO_ERROR) {
        Logger::instance().logError("OpenAL context error detected: '" + std::to_string(err) + "' while executing action: " + std::string(action));
    }
}
