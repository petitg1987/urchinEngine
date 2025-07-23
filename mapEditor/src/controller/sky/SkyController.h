#pragma once

#include <UrchinCommon.h>
#include <UrchinAggregation.h>

#include "controller/AbstractController.h"

namespace urchin {

    class SkyController final : public AbstractController {
        public:
            const SkyEntity& getSkyEntity() const;

            void updateSkyEntity(const std::vector<std::string>&, float);

        private:
            bool isSkyboxFilenamesAllEmpty(const std::vector<std::string>&) const;
    };

}
