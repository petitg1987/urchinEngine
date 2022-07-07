#pragma once

#include <UrchinCommon.h>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/container/Container.h>

namespace urchin {

    class Textarea : public Widget {
        public:
            static std::shared_ptr<Textarea> create(Widget*, Position, Size, std::string);

            WidgetType getWidgetType() const override;

            std::string getText() const;
            const Text& getTextWidget() const;
            void updateText(std::string_view);

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Textarea(Position, Size, std::string);

            bool onKeyPressEvent(unsigned int) override;
            bool onKeyReleaseEvent(unsigned int) override;
            bool onCharEvent(char32_t) override;
            bool onMouseMoveEvent(int, int) override;
            void onResetStateEvent() override;

            bool isCharacterAllowed(char32_t) const;
            bool isMaxCharacterReach() const;

            void refreshText(bool);
            Point2<int> computeCursorPosition(std::size_t, WordCutIndexPositioning = WordCutIndexPositioning::BEGIN_OF_NEXT_LINE);
            void adjustScrollToCursor() const;
            std::size_t computeCursorIndex(int, int) const;

            void clearSelection();
            void displaySelection();
            std::size_t computeEndOfLineIndex(std::size_t) const;

            //properties
            mutable WStringConvertA stringConvert;
            std::string skinName;
            U32StringA allowedCharacters;
            std::string inputText;
            int maxCharacter;

            //display information
            std::shared_ptr<Container> textContainer;
            float scrollbarWidthInPixel;
            std::shared_ptr<Text> text;
            U32StringA originalText; //original text of the textarea
            std::shared_ptr<StaticBitmap> cursor;
            std::size_t cursorIndex; //index of the cursor in 'originalText'
            Point2<int> cursorPosition; //position (in pixel) of the cursor in 'text'
            float cursorBlink;

            //selection
            bool selectModeOn;
            std::size_t selectionStartIndex; //index of the cursor in 'originalText'
            std::shared_ptr<Texture> selectionTexture;
            std::vector<std::shared_ptr<StaticBitmap>> selectionImgs;

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
    };

}
