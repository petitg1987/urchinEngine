#ifndef ENGINE_DEBUGVISUALIZER_SCENEDISPLAYER_H
#define ENGINE_DEBUGVISUALIZER_SCENEDISPLAYER_H

#include <memory>
#include <vector>
#include <QWidget>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

#include "scene/displayer/SceneFreeCamera.h"
#include "scene/entity/GeometryEntityHandler.h"

namespace urchin
{

	class SceneDisplayer
	{
		public:
			SceneDisplayer(QWidget *);
			~SceneDisplayer();

			 void initializeForOpen(const std::string &, std::shared_ptr<GeometryEntityHandler>);

			void paint();
			void resize(unsigned int width, unsigned int height);

			SceneManager *getSceneManager() const;

		private:
			void initializeEngineResources(const std::string &);
			void initializeScene();

			bool isInitialized;
			QWidget *parentWidget;

			std::shared_ptr<GeometryEntityHandler> geometryEntityHandler;

			SceneManager *sceneManager;
			SceneFreeCamera *camera;
	};

}

#endif
