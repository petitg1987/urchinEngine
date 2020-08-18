#ifndef URCHINENGINE_OBSERVER_H
#define URCHINENGINE_OBSERVER_H

namespace urchin
{
    class Observable;

    class Observer
    {
        public:
            virtual ~Observer() = default;

            virtual void notify(Observable *, int) = 0;
    };

}

#endif
