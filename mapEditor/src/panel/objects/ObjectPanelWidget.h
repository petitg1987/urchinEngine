#pragma once

#include <memory>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTabWidget>

#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <controller/objects/ObjectController.h>
#include <panel/objects/ObjectTableView.h>
#include <panel/objects/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class ObjectPanelWidget final : public QWidget, public Observer, public Observable {
        Q_OBJECT

        public:
            ObjectPanelWidget();
            ~ObjectPanelWidget() override = default;

            enum NotificationType {
                OBJECT_BODY_SHAPE_WIDGET_CREATED
            };

            ObjectTableView* getObjectTableView() const;
            BodyShapeWidget* getBodyShapeWidget() const;

            void load(ObjectController&);
            void unload();

        private:
            void setupTransformBox(QVBoxLayout*);
            void setupPosition(QGridLayout*);
            void setupOrientation(QGridLayout*);
            void setupScale(QGridLayout*);
            void setupPropertiesBox(QVBoxLayout*);
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
            QPushButton* renameObjectButton;
            QPushButton* moveUpObjectButton;
            QPushButton* moveDownObjectButton;
            QTabWidget* tabWidget;

            bool disableObjectEvent;

            //general
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
            void removeSelectedObject() const;
            void showCloneObjectDialog();
            void showRenameObjectDialog();
            void moveUpSelectedObject() const;
            void moveDownSelectedObject() const;

            void updateObjectOrientationType() const;
            void updateObjectTransform() const;
            void updateObjectScale();
            void updateObjectProperties() const;
            void updateObjectTags() const;

            void showChangeBodyShapeDialog();
            void rigidBodyToggled(int);
            void updateObjectPhysicsProperties() const;
            void bodyShapeChanged(std::unique_ptr<const CollisionShape3D>&) const;
    };

}
