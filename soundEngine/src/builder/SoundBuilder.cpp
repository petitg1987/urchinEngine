#include <builder/SoundBuilder.h>

namespace urchin {

    SoundBuilder::SoundBuilder(SoundEnvironment& soundEnvironment) :
            soundEnvironment(soundEnvironment) {

    }

    std::unique_ptr<SoundWrapper<GlobalSound, ManualTrigger>> SoundBuilder::newManualEffect(std::string filename, PlayBehavior playBehavior) {
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<SoundWrapper<GlobalSound, ManualTrigger>>(soundEnvironment, std::move(globalSound), std::move(manualTrigger));
    }

    std::unique_ptr<SoundWrapper<GlobalSound, ManualTrigger>> SoundBuilder::newManualMusic(std::string filename, PlayBehavior playBehavior){
        auto globalSound = std::make_shared<GlobalSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<SoundWrapper<GlobalSound, ManualTrigger>>(soundEnvironment, std::move(globalSound), std::move(manualTrigger));
    }

    std::unique_ptr<SoundWrapper<SpatialSound, ManualTrigger>> SoundBuilder::newManualSpatialEffect(std::string filename, const Point3<float>& position,
                                                                                                    float inaudibleDistance, PlayBehavior playBehavior) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, inaudibleDistance);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<SoundWrapper<SpatialSound, ManualTrigger>>(soundEnvironment, std::move(spatialSound), std::move(manualTrigger));
    }

    std::unique_ptr<SoundWrapper<SpatialSound, ManualTrigger>> SoundBuilder::newManualSpatialMusic(std::string filename, const Point3<float>& position,
                                                                                                   float inaudibleDistance, PlayBehavior playBehavior) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, inaudibleDistance);
        auto manualTrigger = std::make_shared<ManualTrigger>(playBehavior);
        return std::make_unique<SoundWrapper<SpatialSound, ManualTrigger>>(soundEnvironment, std::move(spatialSound), std::move(manualTrigger));
    }

    std::unique_ptr<SoundWrapper<SpatialSound, ShapeTrigger>> SoundBuilder::newSpatialEffect(std::string filename, const Point3<float>& position,
                                                                                             float inaudibleDistance, std::shared_ptr<ShapeTrigger> shapeTrigger) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::EFFECTS, 1.0f, position, inaudibleDistance);
        return std::make_unique<SoundWrapper<SpatialSound, ShapeTrigger>>(soundEnvironment, std::move(spatialSound), std::move(shapeTrigger));
    }

    std::unique_ptr<SoundWrapper<SpatialSound, ShapeTrigger>> SoundBuilder::newSpatialMusic(std::string filename, const Point3<float>& position,
                                                                                            float inaudibleDistance, std::shared_ptr<ShapeTrigger> shapeTrigger) {
        auto spatialSound = std::make_shared<SpatialSound>(std::move(filename), Sound::SoundCategory::MUSIC, 1.0f, position, inaudibleDistance);
        return std::make_unique<SoundWrapper<SpatialSound, ShapeTrigger>>(soundEnvironment, std::move(spatialSound), std::move(shapeTrigger));
    }

}