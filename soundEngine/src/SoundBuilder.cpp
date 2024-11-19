#include <SoundBuilder.h>
#include <SoundEnvironment.h>
#include <sound/global/GlobalSound.h>
#include <sound/localizable/LocalizableSound.h>
#include <trigger/ManualTrigger.h>
#include <trigger/shape/SoundSphere.h>

namespace urchin {

    SoundBuilder::SoundBuilder(SoundEnvironment& soundEnvironment) :
            soundEnvironment(soundEnvironment) {

    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualEffect(std::string filename, PlayBehavior playBehavior) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(globalSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualMusic(std::string filename, PlayBehavior playBehavior){
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(globalSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualSpatialEffect(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(localizableSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualSpatialMusic(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(localizableSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newAreaEffect(std::string filename, std::shared_ptr<AreaTrigger> areaTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        return buildSoundComponent(std::move(globalSound), std::move(areaTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newAreaMusic(std::string filename, std::shared_ptr<AreaTrigger> areaTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        globalSound->preLoadChunks(chunkPreLoader);
        return buildSoundComponent(std::move(globalSound), std::move(areaTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newAreaSpatialEffect(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto areaTrigger = std::make_shared<AreaTrigger>(playBehavior, std::make_unique<SoundSphere>(radius, position, 0.0f));
        return buildSoundComponent(std::move(localizableSound), std::move(areaTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newAreaSpatialMusic(std::string filename, const Point3<float>& position, float radius, PlayBehavior playBehavior) {
        auto localizableSound = std::make_shared<LocalizableSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, radius);
        localizableSound->preLoadChunks(chunkPreLoader);
        auto areaTrigger = std::make_shared<AreaTrigger>(playBehavior, std::make_unique<SoundSphere>(radius, position, 0.0f));
        return buildSoundComponent(std::move(localizableSound), std::move(areaTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::buildSoundComponent(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) const {
        std::shared_ptr soundComponent = std::make_unique<SoundComponent>(std::move(sound), std::move(soundTrigger));
        soundEnvironment.addSoundComponent(soundComponent);
        return soundComponent;
    }

}
