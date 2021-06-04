#pragma once

#include <scene/UI/widget/Widget.h>
#include <scene/UI/widget/Position.h>

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
