#include "system/input/InputKeyLabel.h"

namespace urchin {

    const std::map<InputKey, std::string>& InputKeyLabel::getLabels() {
        static const std::map<InputKey, std::string> labels = buildLabels();
        return labels;
    }

    std::map<InputKey, std::string> InputKeyLabel::buildLabels() {
        std::map<InputKey, std::string> keysLabelMap;

        //keyboard
        for (auto i = (unsigned int)InputKey::A; i <= (unsigned int)InputKey::Z; ++i) {
            keysLabelMap[static_cast<InputKey>(i)] = std::string(1, static_cast<char>('A' + i));
        }
        for (auto i = (unsigned int)InputKey::K0; i <= (unsigned int)InputKey::K9; ++i) {
            keysLabelMap[static_cast<InputKey>(i)] = std::to_string(i - (unsigned int)InputKey::K0);
        }
        for (auto i = (unsigned int)InputKey::F1; i <= (unsigned int)InputKey::F12; ++i) {
            keysLabelMap[static_cast<InputKey>(i)] = "F" + std::to_string(i - (unsigned int)InputKey::F1 + 1);
        }
        for (auto i = (unsigned int)InputKey::NUM_PAD_0; i <= (unsigned int)InputKey::NUM_PAD_9; ++i) {
            keysLabelMap[static_cast<InputKey>(i)] = "Num " + std::to_string(i - (unsigned int)InputKey::NUM_PAD_0);
        }
        keysLabelMap[InputKey::NUM_PAD_DECIMAL] = "Num .";
        keysLabelMap[InputKey::NUM_PAD_DIVIDE] = "Num /";
        keysLabelMap[InputKey::NUM_PAD_MULTIPLY] = "Num *";
        keysLabelMap[InputKey::NUM_PAD_SUBTRACT] = "Num -";
        keysLabelMap[InputKey::NUM_PAD_ADD] = "Num +";
        keysLabelMap[InputKey::NUM_PAD_ENTER] = "Num Enter";
        keysLabelMap[InputKey::NUM_PAD_EQUAL] = "Num =";
        keysLabelMap[InputKey::EQUAL] = "=";
        keysLabelMap[InputKey::COLON] = ":";
        keysLabelMap[InputKey::SEMICOLON] = ";";
        keysLabelMap[InputKey::COMMA] = ",";
        keysLabelMap[InputKey::MINUS] = "-";
        keysLabelMap[InputKey::SLASH] = "/";
        keysLabelMap[InputKey::BACKSLASH] = "\\";
        keysLabelMap[InputKey::APOSTROPHE] = "'";
        keysLabelMap[InputKey::LEFT_BRACKET] = "[";
        keysLabelMap[InputKey::RIGHT_BRACKET] = "]";
        keysLabelMap[InputKey::PERIOD] = ".";
        keysLabelMap[InputKey::GRAVE_ACCENT] = '`';
        keysLabelMap[InputKey::ESCAPE] = "Esc";
        keysLabelMap[InputKey::SPACE] = "Space";
        keysLabelMap[InputKey::CTRL_LEFT] = "Ctrl";
        keysLabelMap[InputKey::CTRL_RIGHT] = "Ctrl (right)";
        keysLabelMap[InputKey::ALT_LEFT] = "Alt";
        keysLabelMap[InputKey::ALT_RIGHT] = "Alt (right)";
        keysLabelMap[InputKey::SHIFT_LEFT] = "Shift";
        keysLabelMap[InputKey::SHIFT_RIGHT] = "Shift (right)";
        keysLabelMap[InputKey::SUPER_LEFT] = "Super";
        keysLabelMap[InputKey::SUPER_RIGHT] = "Super (right)";
        keysLabelMap[InputKey::MENU] = "Menu";
        keysLabelMap[InputKey::ARROW_LEFT] = "Left";
        keysLabelMap[InputKey::ARROW_RIGHT] = "Right";
        keysLabelMap[InputKey::ARROW_UP] = "Up";
        keysLabelMap[InputKey::ARROW_DOWN] = "Down";
        keysLabelMap[InputKey::ENTER] = "Enter";
        keysLabelMap[InputKey::TAB] = "Tab";
        keysLabelMap[InputKey::BACKSPACE] = "Backspace";
        keysLabelMap[InputKey::INSERT] = "Insert";
        keysLabelMap[InputKey::DEL] = "Delete";
        keysLabelMap[InputKey::HOME] = "Home";
        keysLabelMap[InputKey::END] = "End";
        keysLabelMap[InputKey::PAGE_UP] = "Page Up";
        keysLabelMap[InputKey::PAGE_DOWN] = "Page Down";
        keysLabelMap[InputKey::CAPS_LOCK] = "Caps lock";
        keysLabelMap[InputKey::SCROLL_LOCK] = "Scroll lock";
        keysLabelMap[InputKey::NUM_LOCK] = "Num lock";
        keysLabelMap[InputKey::PRINT_SCREEN] = "Ptr scr";
        keysLabelMap[InputKey::PAUSE] = "Pause";

        //mouse
        keysLabelMap[InputKey::LMB] = "Mouse Left";
        keysLabelMap[InputKey::RMB] = "Mouse Right";
        keysLabelMap[InputKey::MMB] = "Mouse Wheel";
        keysLabelMap[InputKey::MOUSE_F1] = "Mouse F1";
        keysLabelMap[InputKey::MOUSE_F2] = "Mouse F2";

        keysLabelMap[InputKey::UNKNOWN_KEY] = "[UNKNOWN]";

        return keysLabelMap;
    }
    
}
