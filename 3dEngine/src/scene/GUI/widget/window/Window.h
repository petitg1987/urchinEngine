#ifndef URCHINENGINE_WINDOW_H
#define URCHINENGINE_WINDOW_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/GUISkinService.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

    class Window : public Widget
    {
        public:
            Window(Position, Size, std::string , std::string stringTitle = "");

            ~Window() override;

            void createOrUpdateWidget() override;

            void addChild(Widget *) override;

            void display(int, float) override;

        private:
            bool onKeyDownEvent(unsigned int) override;
            bool onKeyUpEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;

            //properties
            const std::string nameSkin;
            const std::string stringTitle;

            //state
            int mousePositionX, mousePositionY;
            enum windowStates
            {
                DEFAULT,
                MOVING,
                CLOSING
            } state;

            //visual
            Text *title;
            std::shared_ptr<Image> texWindow;
            WidgetOutline *widgetOutline;
            std::shared_ptr<QuadDisplayer> quadDisplayer;
    };

}

#endif
