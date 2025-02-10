#include <UrchinAggregation.h>

#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/ComboBoxStyleHelper.h>
#include <panel/objects/ObjectPanelWidget.h>
#include <panel/objects/dialog/NewObjectDialog.h>
#include <panel/objects/dialog/CloneObjectDialog.h>
#include <panel/objects/dialog/RenameObjectDialog.h>
#include <panel/objects/dialog/ChangeLightTypeDialog.h>
#include <panel/objects/dialog/ChangeSoundDialog.h>
#include <scene/objects/move/ObjectMoveController.h>
#include <scene/SceneDisplayerWindow.h>

namespace urchin {

    ObjectPanelWidget::ObjectPanelWidget() :
            objectController(nullptr),
            objectTableView(nullptr),
            addObjectButton(nullptr),
            removeObjectButton(nullptr),
            cloneObjectButton(nullptr),
            renameObjectButton(nullptr),
            tabWidget(nullptr),
            tabSelected(ObjectTab::GENERAL),
            disableObjectEvent(false),
            positionX(nullptr), positionY(nullptr), positionZ(nullptr),
            orientationType(nullptr),
            eulerAxis0(nullptr), eulerAxis1(nullptr), eulerAxis2(nullptr),
            scaleX(nullptr), scaleY(nullptr), scaleZ(nullptr),
            shadowBehavior(nullptr),
            cullBehavior(nullptr),
            tags(nullptr),
            hasRigidBody(nullptr),
            physicsWidget(nullptr),
            lightTypeValueLabel(nullptr),
            changeLightTypeButton(nullptr),
            lightWidget(nullptr),
            soundTypeValueLabel(nullptr),
            changeSoundTypeButton(nullptr),
            soundWidget(nullptr) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        objectTableView = new ObjectTableView(this);
        mainLayout->addWidget(objectTableView);
        objectTableView->addObserver(this, ObjectTableView::OBJECT_SELECTION_CHANGED);
        objectTableView->setFixedHeight(320);

        auto* buttonsLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonsLayout);
        buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        addObjectButton = new QPushButton("New");
        buttonsLayout->addWidget(addObjectButton);
        ButtonStyleHelper::applyNormalStyle(addObjectButton);
        connect(addObjectButton, SIGNAL(clicked()), this, SLOT(showAddObjectDialog()));

        removeObjectButton = new QPushButton("Remove");
        buttonsLayout->addWidget(removeObjectButton);
        ButtonStyleHelper::applyNormalStyle(removeObjectButton);
        removeObjectButton->setEnabled(false);
        connect(removeObjectButton, SIGNAL(clicked()), this, SLOT(removeSelectedObject()));

        cloneObjectButton = new QPushButton("Clone");
        buttonsLayout->addWidget(cloneObjectButton);
        ButtonStyleHelper::applyNormalStyle(cloneObjectButton);
        cloneObjectButton->setEnabled(false);
        connect(cloneObjectButton, SIGNAL(clicked()), this, SLOT(showCloneObjectDialog()));

        renameObjectButton = new QPushButton("Rename");
        buttonsLayout->addWidget(renameObjectButton);
        ButtonStyleHelper::applyNormalStyle(renameObjectButton);
        renameObjectButton->setEnabled(false);
        connect(renameObjectButton, SIGNAL(clicked()), this, SLOT(showRenameObjectDialog()));

        moveUpObjectButton = new QPushButton("▲");
        buttonsLayout->addWidget(moveUpObjectButton);
        ButtonStyleHelper::applyNormalStyle(moveUpObjectButton);
        moveUpObjectButton->setFixedWidth(22);
        moveUpObjectButton->setEnabled(false);
        connect(moveUpObjectButton, SIGNAL(clicked()), this, SLOT(moveUpSelectedObject()));

        moveDownObjectButton = new QPushButton("▼");
        buttonsLayout->addWidget(moveDownObjectButton);
        ButtonStyleHelper::applyNormalStyle(moveDownObjectButton);
        moveDownObjectButton->setFixedWidth(22);
        moveDownObjectButton->setEnabled(false);
        connect(moveDownObjectButton, SIGNAL(clicked()), this, SLOT(moveDownSelectedObject()));

        tabWidget = new QTabWidget();
        mainLayout->addWidget(tabWidget);
        tabWidget->hide();

        //general properties
        auto* tabGeneral = new QWidget();
        auto* generalLayout = new QVBoxLayout(tabGeneral);
        generalLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        generalLayout->setContentsMargins(1, 1, 1, 1);
        setupTransformBox(generalLayout);
        setupPropertiesBox(generalLayout);
        tabWidget->addTab(tabGeneral, "General");

        //physics properties
        auto* tabPhysics = new QWidget();
        auto* physicsLayout = new QVBoxLayout(tabPhysics);
        physicsLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        physicsLayout->setContentsMargins(1, 1, 1, 1);
        setupPhysicsBox(physicsLayout);
        tabWidget->addTab(tabPhysics, "Physics");

        //light properties
        auto* tabLight = new QWidget();
        auto* lightLayout = new QVBoxLayout(tabLight);
        lightLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        lightLayout->setContentsMargins(1, 1, 1, 1);
        setupLightBox(lightLayout);
        tabWidget->addTab(tabLight, "Light");

        //sound properties
        auto* tabSound = new QWidget();
        auto* soundLayout = new QVBoxLayout(tabSound);
        soundLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        soundLayout->setContentsMargins(1, 1, 1, 1);
        setupSoundBox(soundLayout);
        tabWidget->addTab(tabSound, "Sound");

        //tags properties
        auto* tabTags = new QWidget();
        auto* tagsLayout = new QVBoxLayout(tabTags);
        tagsLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        tagsLayout->setContentsMargins(1, 1, 1, 1);
        setupTagsBox(tagsLayout);
        tabWidget->addTab(tabTags, "Tags");

        connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onObjectTabChanged(int)));
    }

    void ObjectPanelWidget::onObjectTabChanged(int tabSelected) {
        this->tabSelected = static_cast<ObjectTab>(tabSelected);

        notifyObservers(this, OBJECT_SUB_TAB_SELECTION_CHANGED);
    }

    ObjectTableView* ObjectPanelWidget::getObjectTableView() const {
        return objectTableView;
    }

    ObjectPanelWidget::ObjectTab ObjectPanelWidget::getTabSelected() const {
        return tabSelected;
    }

    PhysicsWidget* ObjectPanelWidget::getPhysicsWidget() const {
        return physicsWidget;
    }

    void ObjectPanelWidget::setupTransformBox(QVBoxLayout* generalLayout) {
        auto* transformGroupBox = new QGroupBox("Transform");
        generalLayout->addWidget(transformGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(transformGroupBox);
        transformGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* transformLayout = new QGridLayout(transformGroupBox);

        setupPosition(transformLayout);
        setupOrientation(transformLayout);
        setupScale(transformLayout);
    }

    void ObjectPanelWidget::setupPosition(QGridLayout* transformLayout) {
        auto* positionLabel= new QLabel("Position:");
        transformLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        transformLayout->addLayout(positionLayout, 0, 1);
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
    }

    void ObjectPanelWidget::setupOrientation(QGridLayout* transformLayout) {
        auto* orientationTypeLabel = new QLabel("Orient. Type:");
        transformLayout->addWidget(orientationTypeLabel, 1, 0);

        orientationType = new QComboBox();
        transformLayout->addWidget(orientationType, 1, 1);
        ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
        connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateObjectOrientationType()));

        auto* eulerAngleLabel = new QLabel("Euler Angle:");
        transformLayout->addWidget(eulerAngleLabel, 2, 0);

        auto* eulerAxisLayout = new QHBoxLayout();
        transformLayout->addLayout(eulerAxisLayout, 2, 1);
        eulerAxis0 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis0);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
        connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        eulerAxis1 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis1);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
        connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        eulerAxis2 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis2);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis2);
        connect(eulerAxis2, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
    }

    void ObjectPanelWidget::setupScale(QGridLayout* transformLayout) {
        auto* angleLabel = new QLabel("Scale:");
        transformLayout->addWidget(angleLabel, 3, 0);

        auto* positionLayout = new QHBoxLayout();
        transformLayout->addLayout(positionLayout, 3, 1);
        scaleX = new QDoubleSpinBox();
        positionLayout->addWidget(scaleX);
        SpinBoxStyleHelper::applyDefaultStyleOn(scaleX);
        scaleX->setMinimum(0.01);
        connect(scaleX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectScale()));
        scaleY = new QDoubleSpinBox();
        positionLayout->addWidget(scaleY);
        SpinBoxStyleHelper::applyDefaultStyleOn(scaleY);
        scaleY->setMinimum(0.01);
        connect(scaleY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectScale()));
        scaleZ = new QDoubleSpinBox();
        positionLayout->addWidget(scaleZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(scaleZ);
        scaleZ->setMinimum(0.01);
        connect(scaleZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectScale()));
    }

    void ObjectPanelWidget::setupPropertiesBox(QVBoxLayout* generalLayout) {
        auto* propertiesGroupBox = new QGroupBox("Properties");
        generalLayout->addWidget(propertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(propertiesGroupBox);
        propertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* propertiesLayout = new QGridLayout(propertiesGroupBox);
        propertiesLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* shadowBehaviorLabel = new QLabel("Shadow behavior:");
        propertiesLayout->addWidget(shadowBehaviorLabel, 0, 0);

        shadowBehavior = new QComboBox();
        propertiesLayout->addWidget(shadowBehavior, 0, 1, 1, 3);
        shadowBehavior->setFixedWidth(150);
        shadowBehavior->addItem(RECEIVER_AND_CASTER_LABEL, QVariant((int)Model::ShadowBehavior::RECEIVER_AND_CASTER));
        shadowBehavior->addItem(RECEIVER_ONLY_LABEL, QVariant((int)Model::ShadowBehavior::RECEIVER_ONLY));
        shadowBehavior->addItem(NONE_LABEL, QVariant((int)Model::ShadowBehavior::NONE));
        connect(shadowBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateObjectProperties()));

        auto* cullBehaviorLabel = new QLabel("Cull behavior:");
        propertiesLayout->addWidget(cullBehaviorLabel, 1, 0);

        cullBehavior = new QComboBox();
        propertiesLayout->addWidget(cullBehavior, 1, 1, 1, 3);
        cullBehavior->setFixedWidth(150);
        cullBehavior->addItem(CULL_LABEL, QVariant((int)Model::CullBehavior::CULL));
        cullBehavior->addItem(NO_CULL_LABEL, QVariant((int)Model::CullBehavior::NO_CULL));
        connect(cullBehavior, SIGNAL(currentIndexChanged(int)), this, SLOT(updateObjectProperties()));
    }

    void ObjectPanelWidget::setupPhysicsBox(QVBoxLayout* physicsLayout) {
        hasRigidBody = new QCheckBox("Rigid Body");
        physicsLayout->addWidget(hasRigidBody);
        connect(hasRigidBody, SIGNAL(stateChanged(int)), this, SLOT(rigidBodyToggled(int)));

        physicsWidget = new PhysicsWidget();
        physicsLayout->addWidget(physicsWidget);
    }

    void ObjectPanelWidget::setupLightBox(QVBoxLayout* lightLayout) {
        auto* lightTypeLayout = new QHBoxLayout();
        lightTypeLayout->setAlignment(Qt::AlignLeft);
        lightTypeLayout->setSpacing(15);
        lightLayout->addLayout(lightTypeLayout);

        auto* lightTypeLabel = new QLabel("Light Type:");
        lightTypeLayout->addWidget(lightTypeLabel);

        lightTypeValueLabel = new QLabel();
        lightTypeLayout->addWidget(lightTypeValueLabel);

        changeLightTypeButton = new QPushButton("Change");
        lightTypeLayout->addWidget(changeLightTypeButton);
        ButtonStyleHelper::applyNormalStyle(changeLightTypeButton);
        connect(changeLightTypeButton, SIGNAL(clicked()), this, SLOT(showChangeLightTypeDialog()));

        lightWidget = new LightWidget();
        lightLayout->addWidget(lightWidget);
    }

    void ObjectPanelWidget::setupSoundBox(QVBoxLayout* soundLayout) {
        auto* soundTypeLayout = new QHBoxLayout();
        soundTypeLayout->setAlignment(Qt::AlignLeft);
        soundTypeLayout->setSpacing(15);
        soundLayout->addLayout(soundTypeLayout);

        auto* soundTypeLabel = new QLabel("Sound Type:");
        soundTypeLayout->addWidget(soundTypeLabel);

        soundTypeValueLabel = new QLabel();
        soundTypeLayout->addWidget(soundTypeValueLabel);

        changeSoundTypeButton = new QPushButton("Change");
        soundTypeLayout->addWidget(changeSoundTypeButton);
        ButtonStyleHelper::applyNormalStyle(changeSoundTypeButton);
        connect(changeSoundTypeButton, SIGNAL(clicked()), this, SLOT(showChangeSoundDialog()));

        soundWidget = new SoundWidget();
        soundLayout->addWidget(soundWidget);
    }

    void ObjectPanelWidget::setupTagsBox(QVBoxLayout* mainTagsLayout) {
        auto* tagsGroupBox = new QGroupBox("Tags");
        mainTagsLayout->addWidget(tagsGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(tagsGroupBox);
        tagsGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* tagsLayout = new QGridLayout(tagsGroupBox);

        auto* tagsLabel = new QLabel("Tags:");
        tagsLabel->setToolTip("Comma separated");
        tagsLayout->addWidget(tagsLabel, 0, 0);

        tags = new QLineEdit();
        tagsLayout->addWidget(tags, 0, 1);
        connect(tags, SIGNAL(textChanged(QString)), this, SLOT(updateObjectTags()));
    }

    void ObjectPanelWidget::load(ObjectController& objectController) {
        this->objectController = &objectController;

        std::list<const ObjectEntity*> objectEntities = this->objectController->getObjectEntities();
        for (auto& objectEntity : objectEntities) {
            objectTableView->addObject(*objectEntity);
        }
    }

    void ObjectPanelWidget::unload() {
        objectTableView->removeAllObjects();

        objectController = nullptr;
    }

    void ObjectPanelWidget::notify(Observable* observable, int notificationType) {
        if (const auto* objectTableView = dynamic_cast<ObjectTableView*>(observable)) {
            if (notificationType == ObjectTableView::OBJECT_SELECTION_CHANGED) {
                if (objectTableView->hasObjectEntitySelected()) {
                    const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
                    setupObjectDataFrom(objectEntity);

                    removeObjectButton->setEnabled(true);
                    cloneObjectButton->setEnabled(true);
                    renameObjectButton->setEnabled(true);
                    moveUpObjectButton->setEnabled(!objectTableView->isFirstObjectEntitySelected());
                    moveDownObjectButton->setEnabled(!objectTableView->isLastObjectEntitySelected());
                    tabWidget->show();
                } else {
                    removeObjectButton->setEnabled(false);
                    cloneObjectButton->setEnabled(false);
                    renameObjectButton->setEnabled(false);
                    moveUpObjectButton->setEnabled(false);
                    moveDownObjectButton->setEnabled(false);
                    tabWidget->hide();
                }
            }
        } else if (const auto* sceneDisplayerWidget = dynamic_cast<SceneDisplayerWindow*>(observable)) {
            if (notificationType == SceneDisplayerWindow::BODY_PICKED) {
                const std::string& bodyId = sceneDisplayerWidget->getLastPickedBodyId();
                const ObjectEntity* objectEntity = bodyId.empty() ? nullptr : objectController->findObjectEntityByBodyId(bodyId);
                if (objectEntity) {
                    int row = this->objectTableView->getObjectEntityRow(objectEntity);
                    if (row >= 0) {
                        this->objectTableView->selectRow(row);
                    }
                } else {
                    this->objectTableView->clearSelection();
                }
            }
        } else if (const auto* objectMoveController = dynamic_cast<ObjectMoveController*>(observable)) {
            if (notificationType == ObjectMoveController::OBJECT_MOVED) {
                setupObjectDataFrom(*objectMoveController->getSelectedObjectEntity());
            }
        }
    }

    void ObjectPanelWidget::setupObjectDataFrom(const ObjectEntity& objectEntity) {
        disableObjectEvent = true;
        const Model* model = objectEntity.getModel();
        const Transform<float>& modelTransform = model->getTransform();

        this->positionX->setValue(modelTransform.getPosition().X);
        this->positionY->setValue(modelTransform.getPosition().Y);
        this->positionZ->setValue(modelTransform.getPosition().Z);

        int orientationTypeInt = orientationType->currentData().toInt();
        Quaternion<float>::RotationSequence rotationSequence = Quaternion<float>::RotationSequence::XYZ;
        if (orientationTypeInt >= 0) {
            rotationSequence = static_cast<Quaternion<float>::RotationSequence>(orientationTypeInt);
        }
        Vector3<float> eulerAngle = modelTransform.getOrientation().toEulerAngle(rotationSequence);
        this->eulerAxis0->setValue(AngleConverter<double>::toDegree(eulerAngle[0]));
        this->eulerAxis1->setValue(AngleConverter<double>::toDegree(eulerAngle[1]));
        this->eulerAxis2->setValue(AngleConverter<double>::toDegree(eulerAngle[2]));

        this->scaleX->setValue(modelTransform.getScale().X);
        this->scaleY->setValue(modelTransform.getScale().Y);
        this->scaleZ->setValue(modelTransform.getScale().Z);

        int shadowBehaviorIndex = shadowBehavior->findData((int)model->getShadowBehavior());
        if (shadowBehaviorIndex != -1) {
            shadowBehavior->setCurrentIndex(shadowBehaviorIndex);
        }

        int cullBehaviorIndex = cullBehavior->findData((int)model->getCullBehavior());
        if (cullBehaviorIndex != -1) {
            cullBehavior->setCurrentIndex(cullBehaviorIndex);
        }

        setupObjectPhysicsDataFrom(objectEntity);
        setupObjectLightDataFrom(objectEntity);
        setupObjectSoundDataFrom(objectEntity);
        setupObjectTagsDataFrom(objectEntity);
        disableObjectEvent = false;
    }

    void ObjectPanelWidget::setupObjectPhysicsDataFrom(const ObjectEntity& objectEntity) {
        disableObjectEvent = true;

        const RigidBody* rigidBody = objectEntity.getRigidBody();
        hasRigidBody->setChecked(rigidBody ? true : false);

        physicsWidget->load(objectEntity, *objectController);
        if (rigidBody) {
            physicsWidget->show();
        } else {
            physicsWidget->hide();
        }

        disableObjectEvent = false;
    }

    void ObjectPanelWidget::setupObjectLightDataFrom(const ObjectEntity& objectEntity) {
        disableObjectEvent = true;

        const Light* light = objectEntity.getLight();
        if (light) {
            if (light->getLightType() == Light::LightType::SUN) {
                lightTypeValueLabel->setText(QString::fromStdString(ChangeLightTypeDialog::SUN_LIGHT_LABEL));
            } else if (light->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                lightTypeValueLabel->setText(QString::fromStdString(ChangeLightTypeDialog::OMNIDIRECTIONAL_LIGHT_LABEL));
            } else if (light->getLightType() == Light::LightType::SPOT) {
                lightTypeValueLabel->setText(QString::fromStdString(ChangeLightTypeDialog::SPOT_LIGHT_LABEL));
            } else {
                throw std::invalid_argument("Unknown the light type: " + std::to_string((int)light->getLightType()));
            }
        } else {
            lightTypeValueLabel->setText(QString::fromStdString(ChangeLightTypeDialog::NONE_LABEL));
        }

        lightWidget->load(objectEntity, *objectController);
        if (light) {
            lightWidget->show();
        } else {
            lightWidget->hide();
        }

        disableObjectEvent = false;
    }

    void ObjectPanelWidget::setupObjectSoundDataFrom(const ObjectEntity& objectEntity) {
        disableObjectEvent = true;

        const SoundComponent* soundComponent = objectEntity.getSoundComponent();
        if (soundComponent) {
            if (soundComponent->getSound().getSoundType() == Sound::SoundType::LOCALIZABLE) {
                soundTypeValueLabel->setText(QString::fromStdString(ChangeSoundDialog::LOCALIZABLE_SOUND_LABEL));
            } else if (soundComponent->getSound().getSoundType() == Sound::SoundType::GLOBAL) {
                soundTypeValueLabel->setText(QString::fromStdString(ChangeSoundDialog::GLOBAL_SOUND_LABEL));
            } else {
                throw std::invalid_argument("Unknown the sound type: " + std::to_string((int)soundComponent->getSound().getSoundType()));
            }
        } else {
            soundTypeValueLabel->setText(QString::fromStdString(ChangeSoundDialog::NONE_LABEL));
        }

        soundWidget->load(objectEntity, *objectController);
        if (soundComponent) {
            soundWidget->show();
        } else {
            soundWidget->hide();
        }

        disableObjectEvent = false;
    }

    void ObjectPanelWidget::setupObjectTagsDataFrom(const ObjectEntity& objectEntity) {
        disableObjectEvent = true;
        std::string tagsValues = StringUtil::merge(objectEntity.getTags(), ObjectController::TAGS_SEPARATOR);
        this->tags->setText(QString::fromStdString(tagsValues));
        disableObjectEvent = false;
    }

    void ObjectPanelWidget::showAddObjectDialog() {
        NewObjectDialog newObjectEntityDialog(this, objectController);
        newObjectEntityDialog.exec();

        if (newObjectEntityDialog.result() == QDialog::Accepted) {
            std::unique_ptr<ObjectEntity> objectEntity = newObjectEntityDialog.moveObjectEntity();
            const ObjectEntity* objectEntityPtr = objectEntity.get();
            objectController->addObjectEntity(std::move(objectEntity));
            objectController->createDefaultBody(*objectEntityPtr);
            objectController->moveObjectInFrontOfCamera(*objectEntityPtr, false);

            int row = objectTableView->addObject(*objectEntityPtr);
            objectTableView->selectRow(row);
        }
    }

    void ObjectPanelWidget::removeSelectedObject() const {
        if (objectTableView->hasObjectEntitySelected()) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            objectController->removeObjectEntity(objectEntity);

            objectTableView->removeSelectedObject();
        }
    }

    void ObjectPanelWidget::showCloneObjectDialog() {
        std::string originalName = objectTableView->getSelectedObjectEntity()->getName();
        CloneObjectDialog cloneObjectEntityDialog(this, originalName, objectController);
        cloneObjectEntityDialog.exec();

        if (cloneObjectEntityDialog.result() == QDialog::Accepted) {
            const std::string& newObjectName = cloneObjectEntityDialog.getObjectName();
            const ObjectEntity& toCloneObjectEntity = *objectTableView->getSelectedObjectEntity();
            const ObjectEntity& newObjectEntity = objectController->cloneObjectEntity(newObjectName, toCloneObjectEntity);
            objectController->moveObjectInFrontOfCamera(newObjectEntity, true);

            int row = objectTableView->addObject(newObjectEntity);
            objectTableView->selectRow(row);
        }
    }

    void ObjectPanelWidget::showRenameObjectDialog() {
        std::string originalName = objectTableView->getSelectedObjectEntity()->getName();
        RenameObjectDialog renameObjectEntityDialog(this, originalName, objectController);
        renameObjectEntityDialog.exec();

        if (renameObjectEntityDialog.result() == QDialog::Accepted) {
            const std::string& newObjectName = renameObjectEntityDialog.getObjectName();
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            objectController->renameObjectEntity(objectEntity, newObjectName);

            objectTableView->updateSelectedObject(objectEntity);
        }
    }

    void ObjectPanelWidget::moveUpSelectedObject() const {
        if (objectTableView->hasObjectEntitySelected()) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            bool controllerMoved = objectController->moveUpObjectEntity(objectEntity);
            bool viewMoved = objectTableView->moveUpSelectedObject();

            if (controllerMoved != viewMoved) {
                Logger::instance().logError("Move up fail for: " + objectEntity.getName());
            }
        }
    }

    void ObjectPanelWidget::moveDownSelectedObject() const {
        if (objectTableView->hasObjectEntitySelected()) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            bool controllerMoved = objectController->moveDownObjectEntity(objectEntity);
            bool viewMoved = objectTableView->moveDownSelectedObject();

            if (controllerMoved != viewMoved) {
                Logger::instance().logError("Move down fail for: " + objectEntity.getName());
            }
        }
    }

    void ObjectPanelWidget::updateObjectOrientationType() const {
        if (!disableObjectEvent) {
            const ObjectEntity* objectEntity = objectTableView->getSelectedObjectEntity();

            QVariant variant = orientationType->currentData();
            auto newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());

            Quaternion<float> orientation = objectEntity->getModel()->getTransform().getOrientation();
            Vector3<float> eulerAngle = orientation.toEulerAngle(newRotationSequence);

            eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
            eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
            eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

            updateObjectTransform();
        }
    }

    void ObjectPanelWidget::updateObjectTransform() const {
        if (!disableObjectEvent) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();

            Vector3 eulerAngle(
                    AngleConverter<float>::toRadian((float)eulerAxis0->value()),
                    AngleConverter<float>::toRadian((float)eulerAxis1->value()),
                    AngleConverter<float>::toRadian((float)eulerAxis2->value())
            );

            QVariant variant = orientationType->currentData();
            auto rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());

            Transform newObjectEntityTransform(
                    Point3((float)positionX->value(),(float)positionY->value(),(float)positionZ->value()),
                    Quaternion<float>::fromEuler(eulerAngle, rotationSequence),
                    Vector3((float)scaleX->value(), (float)scaleY->value(), (float)scaleZ->value()));

            objectController->updateObjectTransform(objectEntity, newObjectEntityTransform);
        }
    }

    void ObjectPanelWidget::updateObjectScale() const {
        if (!disableObjectEvent) {
            updateObjectTransform();

            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            physicsWidget->load(objectEntity, *objectController); //refresh physics shape dimension
        }
    }

    void ObjectPanelWidget::updateObjectProperties() const {
        if (!disableObjectEvent) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();

            QVariant shadowBehaviorVariant = shadowBehavior->currentData();
            auto shadowBehaviorValue = static_cast<Model::ShadowBehavior>(shadowBehaviorVariant.toInt());

            QVariant cullBehaviorVariant = cullBehavior->currentData();
            auto cullBehaviorValue = static_cast<Model::CullBehavior>(cullBehaviorVariant.toInt());

            objectController->updateObjectProperties(objectEntity, shadowBehaviorValue, cullBehaviorValue);
        }
    }

    void ObjectPanelWidget::updateObjectTags() const {
        if (!disableObjectEvent) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();

            std::string tagsValues = tags->text().toUtf8().constData();
            objectController->updateObjectTags(objectEntity, tagsValues);
        }
    }

    void ObjectPanelWidget::rigidBodyToggled(int rigidBodyState) {
        if (!disableObjectEvent) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            if (Qt::CheckState::Checked == rigidBodyState) {
                physicsWidget->show();
                objectController->createDefaultBody(objectEntity);
            } else if (Qt::CheckState::Unchecked == rigidBodyState) {
                objectController->removeBody(objectEntity);
                physicsWidget->hide();
            }

            setupObjectPhysicsDataFrom(objectEntity);
        }
    }

    void ObjectPanelWidget::showChangeLightTypeDialog() {
        ChangeLightTypeDialog changeLightTypeDialog(this);
        changeLightTypeDialog.exec();

        if (changeLightTypeDialog.result() == QDialog::Accepted) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            std::optional<Light::LightType> lightType = changeLightTypeDialog.getLightType();

            objectController->changeLightType(objectEntity, lightType);
            setupObjectLightDataFrom(objectEntity);
        }
    }

    void ObjectPanelWidget::showChangeSoundDialog() {
        ChangeSoundDialog changeSoundDialog(this);
        changeSoundDialog.exec();

        if (changeSoundDialog.result() == QDialog::Accepted) {
            const ObjectEntity& objectEntity = *objectTableView->getSelectedObjectEntity();
            std::optional<Sound::SoundType> soundType = changeSoundDialog.getSoundType();
            std::string soundFilename = changeSoundDialog.getSoundFilename();

            objectController->changeSound(objectEntity, soundType, soundFilename);
            setupObjectSoundDataFrom(objectEntity);
        }
    }

}
