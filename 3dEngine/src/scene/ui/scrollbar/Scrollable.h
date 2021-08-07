#pragma once

namespace urchin {

    class Scrollable {
        public:
            virtual int getScrollShiftY() const = 0;

            virtual void onScrollableContentUpdated() const = 0;
    };

}
