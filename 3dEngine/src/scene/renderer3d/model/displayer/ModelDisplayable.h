#pragma once

#include <vector>
#include <scene/renderer3d/model/displayer/DisplayMode.h>

namespace urchin {

    class ModelInstanceDisplayer;

    class ModelDisplayable {
        public:
            static constexpr std::size_t INSTANCING_DENY_ID = 0;

            virtual ~ModelDisplayable();

            void attachModelDisplayer(ModelInstanceDisplayer&);
            void detachModelDisplayer(ModelInstanceDisplayer&);
            const std::vector<ModelInstanceDisplayer*>& getModelDisplayers() const;

            std::size_t computeInstanceId(DisplayMode) const;

        private:
            std::vector<ModelInstanceDisplayer*> modelDisplayers;
    };

}