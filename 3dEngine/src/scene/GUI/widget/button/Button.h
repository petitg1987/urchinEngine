#ifndef URCHINENGINE_BUTTON_H
#define URCHINENGINE_BUTTON_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/widget/text/Text.h"
#include "resources/image/Image.h"
#include "utils/display/quad/QuadDisplayer.h"

namespace urchin {

    class Button : public Widget {
        public:
            Button(Position, Size, std::string , std::string );
            ~Button() override = default;

            void createOrUpdateWidget() override;

            void display(int, float) override;

        private:
            unsigned int getTextureId();

            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;

            //properties
            const std::string nameSkin;

            //display information
            Text *text;
            const std::string buttonText;

            //visual
            unsigned int textureID;
            std::shared_ptr<Image> texInfoDefault, texInfoOnFocus, texInfoOnClick;
            std::shared_ptr<QuadDisplayer> quadDisplayer;
    };

}

#endif
