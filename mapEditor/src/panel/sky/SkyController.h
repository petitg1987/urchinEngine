#ifndef URCHINENGINE_SKYCONTROLLER_H
#define URCHINENGINE_SKYCONTROLLER_H

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"

#include "panel/AbstractController.h"

namespace urchin
{

    class SkyController : public AbstractController
    {
        public:
            explicit SkyController(MapHandler *);

            const SceneSky *getSceneSky() const;

            const SceneSky *updateSceneSky(const std::vector<std::string> &, float);

        private:
            bool isSkyboxFilenamesAllEmpty(const std::vector<std::string> &) const;
    };

}

#endif
