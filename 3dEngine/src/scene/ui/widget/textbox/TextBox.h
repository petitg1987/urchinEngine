#pragma once

#include <string>
#include <memory>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <api/render/GenericRenderer.h>

namespace urchin {

    class TextBox : public Widget {
        public:
            static std::shared_ptr<TextBox> create(Widget*, Position, Size, std::string);

            WidgetType getWidgetType() const override;

            std::string getText();
            const Text& getTextWidget() const;
            void updateText(std::string_view);

            void setAllowedCharacters(const std::string&);
            void setMaxCharacter(unsigned int);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            TextBox(Position, Size, std::string);

            bool onKeyPressEvent(unsigned int) override;
            bool onCharEvent(char32_t) override;
            void onResetStateEvent() override;

            bool isCharacterAllowed(char32_t) const;
            bool isMaxCharacterReach() const;

            void refreshText(bool);
            void refreshCursorPosition();
            void computeCursorIndex(int, int);

            //properties
            WStringConvertA stringConvert;
            const std::string skinName;
            U32StringA allowedCharacters;
            int maxCharacter;

            //display information
            std::shared_ptr<Text> text;
            U32StringA originalText; //original text of the text box (string)
            unsigned int maxWidthText; //maximum length of text can be displayed
            unsigned int startTextIndex; //index of the first letter to display
            std::shared_ptr<StaticBitmap> cursor;
            unsigned int cursorIndex; //index of the cursor
            Vector2<int> cursorPosition; //position (in pixel) of the cursor
            float cursorBlink;

            //state
            enum TextBoxStates {
                ACTIVE,
                INACTIVE
            };
            TextBoxStates state;

            //visual
            std::shared_ptr<Texture> texTextBoxDefault;
            std::shared_ptr<Texture> texTextBoxFocus;
            std::shared_ptr<Texture> texCursorAlbedo;
            std::unique_ptr<GenericRenderer> textBoxRenderer;
    };

}
