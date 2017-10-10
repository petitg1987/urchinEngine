#ifndef URCHINENGINE_EVENTLISTENER_H
#define URCHINENGINE_EVENTLISTENER_H

namespace urchin
{

	class Widget;

	class EventListener
	{
		public:
			virtual ~EventListener() = default;

			virtual void onClick(Widget *);
			virtual void onClickRelease(Widget *);
			virtual void onFocus(Widget *);
			virtual void onFocusLost(Widget *);
	};

}

#endif
