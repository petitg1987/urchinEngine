#ifndef ENGINE_AMBIENTOCCLUSIONMANAGER_H
#define ENGINE_AMBIENTOCCLUSIONMANAGER_H

#include "UrchinCommon.h"

#include "scene/renderer3d/camera/Camera.h"
#include "utils/display/quad/QuadDisplayer.h"

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

			void initialize(unsigned int, unsigned int);
			void onResize(int, int);
			void createOrUpdateTexture();

			unsigned int getAmbientOcclusionTextureID() const;

			void updateAOTexture(const Camera *const);
			void loadAOTexture(unsigned int) const;

		private:
			//scene information
			bool isInitialized;
			int sceneWidth, sceneHeight;

			//frame buffer object
			unsigned int fboID;
			unsigned int ambientOcclusionTexID;

			//ambient occlusion shader
			unsigned int hbaoShader;
			int mInverseViewProjectionLoc;

			//visual data
			unsigned int depthTexID;
			unsigned int ambienOcclusionTexLoc;
			std::shared_ptr<QuadDisplayer> quadDisplayer;
	};

}

#endif
