#include "trigger/AutoTrigger.h"
#include "sound/localizable/LocalizableSound.h"

namespace urchin {

    AutoTrigger::AutoTrigger(PlayBehavior playBehavior, const std::shared_ptr<Sound>& sound) :
            SoundTrigger(AUTO_TRIGGER, playBehavior),
            sound(sound),
            isPlaying(false) {

    }

    const std::vector<SoundTrigger::TriggerAction>& AutoTrigger::evaluateTrigger(const Point3<float>& listenerPosition) {
        triggerActions.clear();
        if (sound.expired()) {
            return triggerActions;
        }

        bool needToPlaySound = false;
        if (sound.lock()->getSoundType() == Sound::SoundType::GLOBAL) {
            needToPlaySound = true;
        } else if (sound.lock()->getSoundType() == Sound::SoundType::LOCALIZABLE) {
            const auto& localizableSound = static_cast<const LocalizableSound&>(*sound.lock());
            Sphere soundSphere(localizableSound.getRadius(), localizableSound.getPosition());
            needToPlaySound = soundSphere.collideWithPoint(listenerPosition);
        } else {
            throw std::invalid_argument("Unknown sound type for auto trigger: " + std::to_string((int)sound.lock()->getSoundType()));
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

    std::unique_ptr<SoundTrigger> AutoTrigger::clone(const std::shared_ptr<Sound>& sound) const {
        return std::make_unique<AutoTrigger>(getPlayBehavior(), sound);
    }

}
