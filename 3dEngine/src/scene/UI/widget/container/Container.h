#ifndef URCHINENGINE_CONTAINER_H
#define URCHINENGINE_CONTAINER_H

#include "scene/UI/widget/Widget.h"
#include "scene/UI/widget/Position.h"

namespace urchin {

    /**
     * Container of widgets
     */
    class Container : public Widget {
        public:
            Container(Widget*, Position);
            explicit Container(Position);
            ~Container() override = default;

        protected:
            void createOrUpdateWidget() override;
            void displayWidget(float) override;
    };

}

#endif
