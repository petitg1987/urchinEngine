#include <trigger/AutoTrigger.h>
#include <sound/localizable/LocalizableSound.h>

namespace urchin {

    AutoTrigger::AutoTrigger(PlayBehavior playBehavior, std::shared_ptr<Sound> sound) :
            SoundTrigger(AUTO_TRIGGER, playBehavior),
            sound(std::move(sound)),
            isPlaying(false) {

    }

    const std::vector<SoundTrigger::TriggerAction>& AutoTrigger::evaluateTrigger(const Point3<float>& listenerPosition) {
        triggerActions.clear();

        bool needToPlaySound = false;
        if (sound->getSoundType() == Sound::GLOBAL) {
            needToPlaySound = true;
        } else if (sound->getSoundType() == Sound::LOCALIZABLE) {
            const auto& localizableSound = static_cast<const LocalizableSound&>(*sound);
            Sphere soundSphere(localizableSound.getRadius(), localizableSound.getPosition());
            needToPlaySound = soundSphere.collideWithPoint(listenerPosition);
        } else {
            throw std::invalid_argument("Unknown sound type for auto trigger: " + std::to_string(sound->getSoundType()));
        }

        if (!isPlaying && needToPlaySound) {
            isPlaying = true;
            if (getPlayBehavior() == PlayBehavior::PLAY_LOOP) {
                triggerActions.emplace_back(PLAY_NEW_LOOP);
            } else if (getPlayBehavior() == PlayBehavior::PLAY_ONCE) {
                triggerActions.emplace_back(PLAY_NEW);
            } else {
                throw std::runtime_error("Unknown play behavior: " + std::to_string((int)getPlayBehavior()));
            }
        } else if (isPlaying && !needToPlaySound) {
            isPlaying = false;
            triggerActions.emplace_back(STOP_ALL);
        }

        return triggerActions;
    }

}
