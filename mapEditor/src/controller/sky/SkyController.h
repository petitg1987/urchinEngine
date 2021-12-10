#pragma once

#include <UrchinCommon.h>
#include <UrchinMapHandler.h>

#include <controller/AbstractController.h>

namespace urchin {

    class SkyController : public AbstractController {
        public:
            SkyController();

            const SkyEntity& getSkyEntity() const;

            const SkyEntity& updateSkyEntity(const std::vector<std::string>&, float);

        private:
            bool isSkyboxFilenamesAllEmpty(const std::vector<std::string>&) const;
    };

}
