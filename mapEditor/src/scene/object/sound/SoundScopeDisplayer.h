#pragma once

#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAggregation.h>

namespace urchin {

    class SoundScopeDisplayer {
        public:
            explicit SoundScopeDisplayer(Scene&);
            ~SoundScopeDisplayer();

            void displaySoundScope(const ObjectEntity*);

        private:
            std::unique_ptr<GeometryModel> retrieveSoundShapeGeometry(const SoundShape&) const;
            void cleanCurrentDisplay();

            Scene& scene;
            std::vector<std::shared_ptr<GeometryModel>> soundModels;
    };

}
