#ifndef URCHINENGINE_SOUNDTRIGGERDISPLAYER_H
#define URCHINENGINE_SOUNDTRIGGERDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"
#include "UrchinMapHandler.h"

namespace urchin {

    class SoundTriggerDisplayer {
        public:
            explicit SoundTriggerDisplayer(SceneManager *);
            ~SoundTriggerDisplayer();

            void displaySoundTriggerFor(const SceneSound*);

        private:
            GeometryModel *retrieveGeometry(const SoundShape*);

            GeometryModel *retrieveSphereGeometry(const SoundShape*) const;
            GeometryModel *retrieveBoxGeometry(const SoundShape*) const;

            void cleanCurrentDisplay();

            SceneManager *sceneManager;
            std::vector<GeometryModel *> soundTriggerModels;
    };

}

#endif
