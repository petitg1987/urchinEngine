#pragma once

namespace urchin {
    class Observable;

    class Observer {
        public:
            virtual ~Observer() = default;

            virtual void notify(Observable*, int) = 0;
    };

}
