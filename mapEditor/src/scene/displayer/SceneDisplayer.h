#ifndef ENGINE_MAPEDITOR_SCENEDISPLAYER_H
#define ENGINE_MAPEDITOR_SCENEDISPLAYER_H

#include <vector>
#include <QWidget>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"
#include "UrchinMapHandler.h"

#include "scene/displayer/SceneFreeCamera.h"
#include "scene/displayer/objects/BodyShapeDisplayer.h"
#include "scene/displayer/lights/LightScopeDisplayer.h"
#include "scene/displayer/sounds/SoundTriggerDisplayer.h"

namespace urchin
{

	class SceneDisplayer
	{
		public:
			enum ViewProperties
			{
				MODEL_PHYSICS = 0,
				LIGHT_SCOPE,
				SOUND_TRIGGER,

				LAST_VIEW_PROPERTIES
			};

			SceneDisplayer(QWidget *);
			~SceneDisplayer();

			void initializeFromExistingMap(const std::string &, const std::string &);
			void initializeFromNewMap(const std::string &, const std::string &, const std::string &);

			void setViewProperties(SceneDisplayer::ViewProperties, bool);
			void setHighlightSceneObject(const SceneObject *);
			void setHighlightCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape>);
			void setHighlightSceneLight(const SceneLight *);
			void setHighlightSceneSound(const SceneSound *);

			void paint();
			void resize(unsigned int width, unsigned int height);

			SceneManager *getSceneManager() const;
			MapHandler *getMapHandler() const;

		private:
			void initializeEngineResources(const std::string &);
			void initializeScene();

			void refreshRigidBodyShapeModel();
			void refreshLightScopeModel();
			void refreshSoundTriggerModel();

			bool isInitialized;
			QWidget *parentWidget;

			SceneManager *sceneManager;
			SoundManager *soundManager;
			SceneFreeCamera *camera;

			MapHandler *mapHandler;
			BodyShapeDisplayer *bodyShapeDisplayer;
			LightScopeDisplayer *lightScopeDisplayer;
			SoundTriggerDisplayer *soundTriggerDisplayer;

			bool viewProperties[LAST_VIEW_PROPERTIES];
			const SceneObject *highlightSceneObject;
			const SceneLight *highlightSceneLight;
			const SceneSound *highlightSceneSound;
	};

}

#endif
