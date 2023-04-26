#pragma once

#include <string>

#include <scene/ui/widget/WidgetType.h>

namespace urchin {

    class WidgetTypeUtil {
        public:
            static std::string toWidgetTypeString(WidgetType);
    };

}