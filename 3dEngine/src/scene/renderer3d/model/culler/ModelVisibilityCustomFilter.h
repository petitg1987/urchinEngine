#pragma once

#include "scene/renderer3d/model/Model.h"

namespace urchin {

    class ModelVisibilityCustomFilter {
        public:
            virtual ~ModelVisibilityCustomFilter() = default;

            virtual bool isVisible(const Model&) const = 0;
    };

}
