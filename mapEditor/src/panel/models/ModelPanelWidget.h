#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTabWidget>

#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <controller/models/ModelController.h>
#include <panel/models/ModelTableView.h>
#include <panel/models/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class ModelPanelWidget : public QWidget, public Observer, public Observable { //TODO rename in object + packages
        Q_OBJECT

        public:
            ModelPanelWidget();
            ~ModelPanelWidget() override = default;

            enum NotificationType {
                OBJECT_BODY_SHAPE_WIDGET_CREATED
            };

            ModelTableView* getObjectTableView() const;
            BodyShapeWidget* getBodyShapeWidget() const;

            void load(ModelController&);
            void unload();

        private:
            void setupTransformBox(QVBoxLayout*);
            void setupPosition(QGridLayout*);
            void setupOrientation(QGridLayout*);
            void setupScale(QGridLayout*);
            void setupFlagsBox(QVBoxLayout*);
            void setupPhysicsBox(QVBoxLayout*);
            void setupPhysicsGeneralPropertiesBox(QVBoxLayout*);
            void setupPhysicsDampingPropertiesBox(QVBoxLayout*);
            void setupPhysicsFactorPropertiesBox(QVBoxLayout*);
            void setupPhysicsShapeBox(QVBoxLayout*);
            void setupTagsBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupObjectDataFrom(const ObjectEntity&);
            void setupObjectPhysicsDataFrom(const ObjectEntity&);
            void setupObjectTagsDataFrom(const ObjectEntity&);

            BodyShapeWidget& createBodyShapeWidget(const CollisionShape3D&, const ObjectEntity&);
            BodyShapeWidget& createNoBodyShapeWidget(const ObjectEntity&);
            void setupBodyShapeWidget();

            ModelController* objectController;

            ModelTableView* objectTableView;
            QPushButton* addObjectButton;
            QPushButton* removeObjectButton;
            QPushButton* cloneObjectButton;
            QTabWidget* tabWidget;

            bool disableModelEvent;

            //general
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QComboBox* orientationType;
            QDoubleSpinBox* eulerAxis0;
            QDoubleSpinBox* eulerAxis1;
            QDoubleSpinBox* eulerAxis2;
            QDoubleSpinBox* scale;
            QCheckBox* produceShadowCheckBox;
            QLineEdit* tags;

            //physics
            QCheckBox* hasRigidBody;
            QTabWidget* tabPhysicsRigidBody;
            QVBoxLayout* physicsShapeLayout;
            QDoubleSpinBox* mass;
            QDoubleSpinBox* restitution;
            QDoubleSpinBox* friction;
            QDoubleSpinBox* rollingFriction;
            QDoubleSpinBox* linearDamping;
            QDoubleSpinBox* angularDamping;
            QDoubleSpinBox* linearFactorX;
            QDoubleSpinBox* linearFactorY;
            QDoubleSpinBox* linearFactorZ;
            QDoubleSpinBox* angularFactorX;
            QDoubleSpinBox* angularFactorY;
            QDoubleSpinBox* angularFactorZ;
            QLabel* shapeTypeValueLabel;
            QPushButton* changeBodyShapeButton;
            std::unique_ptr<BodyShapeWidget> bodyShapeWidget;

        private slots:
            void showAddObjectDialog();
            void removeSelectedObject();
            void showCloneObjectDialog();

            void updateObjectOrientationType();
            void updateObjectTransform();
            void updateObjectScale();
            void updateObjectFlags();
            void updateObjectTags();

            void showChangeBodyShapeDialog();
            void rigidBodyToggled(int);
            void updateObjectPhysicsProperties();
            void bodyShapeChanged(std::unique_ptr<const CollisionShape3D>&);
    };

}
