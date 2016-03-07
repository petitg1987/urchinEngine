#ifndef ENGINE_EVENTLISTENER_H
#define ENGINE_EVENTLISTENER_H

namespace urchin
{

	class Widget;

	class EventListener
	{
		public:
			EventListener();
			virtual ~EventListener();

			virtual void onClick(Widget *);
			virtual void onClickRelease(Widget *);
			virtual void onFocus(Widget *);
			virtual void onFocusLost(Widget *);
	};

}

#endif
