#include <builder/SoundBuilder.h>

namespace urchin {

    SoundBuilder::SoundBuilder(SoundEnvironment& soundEnvironment) :
            soundEnvironment(soundEnvironment) {

    }

    std::unique_ptr<ManualSound> SoundBuilder::newManualEffect(std::string filename, PlayBehavior playBehavior) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<ManualSound>(soundEnvironment, std::move(globalSound), std::move(manualTrigger));
    }

    std::unique_ptr<ManualSound> SoundBuilder::newManualMusic(std::string filename, PlayBehavior playBehavior){
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<ManualSound>(soundEnvironment, std::move(globalSound), std::move(manualTrigger));
    }

    std::unique_ptr<ManualSpatialSound> SoundBuilder::newManualSpatialEffect(std::string filename, const Point3<float>& position, float inaudibleDistance, PlayBehavior playBehavior) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, inaudibleDistance);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<ManualSpatialSound>(soundEnvironment, std::move(spatialSound), std::move(manualTrigger));
    }

    std::unique_ptr<ManualSpatialSound> SoundBuilder::newManualSpatialMusic(std::string filename, const Point3<float>& position, float inaudibleDistance, PlayBehavior playBehavior) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, inaudibleDistance);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<ManualSpatialSound>(soundEnvironment, std::move(spatialSound), std::move(manualTrigger));
    }

    std::unique_ptr<ZoneSound> SoundBuilder::newZoneEffect(std::string filename, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        return std::make_unique<ZoneSound>(soundEnvironment, std::move(globalSound), std::move(zoneTrigger));
    }

    std::unique_ptr<ZoneSound> SoundBuilder::newZoneMusic(std::string filename, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        return std::make_unique<ZoneSound>(soundEnvironment, std::move(globalSound), std::move(zoneTrigger));
    }

    std::unique_ptr<ZoneSpatialSound> SoundBuilder::newZoneSpatialEffect(std::string filename, const Point3<float>& position, float inaudibleDistance, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, inaudibleDistance);
        return std::make_unique<ZoneSpatialSound>(soundEnvironment, std::move(spatialSound), std::move(zoneTrigger));
    }

    std::unique_ptr<ZoneSpatialSound> SoundBuilder::newZoneSpatialMusic(std::string filename, const Point3<float>& position, float inaudibleDistance, std::shared_ptr<ZoneTrigger> zoneTrigger) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, inaudibleDistance);
        return std::make_unique<ZoneSpatialSound>(soundEnvironment, std::move(spatialSound), std::move(zoneTrigger));
    }

}