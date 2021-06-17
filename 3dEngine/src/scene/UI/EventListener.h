#pragma once

namespace urchin {

    class Widget;

    class EventListener {
        public:
            virtual ~EventListener() = default;

            virtual void onMouseLeftClick(Widget*);
            virtual void onMouseLeftClickRelease(Widget*);

            virtual void onFocus(Widget*);
            virtual void onFocusLost(Widget*);

            virtual void onValueChange(Widget*);
    };

}
