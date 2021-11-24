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
#include <scene/models/BodyShapeDisplayer.h>
#include <scene/models/move/ModelMoveController.h>
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
            void setHighlightSceneModel(const SceneModel*);
            void setHighlightSceneLight(const SceneLight*);
            void setHighlightSceneSound(const SceneSound*);

            void paint();
            void resize(unsigned int, unsigned int);

            Scene& getScene() const;
            SceneFreeCamera* getCamera() const;
            PhysicsWorld& getPhysicsWorld() const;
            MapHandler& getMapHandler() const;

            BodyShapeDisplayer* getBodyShapeDisplayer() const;
            ModelMoveController* getModelMoveController() const;

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
            std::unique_ptr<Scene> scene;
            std::shared_ptr<SceneFreeCamera> camera;
            std::unique_ptr<BodyShapeDisplayer> bodyShapeDisplayer;
            std::unique_ptr<ModelMoveController> modelMoveController;
            std::unique_ptr<LightScopeDisplayer> lightScopeDisplayer;
            std::unique_ptr<SoundTriggerDisplayer> soundTriggerDisplayer;

            //physics
            std::unique_ptr<PhysicsWorld> physicsWorld;

            //AI
            std::unique_ptr<AIEnvironment> aiEnvironment;
            std::unique_ptr<NavMeshDisplayer> navMeshDisplayer;

            //sound
            std::unique_ptr<SoundEnvironment> soundEnvironment;

            //map handler
            std::unique_ptr<MapHandler> mapHandler;

            //scene specifics
            std::array<bool, LAST_VIEW_PROPERTIES> viewProperties;
            const SceneModel* highlightSceneModel;
            const SceneLight* highlightSceneLight;
            const SceneSound* highlightSceneSound;
    };

}
