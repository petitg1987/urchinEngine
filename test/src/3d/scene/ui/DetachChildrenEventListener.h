#pragma once

#include <Urchin3dEngine.h>

class DetachChildrenEventListener : public urchin::EventListener {
    public:
        explicit DetachChildrenEventListener(urchin::Widget*);

        void onMouseLeftClick(urchin::Widget*) override;

    private:
        urchin::Widget* widget;
};
