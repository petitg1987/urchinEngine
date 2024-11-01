#pragma once

#include <string>
#include <map>

namespace urchin {

    struct ControlData;

    class Control {
        public:
            enum Key {
                //keyboard
                A = 0,
                B,
                C,
                D,
                E,
                F,
                G,
                H,
                I,
                J,
                K,
                L,
                M,
                N,
                O,
                P,
                Q,
                R,
                S,
                T,
                U,
                V,
                W,
                X,
                Y,
                Z,
                K0,
                K1,
                K2,
                K3,
                K4,
                K5,
                K6,
                K7,
                K8,
                K9,
                F1,
                F2,
                F3,
                F4,
                F5,
                F6,
                F7,
                F8,
                F9,
                F10,
                F11,
                F12,
                NUM_PAD_0,
                NUM_PAD_1,
                NUM_PAD_2,
                NUM_PAD_3,
                NUM_PAD_4,
                NUM_PAD_5,
                NUM_PAD_6,
                NUM_PAD_7,
                NUM_PAD_8,
                NUM_PAD_9,
                NUM_PAD_DECIMAL,
                NUM_PAD_DIVIDE,
                NUM_PAD_MULTIPLY,
                NUM_PAD_SUBTRACT,
                NUM_PAD_ADD,
                NUM_PAD_ENTER,
                NUM_PAD_EQUAL,
                EQUAL,
                COLON,
                SEMICOLON,
                COMMA,
                MINUS,
                SLASH,
                BACKSLASH,
                APOSTROPHE,
                LEFT_BRACKET,
                RIGHT_BRACKET,
                PERIOD,
                GRAVE_ACCENT,
                ESCAPE,
                SPACE,
                CTRL_LEFT,
                CTRL_RIGHT,
                ALT_LEFT,
                ALT_RIGHT,
                SHIFT_LEFT,
                SHIFT_RIGHT,
                SUPER_LEFT,
                SUPER_RIGHT,
                MENU,
                ARROW_LEFT,
                ARROW_RIGHT,
                ARROW_UP,
                ARROW_DOWN,
                ENTER,
                TAB,
                BACKSPACE,
                INSERT,
                DEL,
                HOME,
                END,
                PAGE_UP,
                PAGE_DOWN,
                CAPS_LOCK,
                SCROLL_LOCK,
                NUM_LOCK,
                PRINT_SCREEN,
                PAUSE,

                //mouse
                LMB = 1000,
                RMB,
                MMB,
                MOUSE_F1,
                MOUSE_F2,

                UNKNOWN_KEY = 9999
            };

            const std::map<Key, std::string>& getKeysLabelMap() const;

        private:
            static ControlData controlData;
    };

    struct ControlData {
        ControlData() noexcept;
        std::map<Control::Key, std::string> keysLabelMap;
    };

}
