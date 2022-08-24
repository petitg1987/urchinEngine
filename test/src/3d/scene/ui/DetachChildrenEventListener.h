#pragma once

#include <Urchin3dEngine.h>

class DetachChildrenEventListener final : public urchin::EventListener {
    public:
        explicit DetachChildrenEventListener(urchin::Widget*);

        bool onMouseLeftClick(urchin::Widget*) override;

    private:
        urchin::Widget* widget;
};
