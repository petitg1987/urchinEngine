#pragma once

#include <string>
#include <memory>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/widget/text/Text.h>
#include <api/render/GenericRenderer.h>

namespace urchin {

    class TextBox : public Widget {
        public:
            static std::shared_ptr<TextBox> create(Widget*, Position, Size, std::string);

            std::string getText();
            void updateText(std::string_view);

            void setAllowedCharacters(const std::string&);
            void setMaxLength(unsigned int);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            TextBox(Position, Size, std::string);

            bool onKeyPressEvent(unsigned int) override;
            bool onCharEvent(char32_t) override;
            void onResetStateEvent() override;

            bool isCharacterAllowed(char32_t) const;
            bool isMaxLengthReach() const;

            void refreshText(int, bool);
            void computeCursorPosition();
            void computeCursorIndex(int);

            //properties
            static constexpr unsigned int LETTER_SHIFT = 5; //when the text box is full of text, we shift all letters to left
            static constexpr unsigned int LETTER_AND_CURSOR_SHIFT = 2; //define space between the letters and cursor
            static constexpr unsigned int CURSOR_WIDTH_PIXEL = 2;
            static constexpr float CURSOR_BLINK_SPEED = 2.25f;
            WStringConvertA stringConvert;
            const std::string skinName;
            U32StringA allowedCharacters;
            int maxLength;

            //display information
            std::shared_ptr<Text> text; //text of the text box (widget)
            U32StringA allText; //text of the text box (string)
            unsigned int maxWidthText; //maximum length of text can be displayed
            unsigned int startTextIndex; //index of the first letter to display
            unsigned int cursorIndex; //index of the cursor
            unsigned int cursorPosition; //position (in pixel) of the cursor
            float cursorBlink;

            //state
            enum TextBoxStates {
                ACTIVE,
                INACTIVE
            } state;

            //visual
            std::shared_ptr<Texture> texTextBoxDefault;
            std::shared_ptr<Texture> texTextBoxFocus;
            std::shared_ptr<Texture> texCursorDiffuse;
            std::unique_ptr<GenericRenderer> textBoxRenderer;
            std::unique_ptr<GenericRenderer> cursorRenderer;
    };

}
