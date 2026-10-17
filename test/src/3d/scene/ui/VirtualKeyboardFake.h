#pragma once

#include <Urchin3dEngine.h>

class VirtualKeyboardFake final : public urchin::VirtualKeyboard {
    public:
        explicit VirtualKeyboardFake(bool&);

        void show(urchin::VirtualKeyboardType, const std::optional<urchin::Rectangle2D<int>>&) override;
        void hide() override;

    private:
        bool& keyboardVisible;
};
