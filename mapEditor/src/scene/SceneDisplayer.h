#pragma once

#include <array>
#include <memory>
#include <QWidget>
#include <UrchinPhysicsEngine.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAggregation.h>

#include <widget/controller/mouse/MouseController.h>
#include <widget/controller/statusbar/StatusBarController.h>
#include <controller/SceneController.h>
#include <scene/SceneFreeCamera.h>
#include <scene/objects/BodyShapeDisplayer.h>
#include <scene/objects/move/ObjectMoveController.h>
#include <scene/lights/LightScopeDisplayer.h>
#include <scene/sounds/SoundScopeDisplayer.h>

namespace urchin {

    class SceneDisplayerWindow;
    class SceneWindowController;

    class SceneDisplayer {
        public:
            enum ViewProperties {
                OBJECT_PHYSICS = 0,
                LIGHT_SCOPE,
                SOUND_SCOPE,
                NAV_MESH,

                LAST_VIEW_PROPERTIES
            };

            SceneDisplayer(SceneWindowController&, SceneController*, MouseController&, StatusBarController&);
            ~SceneDisplayer();

            void loadMap(const std::string&, const std::string&, const std::string&);
            void loadEmptyScene(const std::string&);

            void setViewProperties(ViewProperties, bool);
            void setHighlightObjectEntity(const ObjectEntity*);
            void setHighlightLightEntity(const LightEntity*);
            void setHighlightSoundEntity(const SoundEntity*);

            void paint();
            void resize(unsigned int, unsigned int) const;

            Scene& getScene() const;
            SceneFreeCamera* getCamera() const;
            PhysicsWorld& getPhysicsWorld() const;
            Map& getMap() const;

            BodyShapeDisplayer* getBodyShapeDisplayer() const;
            ObjectMoveController* getObjectMoveController() const;

        private:
            static void initializeEngineResources(const std::string&);
            void initializeScene(const std::string&);

            void refreshObjectsModel() const;
            void refreshLightScopeModel() const;
            void refreshSoundTriggerModel() const;
            void refreshNavMeshModel() const;

            bool isInitialized;
            SceneWindowController& windowController;
            SceneController* sceneController;
            MouseController& mouseController;
            StatusBarController& statusBarController;

            //3d
            std::unique_ptr<Scene> scene;
            std::shared_ptr<SceneFreeCamera> camera;
            std::unique_ptr<BodyShapeDisplayer> bodyShapeDisplayer;
            std::unique_ptr<ObjectMoveController> objectMoveController;
            std::unique_ptr<LightScopeDisplayer> lightScopeDisplayer;
            std::unique_ptr<SoundScopeDisplayer> soundScopeDisplayer;

            //physics
            std::unique_ptr<PhysicsWorld> physicsWorld;

            //AI
            std::unique_ptr<AIEnvironment> aiEnvironment;
            std::unique_ptr<NavMeshDisplayer> navMeshDisplayer;

            //sound
            std::unique_ptr<SoundEnvironment> soundEnvironment;

            //map
            std::unique_ptr<Map> map;

            //scene specifics
            std::array<bool, LAST_VIEW_PROPERTIES> viewProperties;
            const ObjectEntity* highlightObjectEntity;
            const LightEntity* highlightLightEntity;
            const SoundEntity* highlightSoundEntity;
    };

}
