#pragma once

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/text/Text.h>

namespace urchin {

    class Textarea : public Widget { //TODO avoid copy/paste (code & constexpr)
        public:
            static std::shared_ptr<Textarea> create(Widget*, Position, Size, std::string);

            WidgetType getWidgetType() const override;

            std::string getText();
            void updateText(std::string_view);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Textarea(Position, Size, std::string);

            bool onKeyPressEvent(unsigned int) override;
            bool onCharEvent(char32_t) override;
            void onResetStateEvent() override;

            bool isCharacterAllowed(char32_t) const;
            bool isMaxCharacterReach() const;

            void refreshText(int, bool);
            void computeCursorPosition();
            void computeCursorIndex(int, int);

            //properties
            static constexpr unsigned int TEXT_SHIFT_Y_PIXEL = 5;
            static constexpr unsigned int LETTER_AND_CURSOR_SHIFT = 2; //define space between the letters and cursor
            static constexpr unsigned int CURSOR_PADDING_PIXEL = 4;
            static constexpr unsigned int CURSOR_WIDTH_PIXEL = 2;
            static constexpr float CURSOR_BLINK_SPEED = 2.25f;
            WStringConvertA stringConvert;
            std::string skinName;
            U32StringA allowedCharacters;
            std::string inputText;
            int maxCharacter;

            //display information
            std::shared_ptr<Text> text; //text of the text box (widget)
            U32StringA allText; //text of the text box (string)
            unsigned int cursorIndex; //index of the cursor
            Vector2<float> cursorPosition; //position (in pixel) of the cursor
            float cursorBlink;

            //state
            enum TextareaStates {
                ACTIVE,
                INACTIVE
            };
            TextareaStates state;

            //visual
            std::shared_ptr<Texture> texTextareaDefault;
            std::shared_ptr<Texture> texTextareaFocus;
            std::shared_ptr<Texture> texCursorAlbedo;
            std::unique_ptr<GenericRenderer> textareaRenderer;
            std::unique_ptr<GenericRenderer> cursorRenderer;
    };

}
