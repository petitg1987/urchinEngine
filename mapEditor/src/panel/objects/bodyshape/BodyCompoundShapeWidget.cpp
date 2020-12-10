#include <stdexcept>
#include <QtWidgets/QGroupBox>

#include "BodyCompoundShapeWidget.h"
#include "widget/style/LabelStyleHelper.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/SpinBoxStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"
#include "widget/style/ComboBoxStyleHelper.h"
#include "panel/objects/dialog/ChangeBodyShapeDialog.h"
#include "panel/objects/bodyshape/BodyShapeWidgetRetriever.h"
#include "panel/objects/bodyshape/support/DefaultBodyShapeCreator.h"

namespace urchin {

    BodyCompoundShapeWidget::BodyCompoundShapeWidget(const SceneObject* sceneObject) :
            BodyShapeWidget(sceneObject) {
        shapesLabel = new QLabel("Shapes:");
        mainLayout->addWidget(shapesLabel, 0, 0);

        localizedShapeTableView = new LocalizedShapeTableView();
        mainLayout->addWidget(localizedShapeTableView, 1, 0);
        localizedShapeTableView->setFixedHeight(100);
        localizedShapeTableView->addObserver(this, LocalizedShapeTableView::OBJECT_COMPOUND_SHAPE_SELECTION_CHANGED);

        auto* buttonLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonLayout, 2, 0);
        buttonLayout->setAlignment(Qt::AlignLeft);

        addShapeButton = new QPushButton("New Shape");
        buttonLayout->addWidget(addShapeButton);
        ButtonStyleHelper::applyNormalStyle(addShapeButton);
        connect(addShapeButton, SIGNAL(clicked()), this, SLOT(addNewLocalizedShape()));

        removeShapeButton = new QPushButton("Remove Shape");
        buttonLayout->addWidget(removeShapeButton);
        ButtonStyleHelper::applyNormalStyle(removeShapeButton);
        connect(removeShapeButton, SIGNAL(clicked()), this, SLOT(removeSelectedLocalizedShape()));

        localizedShapeDetails = nullptr;
        positionX = nullptr;
        positionY = nullptr;
        positionZ = nullptr;
        orientationType = nullptr;
        eulerAxis0 = nullptr;
        eulerAxis1 = nullptr;
        eulerAxis2 = nullptr;
        bodyShapeWidget = nullptr;
    }

    std::string BodyCompoundShapeWidget::getBodyShapeName() const {
        return COMPOUND_SHAPE_LABEL;
    }

    LocalizedShapeTableView* BodyCompoundShapeWidget::getLocalizedShapeTableView() const {
        return localizedShapeTableView;
    }

    void BodyCompoundShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape) {
        const auto* compoundShape = dynamic_cast<const CollisionCompoundShape*>(shape.get());

        const std::vector<std::shared_ptr<const LocalizedCollisionShape>>& localizedShapes = compoundShape->getLocalizedShapes();
        for (const auto& localizedShape : localizedShapes) {
            localizedShapeTableView->addLocalizedShape(localizedShape);
        }
        localizedShapeTableView->selectLocalizedShape(0);
    }

    std::shared_ptr<const CollisionShape3D> BodyCompoundShapeWidget::createBodyShape() const {
        try {
            LabelStyleHelper::applyNormalStyle(shapesLabel);
            std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes = localizedShapeTableView->getLocalizedShapes();
            return std::make_shared<const CollisionCompoundShape>(localizedCollisionShapes);
        } catch (std::invalid_argument& e) {
            LabelStyleHelper::applyErrorStyle(shapesLabel, std::string(e.what()));
            return DefaultBodyShapeCreator(getSceneObject()).createDefaultBodyShape(CollisionShape3D::ShapeType::COMPOUND_SHAPE, true);
        }
    }

    void BodyCompoundShapeWidget::notify(Observable* observable, int notificationType) {
        if (auto* localizedShapeTableView = dynamic_cast<LocalizedShapeTableView*>(observable)) {
            if (notificationType == LocalizedShapeTableView::OBJECT_COMPOUND_SHAPE_SELECTION_CHANGED) {
                if (localizedShapeTableView->hasLocalizedShapeSelected()) {
                    std::shared_ptr<const LocalizedCollisionShape> localizedShape = localizedShapeTableView->getSelectedLocalizedShape();

                    delete localizedShapeDetails;
                    localizedShapeDetails = new QWidget();
                    mainLayout->addWidget(localizedShapeDetails, 3, 0);

                    auto* localizedShapeLayout = new QVBoxLayout(localizedShapeDetails);
                    setupTransformBox(localizedShapeLayout, localizedShape);
                    setupShapeBox(localizedShapeLayout, localizedShape);

                    localizedShapeDetails->show();
                } else {
                    delete localizedShapeDetails;
                    localizedShapeDetails = nullptr;
                }
            }
        }
    }

    void BodyCompoundShapeWidget::setupTransformBox(QVBoxLayout* localizedShapeLayout, const std::shared_ptr<const LocalizedCollisionShape>& localizedShape) {
        auto* transformGroupBox = new QGroupBox("Transform");
        localizedShapeLayout->addWidget(transformGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(transformGroupBox);
        transformGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* transformLayout = new QGridLayout(transformGroupBox);
        setupPosition(transformLayout, localizedShape->transform.getPosition());
        setupOrientation(transformLayout,localizedShape->transform.getOrientation());
    }

    void BodyCompoundShapeWidget::setupPosition(QGridLayout* transformLayout, const Point3<float>& position) {
        auto *positionLabel= new QLabel("Position:");
        transformLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        transformLayout->addLayout(positionLayout, 0, 1);
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
        positionX->setValue(position.X);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        positionY->setValue(position.Y);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        positionZ->setValue(position.Z);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
    }

    void BodyCompoundShapeWidget::setupOrientation(QGridLayout* transformLayout, const Quaternion<float>& orientation) {
        auto* orientationTypeLabel = new QLabel("Orient. Type:");
        transformLayout->addWidget(orientationTypeLabel, 1, 0);

        orientationType = new QComboBox();
        transformLayout->addWidget(orientationType, 1, 1);
        ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
        connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLocalizedShapeOrientationType()));

        auto* eulerAngleLabel = new QLabel("Euler Angle:");
        transformLayout->addWidget(eulerAngleLabel, 2, 0);

        QVariant variant = orientationType->currentData();
        auto newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
        Vector3<float> eulerAngle = orientation.toEulerAngle(newRotationSequence);

        auto* eulerAxisLayout = new QHBoxLayout();
        transformLayout->addLayout(eulerAxisLayout, 2, 1);
        eulerAxis0 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis0);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
        eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
        connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
        eulerAxis1 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis1);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
        eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
        connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
        eulerAxis2 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis2);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis2);
        eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));
        connect(eulerAxis2, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
    }

    void BodyCompoundShapeWidget::setupShapeBox(QVBoxLayout* localizedShapeLayout, const std::shared_ptr<const LocalizedCollisionShape>& localizedShape) {
        auto* shapeGroupBox = new QGroupBox("Shape");
        localizedShapeLayout->addWidget(shapeGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(shapeGroupBox);
        shapeGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* shapeLayout = new QGridLayout(shapeGroupBox);
        bodyShapeWidget = BodyShapeWidgetRetriever(getSceneObject()).createBodyShapeWidget(localizedShape->shape->getShapeType());
        shapeLayout->addWidget(bodyShapeWidget, 0, 0);
        bodyShapeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        bodyShapeWidget->setupShapePropertiesFrom(localizedShape->shape);
        connect(bodyShapeWidget, SIGNAL(bodyShapeChange(std::shared_ptr<const CollisionShape3D>)), this, SLOT(updateSelectedLocalizedShape()));
    }

    void BodyCompoundShapeWidget::updateLocalizedShapeOrientationType() {
        if (!disableShapeEvent) {
            std::shared_ptr<const LocalizedCollisionShape> localizedShape = localizedShapeTableView->getSelectedLocalizedShape();

            QVariant variant = orientationType->currentData();
            auto newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
            Vector3<float> eulerAngle = localizedShape->transform.getOrientation().toEulerAngle(newRotationSequence);

            eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
            eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
            eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

            updateSelectedLocalizedShape();
        }
    }

    void BodyCompoundShapeWidget::updateSelectedLocalizedShape() {
        if (!disableShapeEvent) {
            if (!localizedShapeTableView->hasLocalizedShapeSelected()) {
                throw std::invalid_argument("Localized shape table hasn't localized shaped selected");
            }

            std::shared_ptr<LocalizedCollisionShape> localizedShape = std::make_shared<LocalizedCollisionShape>();

            localizedShape->position = localizedShapeTableView->getSelectedLocalizedShape()->position;
            localizedShape->shape = bodyShapeWidget->retrieveShape();

            Vector3<float> eulerAngle(
                    AngleConverter<float>::toRadian((float)eulerAxis0->value()),
                    AngleConverter<float>::toRadian((float)eulerAxis1->value()),
                    AngleConverter<float>::toRadian((float)eulerAxis2->value())
            );
            QVariant variant = orientationType->currentData();
            auto rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
            localizedShape->transform = PhysicsTransform(
                    Point3<float>((float)positionX->value(), (float)positionY->value(), (float)positionZ->value()),
                    Quaternion<float>(eulerAngle, rotationSequence));

            localizedShapeTableView->updateSelectedLocalizedShape(localizedShape);

            updateBodyShape();
        }
    }

    void BodyCompoundShapeWidget::addNewLocalizedShape() {
        ChangeBodyShapeDialog changeBodyShapeDialog(this, true);
        changeBodyShapeDialog.exec();

        if (changeBodyShapeDialog.result() == QDialog::Accepted) {
            CollisionShape3D::ShapeType shapeType = changeBodyShapeDialog.getShapeType();
            std::shared_ptr<const CollisionShape3D> defaultNewShape = DefaultBodyShapeCreator(getSceneObject()).createDefaultBodyShape(shapeType, true);

            std::size_t nextPosition = 0;
            for (std::size_t i=0; i<localizedShapeTableView->getLocalizedShapes().size(); ++i) {
                if (nextPosition<=localizedShapeTableView->getLocalizedShapes()[i]->position) {
                    nextPosition = localizedShapeTableView->getLocalizedShapes()[i]->position + 1;
                }
            }

            std::shared_ptr<LocalizedCollisionShape> localizedShape = std::make_shared<LocalizedCollisionShape>();
            localizedShape->position = nextPosition;
            localizedShape->shape = defaultNewShape;
            localizedShape->transform = PhysicsTransform();

            int rowId = localizedShapeTableView->addLocalizedShape(localizedShape);
            localizedShapeTableView->selectLocalizedShape(rowId);

            updateBodyShape();
        }
    }

    void BodyCompoundShapeWidget::removeSelectedLocalizedShape() {
        if (localizedShapeTableView->removeSelectedLocalizedShape()) {
            updateBodyShape();
        }
    }
}
