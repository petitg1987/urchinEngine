#ifndef URCHINENGINE_BUTTON_H
#define URCHINENGINE_BUTTON_H

#include <string>
#include <memory>

#include "scene/UI/widget/Widget.h"
#include "scene/UI/widget/Position.h"
#include "scene/UI/widget/Size.h"
#include "scene/UI/widget/text/Text.h"
#include "resources/image/Image.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class Button : public Widget {
        public:
            Button(Widget*, Position, Size, std::string, std::string);
            Button(Position, Size, std::string, std::string);
            ~Button() override = default;

        protected:
            void createOrUpdateWidget() override;
            void displayWidget(float) override;

        private:
            const std::shared_ptr<Texture>& getTexture();

            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onMouseMoveEvent(int, int) override;

            //properties
            const std::string nameSkin;

            //display information
            Text* text;
            const std::string buttonText;

            //visual
            std::shared_ptr<Texture> texInfoDefault, texInfoOnFocus, texInfoOnClick;
            std::unique_ptr<GenericRenderer> buttonRenderer;
    };

}

#endif
