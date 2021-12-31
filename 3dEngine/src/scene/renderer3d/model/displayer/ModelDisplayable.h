#pragma once

#include <vector>
#include <scene/renderer3d/model/displayer/DisplayMode.h>

namespace urchin {

    class ModelDisplayer;

    class ModelDisplayable {
        public:
            void attachModelDisplayer(ModelDisplayer&);
            void detachModelDisplayer(ModelDisplayer&);

            std::size_t computeInstanceId(DisplayMode) const;

        private:
            std::vector<ModelDisplayer*> modelDisplayers;
    };

}