#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>

namespace urchin {

    /**
     * Container of widgets
     */
    class Container : public Widget {
        public:
            static std::shared_ptr<Container> newContainer(Widget*, Position);
            static std::shared_ptr<Container> newContainer(Widget*, Position, Size);

        protected:
            Container(Position, Size);

            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;
    };

}
