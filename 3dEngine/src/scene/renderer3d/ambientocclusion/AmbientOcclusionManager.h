#ifndef ENGINE_AMBIENTOCCLUSIONMANAGER_H
#define ENGINE_AMBIENTOCCLUSIONMANAGER_H

#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Manager for ambient occlusion (horizon based ambient occlusion - HBAO)
	*/
	class AmbientOcclusionManager
	{
		public:
			AmbientOcclusionManager();
			virtual ~AmbientOcclusionManager();

			void initialize();
			void onResize(int, int);

			void updateAOTexture();
			void loadAOTexture();

		private:
			//scene information
			bool isInitialized;
			int sceneWidth, sceneHeight;

	};

}

#endif
