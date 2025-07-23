#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinAggregation.h>

#include "controller/SceneController.h"
#include "scene/object/ObjectMoveAxisDisplayer.h"
#include "widget/controller/statusbar/StatusBarController.h"
#include "widget/controller/mouse/MouseController.h"

namespace urchin {

    class ObjectMoveController final : public Observable {
        public:
            ObjectMoveController(Scene&, SceneController&, MouseController&, StatusBarController&);

            enum NotificationType {
                OBJECT_MOVED
            };

            void onResize(unsigned int, unsigned int);

            void onCtrlXYZ(unsigned int);
            bool onMouseMove(double, double);
            void onMouseOut();
            bool onMouseLeftButton();
            bool onEscapeKey();

            void setSelectedObjectEntities(std::vector<const ObjectEntity*>);
            const ObjectEntity& getLastMovedObjectEntity() const;

            void displayAxis();

        private:
            bool adjustMousePosition();
            void moveObjects(const Point2<float>&, const Point2<float>&);
            Point3<float> computeSelectObjectsMeanPosition() const;
            void updateObjectPosition(const ObjectEntity*, const Point3<float>&);

            unsigned int sceneWidth;
            unsigned int sceneHeight;

            ObjectMoveAxisDisplayer objectMoveAxisDisplayer;
            Scene& scene;
            SceneController& sceneController;
            MouseController& mouseController;
            StatusBarController& statusBarController;

            std::vector<const ObjectEntity*> selectedObjectEntities;
            const ObjectEntity* lastMovedObjectEntity;
            int selectedAxis;
            std::map<const ObjectEntity*, Point3<float>> savedPositions;

            double oldMouseX;
            double oldMouseY;
    };

}
