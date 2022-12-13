#include <SoundBuilder.h>
#include <SoundEnvironment.h>
#include <sound/global/GlobalSound.h>
#include <sound/spatial/SpatialSound.h>
#include <trigger/ManualTrigger.h>

namespace urchin {

    SoundBuilder::SoundBuilder(SoundEnvironment& soundEnvironment) :
            soundEnvironment(soundEnvironment) {

    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualEffect(std::string filename, PlayBehavior playBehavior) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(globalSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualMusic(std::string filename, PlayBehavior playBehavior){
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(globalSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualSpatialEffect(std::string filename, const Point3<float>& position, float inaudibleDistance, PlayBehavior playBehavior) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, inaudibleDistance);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(spatialSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newManualSpatialMusic(std::string filename, const Point3<float>& position, float inaudibleDistance, PlayBehavior playBehavior) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, inaudibleDistance);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return buildSoundComponent(std::move(spatialSound), std::move(manualTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newZoneEffect(std::string filename, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        return buildSoundComponent(std::move(globalSound), std::move(zoneTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newZoneMusic(std::string filename, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        return buildSoundComponent(std::move(globalSound), std::move(zoneTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newZoneSpatialEffect(std::string filename, const Point3<float>& position, float inaudibleDistance, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, inaudibleDistance);
        return buildSoundComponent(std::move(spatialSound), std::move(zoneTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::newZoneSpatialMusic(std::string filename, const Point3<float>& position, float inaudibleDistance, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, inaudibleDistance);
        return buildSoundComponent(std::move(spatialSound), std::move(zoneTrigger));
    }

    std::shared_ptr<SoundComponent> SoundBuilder::buildSoundComponent(std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) {
        std::shared_ptr<SoundComponent> soundComponent = std::make_unique<SoundComponent>(std::move(sound), std::move(soundTrigger));
        soundEnvironment.addSoundComponent(soundComponent);
        return soundComponent;
    }

}