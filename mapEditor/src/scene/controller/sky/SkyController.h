#ifndef URCHINENGINE_SKYCONTROLLER_H
#define URCHINENGINE_SKYCONTROLLER_H

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"

namespace urchin
{

    class SkyController
    {
        public:
            explicit SkyController(MapHandler *);

            bool isModified() const;
            void resetModified();

            const SceneSky *getSceneSky() const;

            const SceneSky *updateSceneSky(const std::vector<std::string> &);

        private:
            void markModified();

            bool bIsModified;
            MapHandler *mapHandler;
    };

}

#endif
