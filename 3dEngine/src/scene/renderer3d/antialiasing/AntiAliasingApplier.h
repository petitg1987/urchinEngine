#ifndef ENGINE_ANTIALIASINGAPPLIER_H
#define ENGINE_ANTIALIASINGAPPLIER_H

#include <memory>

#include "utils/display/quad/QuadDisplayer.h"

namespace urchin
{

	class AntiAliasingApplier
	{
		public:
			enum Quality
			{
				LOW = 0,
				MEDIUM,
				HIGH,
				VERY_HIGH
			};

			AntiAliasingApplier();
			~AntiAliasingApplier();

			void initialize();
			void onResize(int, int);

			void setQuality(Quality quality);

			void applyOn(unsigned int);
		private:
			void loadFxaaShader();

			bool isInitialized;

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
