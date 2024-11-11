#pragma once

#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAggregation.h>

namespace urchin {

    class SoundTriggerDisplayer {
        public:
            explicit SoundTriggerDisplayer(Scene&);
            ~SoundTriggerDisplayer();

            void displaySoundTrigger(const SoundEntity*);

        private:
            std::unique_ptr<GeometryModel> retrieveGeometry(const SoundShape&) const;

            std::unique_ptr<GeometryModel> retrieveSphereGeometry(const SoundShape&) const;
            std::unique_ptr<GeometryModel> retrieveBoxGeometry(const SoundShape&) const;

            void cleanCurrentDisplay();

            Scene& scene;
            std::vector<std::shared_ptr<GeometryModel>> soundTriggerModels;
    };

}
