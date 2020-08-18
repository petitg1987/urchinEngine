#ifndef URCHINENGINE_CONTAINER_H
#define URCHINENGINE_CONTAINER_H

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"

namespace urchin
{

    /**
     * Container of widgets
     */
    class Container : public Widget
    {
        public:
            explicit Container(Position);
            ~Container() override = default;

            void createOrUpdateWidget() override;
    };

}

#endif
