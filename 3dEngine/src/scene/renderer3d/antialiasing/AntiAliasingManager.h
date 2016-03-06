#ifndef ENGINE_ANTIALIASINGMANAGER_H
#define ENGINE_ANTIALIASINGMANAGER_H

#include <memory>

#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class AntiAliasingManager
	{
		public:
			enum Quality
			{
				LOW = 0,
				MEDIUM,
				HIGH,
				VERY_HIGH
			};

			AntiAliasingManager();
			~AntiAliasingManager();

			void onResize(int, int);

			void setQuality(Quality quality);

			void applyOn(unsigned int);
		private:
			void loadFxaaShader();

			//properties
			Quality quality;
			int sceneWidth, sceneHeight;

			//shader
			unsigned int fxaaShader;
			int texLoc, invSceneSizeLoc;

			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};

}

#endif
