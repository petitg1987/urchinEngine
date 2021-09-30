#pragma once

namespace urchin {

    class Widget;

    class EventListener {
        public:
            virtual ~EventListener() = default;

            virtual bool onMouseLeftClick(Widget*);
            virtual bool onMouseLeftClickRelease(Widget*);

            virtual bool onFocus(Widget*);
            virtual bool onFocusLost(Widget*);

            virtual void onValueChange(Widget*);
    };

}
