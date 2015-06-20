#ifndef ENGINE_BUTTONCOMMAND_H
#define ENGINE_BUTTONCOMMAND_H

namespace urchin
{

	class ButtonCommand
	{
		public:
			ButtonCommand();
			virtual ~ButtonCommand();

			virtual void onClick() = 0;
	};

}

#endif
