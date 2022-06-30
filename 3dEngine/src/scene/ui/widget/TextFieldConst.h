#pragma once

namespace urchin {

    class TextFieldConst {
        public:
            static constexpr unsigned int TEXT_AND_SCROLLBAR_SHIFT = 4; //define space between the text and the scrollbar
            static constexpr unsigned int LETTER_AND_CURSOR_SHIFT = 2; //define space between the letters and cursor
            static constexpr unsigned int LETTER_SHIFT = 5; //when the widget is full of text, we shift all letters to left/right
            static constexpr unsigned int CURSOR_HEIGHT_MARGIN_PIXEL = 4;
            static constexpr unsigned int CURSOR_WIDTH_PIXEL = 2;
            static constexpr float CURSOR_BLINK_SPEED = 2.25f;
    };

}
