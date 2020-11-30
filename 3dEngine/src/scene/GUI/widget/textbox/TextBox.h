#ifndef URCHINENGINE_TEXTBOX_H
#define URCHINENGINE_TEXTBOX_H

#include <string>
#include <memory>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/GUISkinService.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class TextBox : public Widget {
        public:
            TextBox(Position, Size, std::string );
            ~TextBox() override;

            void createOrUpdateWidget() override;

            std::string getText();

            void display(const RenderTarget*, const ShaderVar&, float) override;

        private:
            bool onKeyPressEvent(unsigned int) override;
            bool onCharEvent(unsigned int) override;
            void reset() override;

            void refreshText(unsigned int);
            void computeCursorPosition();
            void computeCursorIndex(int);

            //properties
            const std::string nameSkin;

            //display information
            Text* text; //text of the text box (widget)
            std::string allText; //text of the text box (string)
            unsigned int maxWidthText; //maximum length of text can be displayed
            unsigned int startTextIndex; //index of the first letter to display
            unsigned int cursorIndex; //index of the cursor
            unsigned int cursorPosition; //position (in pixel) of the cursor
            float cursorBlink;

            //state
            enum textboxStates {
                ACTIVE,
                UNACTIVE
            } state;

            //visual
            std::shared_ptr<Texture> texTextBoxDefault, texTextBoxFocus;
            WidgetOutline* widgetOutline;
            std::shared_ptr<Texture> texCursorDiffuse;
            std::unique_ptr<GenericRenderer> textBoxRenderer, cursorRenderer;
    };

}

#endif
