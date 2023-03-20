#include <system/control/Control.h>

namespace urchin {

    //static
    ControlData Control::controlData;

    const std::map<Control::Key, std::string>& Control::getKeysLabelMap() const {
        return controlData.keysLabelMap;
    }

    ControlData::ControlData() noexcept {
        //keyboard
        for (unsigned int i = Control::Key::A; i <= Control::Key::Z; ++i) {
            keysLabelMap[static_cast<Control::Key>(i)] = std::string(1, static_cast<char>('A' + i));
        }
        for (unsigned int i = Control::Key::K0; i <= Control::Key::K9; ++i) {
            keysLabelMap[static_cast<Control::Key>(i)] = std::to_string(i - Control::Key::K0);
        }
        for (unsigned int i = Control::Key::F1; i <= Control::Key::F12; ++i) {
            keysLabelMap[static_cast<Control::Key>(i)] = "F" + std::to_string(i - Control::Key::F1 + 1);
        }
        for (unsigned int i = Control::Key::NUM_PAD_0; i <= Control::Key::NUM_PAD_9; ++i) {
            keysLabelMap[static_cast<Control::Key>(i)] = "Num " + std::to_string(i - Control::Key::NUM_PAD_0);
        }
        keysLabelMap[Control::Key::NUM_PAD_DECIMAL] = "Num .";
        keysLabelMap[Control::Key::NUM_PAD_DIVIDE] = "Num /";
        keysLabelMap[Control::Key::NUM_PAD_MULTIPLY] = "Num *";
        keysLabelMap[Control::Key::NUM_PAD_SUBTRACT] = "Num -";
        keysLabelMap[Control::Key::NUM_PAD_ADD] = "Num +";
        keysLabelMap[Control::Key::NUM_PAD_ENTER] = "Num Enter";
        keysLabelMap[Control::Key::NUM_PAD_EQUAL] = "Num =";
        keysLabelMap[Control::Key::EQUAL] = "=";
        keysLabelMap[Control::Key::COLON] = ":";
        keysLabelMap[Control::Key::SEMICOLON] = ";";
        keysLabelMap[Control::Key::COMMA] = ",";
        keysLabelMap[Control::Key::MINUS] = "-";
        keysLabelMap[Control::Key::SLASH] = "/";
        keysLabelMap[Control::Key::BACKSLASH] = "\\";
        keysLabelMap[Control::Key::APOSTROPHE] = "'";
        keysLabelMap[Control::Key::LEFT_BRACKET] = "[";
        keysLabelMap[Control::Key::RIGHT_BRACKET] = "]";
        keysLabelMap[Control::Key::PERIOD] = ".";
        keysLabelMap[Control::Key::GRAVE_ACCENT] = '`';
        keysLabelMap[Control::Key::ESCAPE] = "Esc";
        keysLabelMap[Control::Key::SPACE] = "Space";
        keysLabelMap[Control::Key::CTRL_LEFT] = "Ctrl L";
        keysLabelMap[Control::Key::CTRL_RIGHT] = "Ctrl R";
        keysLabelMap[Control::Key::ALT_LEFT] = "Alt L";
        keysLabelMap[Control::Key::ALT_RIGHT] = "Alt R";
        keysLabelMap[Control::Key::SHIFT_LEFT] = "Shift L";
        keysLabelMap[Control::Key::SHIFT_RIGHT] = "Shift R";
        keysLabelMap[Control::Key::SUPER_LEFT] = "Super L";
        keysLabelMap[Control::Key::SUPER_RIGHT] = "Super R";
        keysLabelMap[Control::Key::MENU] = "Menu";
        keysLabelMap[Control::Key::ARROW_LEFT] = "Left";
        keysLabelMap[Control::Key::ARROW_RIGHT] = "Right";
        keysLabelMap[Control::Key::ARROW_UP] = "Up";
        keysLabelMap[Control::Key::ARROW_DOWN] = "Down";
        keysLabelMap[Control::Key::ENTER] = "Enter";
        keysLabelMap[Control::Key::TAB] = "Tab";
        keysLabelMap[Control::Key::BACKSPACE] = "Backspace";
        keysLabelMap[Control::Key::INSERT] = "Insert";
        keysLabelMap[Control::Key::DEL] = "Delete";
        keysLabelMap[Control::Key::HOME] = "Home";
        keysLabelMap[Control::Key::END] = "End";
        keysLabelMap[Control::Key::PAGE_UP] = "Page Up";
        keysLabelMap[Control::Key::PAGE_DOWN] = "Page Down";
        keysLabelMap[Control::Key::CAPS_LOCK] = "Caps lock";
        keysLabelMap[Control::Key::SCROLL_LOCK] = "Scroll lock";
        keysLabelMap[Control::Key::NUM_LOCK] = "Num lock";
        keysLabelMap[Control::Key::PRINT_SCREEN] = "Ptr scr";
        keysLabelMap[Control::Key::PAUSE] = "Pause";

        //mouse
        keysLabelMap[Control::Key::LMB] = "Mouse Left";
        keysLabelMap[Control::Key::RMB] = "Mouse Right";
        keysLabelMap[Control::Key::MMB] = "Mouse Wheel";
        keysLabelMap[Control::Key::MOUSE_F1] = "Mouse F1";
        keysLabelMap[Control::Key::MOUSE_F2] = "Mouse F2";
        keysLabelMap[Control::Key::MOUSE_F3] = "Mouse F3";
        keysLabelMap[Control::Key::MOUSE_F4] = "Mouse F4";
        keysLabelMap[Control::Key::MOUSE_F5] = "Mouse F5";

        keysLabelMap[Control::Key::UNKNOWN_KEY] = "[UNKNOWN]";
    }
    
}
