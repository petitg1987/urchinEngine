#pragma once

#include <Urchin3dEngine.h>

class FocusEventListener final : public urchin::EventListener {
    public:
        FocusEventListener(bool&, bool&);

        bool onFocus(urchin::Widget*) override;
        bool onFocusLost(urchin::Widget*) override;

    private:
        bool& focused;
        bool& focusLost;
};
