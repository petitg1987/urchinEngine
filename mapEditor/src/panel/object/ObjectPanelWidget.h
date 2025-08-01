#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTabWidget>
#include <UrchinCommon.h>

#include "controller/object/ObjectController.h"
#include "panel/object/ObjectTableView.h"
#include "panel/object/physics/PhysicsWidget.h"
#include "panel/object/light/LightWidget.h"
#include "panel/object/sound/SoundWidget.h"

namespace urchin {

    class ObjectPanelWidget final : public QWidget, public Observer, public Observable {
        Q_OBJECT

        public:
            ObjectPanelWidget();
            ~ObjectPanelWidget() override = default;

            enum class ObjectTab {
                MODEL = 0,
                PHYSICS,
                LIGHT,
                SOUND,
                TAG
            };

            enum NotificationType {
                OBJECT_SUB_TAB_SELECTION_CHANGED
            };

            ObjectTableView* getObjectTableView() const;
            ObjectTab getTabSelected() const;
            PhysicsWidget* getPhysicsWidget() const;

            void load(ObjectController&);
            void unload();

        private:
            void setupMeshesBox(QVBoxLayout*);
            void setupTransformBox(QVBoxLayout*);
            void setupPosition(QGridLayout*);
            void setupOrientation(QGridLayout*);
            void setupScale(QGridLayout*);
            void setupPropertiesBox(QVBoxLayout*);
            void setupPhysicsBox(QVBoxLayout*);
            void setupLightBox(QVBoxLayout*);
            void setupSoundBox(QVBoxLayout*);
            void setupTagsBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupObjectDataFrom(const ObjectEntity&);
            void setupObjectModelDataFrom(const ObjectEntity&);
            void setupObjectPhysicsDataFrom(const ObjectEntity&);
            void setupObjectLightDataFrom(const ObjectEntity&);
            void setupObjectSoundDataFrom(const ObjectEntity&);
            void setupObjectTagsDataFrom(const ObjectEntity&);

            static constexpr char RECEIVER_AND_CASTER_LABEL[] = "Receiver & Caster";
            static constexpr char RECEIVER_ONLY_LABEL[] = "Receiver only";
            static constexpr char NONE_LABEL[] = "None";

            static constexpr char CULL_LABEL[] = "Cull";
            static constexpr char NO_CULL_LABEL[] = "No cull";

            ObjectController* objectController;

            ObjectTableView* objectTableView;
            QPushButton* addObjectButton;
            QPushButton* removeObjectButton;
            QPushButton* cloneObjectButton;
            QPushButton* updateObjectButton;
            QTabWidget* tabWidget;
            ObjectTab tabSelected;

            const ObjectEntity* objectEntitySelected;
            bool disableObjectEvent;

            //model
            QLabel* meshesFile;
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QComboBox* orientationType;
            QDoubleSpinBox* eulerAxis0;
            QDoubleSpinBox* eulerAxis1;
            QDoubleSpinBox* eulerAxis2;
            QDoubleSpinBox* scaleX;
            QDoubleSpinBox* scaleY;
            QDoubleSpinBox* scaleZ;
            QComboBox* shadowBehavior;
            QComboBox* cullBehavior;
            QLineEdit* tags;

            //physics
            QCheckBox* hasRigidBody;
            PhysicsWidget* physicsWidget;

            //light
            QLabel* lightTypeValueLabel;
            LightWidget* lightWidget;

            //sound
            QLabel* soundTypeValueLabel;
            SoundWidget* soundWidget;

        private slots:
            void showAddObjectDialog();
            void removeSelectedObject() const;
            void showCloneObjectDialog();
            void showUpdateObjectDialog();
            void onObjectTabChanged(int);

            void updateObjectOrientationType() const;
            void updateObjectTransform() const;
            void updateObjectScale() const;
            void updateObjectProperties() const;
            void updateObjectTags() const;

            void showChangeMeshesFileDialog();

            void rigidBodyToggled(int);

            void showChangeLightTypeDialog();

            void showChangeSoundDialog();
    };

}
