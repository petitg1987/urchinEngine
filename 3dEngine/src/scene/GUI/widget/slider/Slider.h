#ifndef ENGINE_SLIDER_H
#define ENGINE_SLIDER_H

#include <string>
#include <memory>
#include <vector>

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"
#include "scene/GUI/widget/Size.h"
#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/EventListener.h"
namespace urchin
{

	class Slider : public Widget
	{
		public:
			Slider(Position, Size, const std::vector<std::string> &, const std::string &);
			virtual ~Slider();

			void createOrUpdateWidget();

			unsigned int getSelectedIndex() const;
			void setSelectedIndex(unsigned int);

			void display(int, float);

		private:
			std::string buttonNameSkin, valuesNameSkin;
			std::string leftButtonString, rightButtonString;
			const std::vector<std::string> values;
			unsigned int selectedIndex;

			//visual
			Text *leftButton, *rightButton;
			std::vector<Text *> valuesText;

			class ButtonSliderEventListener : public EventListener
			{
				public:
					ButtonSliderEventListener(Slider *, bool);
					void onClickRelease(Widget *);

				private:
					Slider *slider;
					bool isLeftButton;
			};
	};

}

#endif
