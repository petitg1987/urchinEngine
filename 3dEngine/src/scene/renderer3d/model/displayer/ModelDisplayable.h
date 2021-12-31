#pragma once

#include <vector>
#include <scene/renderer3d/model/displayer/DisplayMode.h>

namespace urchin {

    class ModelDisplayer;

    class ModelDisplayable {
        public:
            static constexpr std::size_t INSTANCING_DENY_ID = 0;

            void attachModelDisplayer(ModelDisplayer&);
            void detachModelDisplayer(ModelDisplayer&);
            const std::vector<ModelDisplayer*>& getModelDisplayers() const;

            std::size_t computeInstanceId(DisplayMode) const;

        private:
            std::vector<ModelDisplayer*> modelDisplayers;
    };

}