#ifndef ENGINE_GUIMANAGER_H
#define ENGINE_GUIMANAGER_H

#include <set>
#include "UrchinCommon.h"

#include "scene/GUI/Widget.h"

namespace urchin
{
	
	class GUIManager : public Observable, public Observer
	{
		public:
			GUIManager();
			virtual ~GUIManager();

			enum NotificationType
			{
				ADD_WIDGET, //A widget has been added
				REMOVE_WIDGET, //A widget has been removed
			};

			void initialize();
			void onResize(int, int);
			void notify(Observable *, int);
			void setupSkin(const std::string &);

			Widget *getLastUpdatedWidget();

			void onKeyDown(unsigned int);
			void onKeyUp(unsigned int);
			void onChar(unsigned int);
			void onMouseMove(int, int);
			
			void addWidget(Widget *);
			void removeWidget(Widget *);

			void display(float);
			
		private:
			void onWidgetEvent(Widget *const, NotificationType);
		
			bool isInitialized;
			int width, height;

			std::vector<Widget *> widgets;
			Widget *lastUpdatedWidget;

			Matrix3<float> mProjection, mModelView;
			unsigned int GUIShader;
			int mProjectionLoc, translateDistanceLoc, diffuseTexSamplerLoc;

			bool stopPropagateEvents;
	};
	
}

#endif
