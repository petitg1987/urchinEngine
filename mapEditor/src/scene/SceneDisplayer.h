#pragma once

#include <vector>
#include <QWidget>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinMapHandler.h>

#include <widget/controller/mouse/MouseController.h>
#include <widget/controller/statusbar/StatusBarController.h>
#include <controller/SceneController.h>
#include <scene/SceneFreeCamera.h>
#include <scene/objects/BodyShapeDisplayer.h>
#include <scene/objects/move/ObjectMoveController.h>
#include <scene/lights/LightScopeDisplayer.h>
#include <scene/sounds/SoundTriggerDisplayer.h>

namespace urchin {

    class SceneDisplayerWindow;
    class SceneWindowController;

    class SceneDisplayer {
        public:
            enum ViewProperties {
                MODEL_PHYSICS = 0,
                LIGHT_SCOPE,
                SOUND_TRIGGER,
                NAV_MESH,

                LAST_VIEW_PROPERTIES
            };

            SceneDisplayer(SceneWindowController&, SceneController*, MouseController&, StatusBarController&);
            ~SceneDisplayer();

            void loadMap(const std::string&, const std::string&, const std::string&);
            void loadEmptyScene(const std::string&);

            void setViewProperties(SceneDisplayer::ViewProperties, bool);
            void setHighlightSceneObject(const SceneObject*);
            void setHighlightSceneLight(const SceneLight*);
            void setHighlightSceneSound(const SceneSound*);

            void paint();
            void resize(unsigned int, unsigned int);

            SceneManager& getSceneManager() const;
            SceneFreeCamera* getCamera() const;
            PhysicsWorld& getPhysicsWorld() const;
            MapHandler& getMapHandler() const;

            BodyShapeDisplayer* getBodyShapeDisplayer() const;
            ObjectMoveController* getObjectMoveController() const;

        private:
            static void initializeEngineResources(const std::string&);
            void initializeScene(const std::string&);

            void refreshObjectsModel();
            void refreshLightScopeModel();
            void refreshSoundTriggerModel();
            void refreshNavMeshModel();

            bool isInitialized;
            SceneWindowController& windowController;
            SceneController* sceneController;
            MouseController& mouseController;
            StatusBarController& statusBarController;

            //3d
            std::unique_ptr<SceneManager> sceneManager;
            std::shared_ptr<SceneFreeCamera> camera;
            std::unique_ptr<BodyShapeDisplayer> bodyShapeDisplayer;
            std::unique_ptr<ObjectMoveController> objectMoveController;
            std::unique_ptr<LightScopeDisplayer> lightScopeDisplayer;
            std::unique_ptr<SoundTriggerDisplayer> soundTriggerDisplayer;

            //physics
            std::unique_ptr<PhysicsWorld> physicsWorld;

            //AI
            std::unique_ptr<AIManager> aiManager;
            std::unique_ptr<NavMeshDisplayer> navMeshDisplayer;

            //sound
            std::unique_ptr<SoundManager> soundManager;

            //map handler
            std::unique_ptr<MapHandler> mapHandler;

            //scene specifics
            bool viewProperties[LAST_VIEW_PROPERTIES];
            const SceneObject* highlightSceneObject;
            const SceneLight* highlightSceneLight;
            const SceneSound* highlightSceneSound;
    };

}
