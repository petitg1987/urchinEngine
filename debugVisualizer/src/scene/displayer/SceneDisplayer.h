#ifndef ENGINE_DEBUGVISUALIZER_SCENEDISPLAYER_H
#define ENGINE_DEBUGVISUALIZER_SCENEDISPLAYER_H

#include <vector>
#include <QWidget>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"
#include "UrchinMapHandler.h"

#include "scene/displayer/SceneFreeCamera.h"

namespace urchin
{

	class SceneDisplayer
	{
		public:
			SceneDisplayer(QWidget *);
			~SceneDisplayer();

			void initializeFromDebugFile(const std::string &, const std::string &);
			void initializeEngineResources(const std::string &);
			void initializeScene();

			void paint();
			void resize(unsigned int width, unsigned int height);

			SceneManager *getSceneManager() const;

		private:
			bool isInitialized;
			QWidget *parentWidget;

			SceneManager *sceneManager;
			SceneFreeCamera *camera;
	};

}

#endif
