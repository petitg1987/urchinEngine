#pragma once

namespace urchin {

    class Scrollable {
        public:
            virtual ~Scrollable() = default;

            virtual int getScrollShiftY() const = 0;

            virtual void onScrollableContentUpdated() const = 0;
    };

}
