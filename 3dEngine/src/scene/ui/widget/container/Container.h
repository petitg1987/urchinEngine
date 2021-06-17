#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>

namespace urchin {

    /**
     * Container of widgets
     */
    class Container : public Widget {
        public:
            Container(Widget*, Position);
            ~Container() override = default;

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;
    };

}
