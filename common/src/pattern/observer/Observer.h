#ifndef ENGINE_OBSERVER_H
#define ENGINE_OBSERVER_H

namespace urchin
{
	class Observable;

	class Observer
	{
		public:
			Observer();
			virtual ~Observer();

			virtual void notify(Observable *, int) = 0;
	};

}

#endif
