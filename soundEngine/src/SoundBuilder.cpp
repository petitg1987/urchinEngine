#include <SoundBuilder.h>
#include <SoundEnvironment.h>
#include <sound/global/GlobalSound.h>
#include <sound/localizable/LocalizableSound.h>
#include <trigger/AutoTrigger.h>
#include <trigger/AreaTrigger.h>
#include <trigger/ManualTrigger.h>

namespace urchin {

    SoundBuilder::SoundBuilder(SoundEnvironment& soundEnvironment) :
            soundEnvironment(soundEnvironment) {

    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualTriggerEffect(std::string filename, PlayBehavior playBehavior) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(globalSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualTriggerMusic(std::string filename, PlayBehavior playBehavior){
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(globalSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualTriggerLocalizableEffect(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(localizableSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualTriggerLocalizableMusic(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(localizableSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newAreaTriggerEffect(std::string filename, std::shared_ptr<AreaTrigger> areaTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        return buildSoundComponent(std::move(globalSound), std::move(areaTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newAreaTriggerMusic(std::string filename, std::shared_ptr<AreaTrigger> areaTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        return buildSoundComponent(std::move(globalSound), std::move(areaTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newLocalizableEffect(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto autoTrigger = std::make_shared<AutoTrigger>(playBehavior, localizableSound);
        return buildSoundComponent(std::move(localizableSound), std::move(autoTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newLocalizableMusic(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto autoTrigger = std::make_shared<AutoTrigger>(playBehavior, localizableSound);
        return buildSoundComponent(std::move(localizableSound), std::move(autoTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::buildSoundComponent(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) const {
        std::shared_ptr soundComponent = std::make_unique<SoundComponent>(std::move(sound), std::move(soundTrigger));
        soundEnvironment.addSoundComponent(soundComponent);
        return soundComponent;
    }

}
