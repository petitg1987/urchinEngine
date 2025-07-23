#pragma once

#include <vector>

#include "scene/renderer3d/model/displayer/DisplayMode.h"

namespace urchin {

    class ModelInstanceDisplayer;

    class ModelDisplayable {
        public:
            static constexpr std::size_t INSTANCING_DENY_ID = 0;

            virtual ~ModelDisplayable();

            void attachModelInstanceDisplayer(ModelInstanceDisplayer&);
            void detachModelInstanceDisplayer(ModelInstanceDisplayer&);
            std::span<ModelInstanceDisplayer* const> getModelInstanceDisplayers() const;

            std::size_t computeInstanceId(DisplayMode) const;

        private:
            std::vector<ModelInstanceDisplayer*> modelInstanceDisplayers;
    };

}