#include <QMessageBox>
#include <UrchinAggregation.h>

#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/SpinBoxStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"
#include "widget/style/ComboBoxStyleHelper.h"
#include "panel/object/ObjectPanelWidget.h"

#include "panel/object/dialog/AddObjectDialog.h"
#include "panel/object/dialog/CloneObjectDialog.h"
#include "panel/object/dialog/UpdateObjectDialog.h"
#include "panel/object/dialog/ChangeLightTypeDialog.h"
#include "panel/object/dialog/ChangeMeshesFileDialog.h"
#include "panel/object/dialog/ChangeSoundDialog.h"
#include "scene/object/ObjectMoveController.h"
#include "scene/SceneDisplayerWindow.h"

namespace urchin {

    ObjectPanelWidget::ObjectPanelWidget() :
            objectController(nullptr),
            objectTableView(nullptr),
            addObjectButton(nullptr),
            removeObjectButton(nullptr),
            cloneObjectButton(nullptr),
            updateObjectButton(nullptr),
            tabWidget(nullptr),
            tabSelected(ObjectTab::MODEL),
            objectEntitySelected(nullptr),
            disableObjectEvent(false),
            meshesFile(nullptr),
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
            lightWidget(nullptr),
            soundTypeValueLabel(nullptr),
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

        updateObjectButton = new QPushButton("Update");
        buttonsLayout->addWidget(updateObjectButton);
        ButtonStyleHelper::applyNormalStyle(updateObjectButton);
        updateObjectButton->setEnabled(false);
        connect(updateObjectButton, SIGNAL(clicked()), this, SLOT(showUpdateObjectDialog()));

        tabWidget = new QTabWidget();
        mainLayout->addWidget(tabWidget);
        tabWidget->hide();

        //model properties
        auto* tabModel = new QWidget();
        auto* modelLayout = new QVBoxLayout(tabModel);
        modelLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        modelLayout->setContentsMargins(1, 1, 1, 1);
        setupMeshesBox(modelLayout);
        setupTransformBox(modelLayout);
        setupPropertiesBox(modelLayout);
        tabWidget->addTab(tabModel, "Model");

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

    void ObjectPanelWidget::setupMeshesBox(QVBoxLayout* modelLayout) {
        auto* meshGroupBox = new QGroupBox("Meshes");
        modelLayout->addWidget(meshGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(meshGroupBox);
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* meshesFileLayout = new QHBoxLayout(meshGroupBox);
        meshesFileLayout->setAlignment(Qt::AlignLeft);
        meshesFileLayout->setSpacing(15);

        auto* meshesFilenameLabel = new QLabel("Filename:");
        meshesFileLayout->addWidget(meshesFilenameLabel);

        meshesFile = new QLabel();
        meshesFileLayout->addWidget(meshesFile);

        auto* changeMeshesFileButton = new QPushButton("Change");
        meshesFileLayout->addWidget(changeMeshesFileButton);
        ButtonStyleHelper::applyNormalStyle(changeMeshesFileButton);
        connect(changeMeshesFileButton, SIGNAL(clicked()), this, SLOT(showChangeMeshesFileDialog()));
    }

    void ObjectPanelWidget::setupTransformBox(QVBoxLayout* modelLayout) {
        auto* transformGroupBox = new QGroupBox("Transform");
        modelLayout->addWidget(transformGroupBox);
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

    void ObjectPanelWidget::setupPropertiesBox(QVBoxLayout* modelLayout) {
        auto* propertiesGroupBox = new QGroupBox("Properties");
        modelLayout->addWidget(propertiesGroupBox);
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

        auto* changeLightTypeButton = new QPushButton("Change");
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

        auto* changeSoundTypeButton = new QPushButton("Change");
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

        for (auto& objectEntity : this->objectController->getObjectEntities()) {
            objectTableView->addObjectEntity(*objectEntity, false);
        }
    }

    void ObjectPanelWidget::unload() {
        objectTableView->removeAll();

        objectController = nullptr;
    }

    void ObjectPanelWidget::notify(Observable* observable, int notificationType) {
        if (const auto* objectTableView = dynamic_cast<ObjectTableView*>(observable)) {
            if (notificationType == ObjectTableView::OBJECT_SELECTION_CHANGED) {
                bool hasMainObjectEntitySelected = objectTableView->hasMainObjectEntitySelected();
                bool hasObjectsEntitiesSelected = !objectTableView->getAllSelectedObjectEntities().empty();
                if (hasMainObjectEntitySelected) {
                    const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();
                    objectEntitySelected = &objectEntity;
                    setupObjectDataFrom(objectEntity);
                    tabWidget->show();
                } else {
                    objectEntitySelected = nullptr;
                    tabWidget->hide();
                }

                removeObjectButton->setEnabled(hasObjectsEntitiesSelected);
                cloneObjectButton->setEnabled(hasMainObjectEntitySelected);
                updateObjectButton->setEnabled(hasMainObjectEntitySelected);
            }
        } else if (const auto* sceneDisplayerWidget = dynamic_cast<SceneDisplayerWindow*>(observable)) {
            if (notificationType == SceneDisplayerWindow::BODY_PICKED) {
                const std::string& bodyId = sceneDisplayerWidget->getLastPickedBodyId();
                const ObjectEntity* objectEntity = bodyId.empty() ? nullptr : objectController->findObjectEntityByBodyId(bodyId);
                if (objectEntity) {
                    this->objectTableView->selectObjectEntity(*objectEntity);
                } else {
                    this->objectTableView->clearSelection();
                }
            }
        } else if (const auto* objectMoveController = dynamic_cast<ObjectMoveController*>(observable)) {
            if (notificationType == ObjectMoveController::OBJECT_MOVED) {
                if (objectEntitySelected == &objectMoveController->getLastMovedObjectEntity()) {
                    setupObjectDataFrom(objectMoveController->getLastMovedObjectEntity());
                }
            }
        }
    }

    void ObjectPanelWidget::setupObjectDataFrom(const ObjectEntity& objectEntity) {
        setupObjectModelDataFrom(objectEntity);
        setupObjectPhysicsDataFrom(objectEntity);
        setupObjectLightDataFrom(objectEntity);
        setupObjectSoundDataFrom(objectEntity);
        setupObjectTagsDataFrom(objectEntity);
    }

    void ObjectPanelWidget::setupObjectModelDataFrom(const ObjectEntity& objectEntity) {
        disableObjectEvent = true;

        const Model* model = objectEntity.getModel();
        const ConstMeshes* constMeshes = objectEntity.getModel()->getConstMeshes();
        const Transform<float>& modelTransform = model->getTransform();

        if (constMeshes) {
            meshesFile->setText(QString::fromStdString(constMeshes->getMeshesName()));
            meshesFile->setToolTip(QString::fromStdString(PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), constMeshes->getMeshesFilename())));
        } else {
            meshesFile->setText("/");
            meshesFile->setToolTip("");
        }

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
        std::string tagsValues = StringUtil::join(objectEntity.getTags(), ObjectController::TAGS_SEPARATOR);
        this->tags->setText(QString::fromStdString(tagsValues));
        disableObjectEvent = false;
    }

    void ObjectPanelWidget::showAddObjectDialog() {
        std::vector<std::string> defaultGroupHierarchy;
        if (objectTableView->hasMainObjectEntitySelected()) {
            defaultGroupHierarchy = objectTableView->getMainSelectedObjectEntity()->getGroupHierarchy();
        }
        AddObjectDialog addObjectEntityDialog(this, objectController, defaultGroupHierarchy);
        addObjectEntityDialog.exec();

        if (addObjectEntityDialog.result() == QDialog::Accepted) {
            std::unique_ptr<ObjectEntity> objectEntity = addObjectEntityDialog.moveObjectEntity();
            ObjectEntity& objectEntityInserted = objectController->addObjectEntity(std::move(objectEntity));
            objectController->createDefaultBody(objectEntityInserted);
            objectController->moveObjectInFrontOfCamera(objectEntityInserted, false);

            objectTableView->addObjectEntity(objectEntityInserted, true);
        }
    }

    void ObjectPanelWidget::removeSelectedObject() {
        std::vector<const ObjectEntity*> selectedEntityObjects = objectTableView->getAllSelectedObjectEntities();
        if (selectedEntityObjects.size() > 1) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete all the selected objects ?");
            if (reply != QMessageBox::Yes) {
                return;
            }
        }

        for (const ObjectEntity* selectedEntityObject : selectedEntityObjects) {
            objectController->removeObjectEntity(*selectedEntityObject);
        }
        objectTableView->removeSelectedItems();
    }

    void ObjectPanelWidget::showCloneObjectDialog() {
        std::string originalName = objectTableView->getMainSelectedObjectEntity()->getName();
        CloneObjectDialog cloneObjectEntityDialog(this, originalName, objectController);
        cloneObjectEntityDialog.exec();

        if (cloneObjectEntityDialog.result() == QDialog::Accepted) {
            const std::string& newObjectName = cloneObjectEntityDialog.getObjectName();
            const ObjectEntity& toCloneObjectEntity = *objectTableView->getMainSelectedObjectEntity();
            ObjectEntity& objectEntityInserted = objectController->cloneObjectEntity(newObjectName, toCloneObjectEntity);
            objectController->moveObjectInFrontOfCamera(objectEntityInserted, true);

            objectTableView->addObjectEntity(objectEntityInserted, true);
        }
    }

    void ObjectPanelWidget::showUpdateObjectDialog() {
        std::string originalName = objectTableView->getMainSelectedObjectEntity()->getName();
        UpdateObjectDialog updateObjectDialog(this, originalName, objectController);
        updateObjectDialog.exec();

        if (updateObjectDialog.result() == QDialog::Accepted) {
            const std::string& newObjectName = updateObjectDialog.getObjectName();
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();
            objectController->renameObjectEntity(objectEntity, newObjectName);

            objectTableView->refreshMainSelectedObjectEntity();
        }
    }

    void ObjectPanelWidget::updateObjectOrientationType() const {
        if (!disableObjectEvent) {
            const ObjectEntity* objectEntity = objectTableView->getMainSelectedObjectEntity();

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
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();

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

            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();
            physicsWidget->load(objectEntity, *objectController); //refresh physics shape dimension
        }
    }

    void ObjectPanelWidget::updateObjectProperties() const {
        if (!disableObjectEvent) {
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();

            QVariant shadowBehaviorVariant = shadowBehavior->currentData();
            auto shadowBehaviorValue = static_cast<Model::ShadowBehavior>(shadowBehaviorVariant.toInt());

            QVariant cullBehaviorVariant = cullBehavior->currentData();
            auto cullBehaviorValue = static_cast<Model::CullBehavior>(cullBehaviorVariant.toInt());

            objectController->updateObjectProperties(objectEntity, shadowBehaviorValue, cullBehaviorValue);
        }
    }

    void ObjectPanelWidget::updateObjectTags() const {
        if (!disableObjectEvent) {
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();

            std::string tagsValues = tags->text().toUtf8().constData();
            objectController->updateObjectTags(objectEntity, tagsValues);
        }
    }

    void ObjectPanelWidget::showChangeMeshesFileDialog() {
        ChangeMeshesFileDialog changeMeshesFileDialog(this);
        changeMeshesFileDialog.exec();

        if (changeMeshesFileDialog.result() == QDialog::Accepted) {
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();
            std::string meshesFilename = changeMeshesFileDialog.getMeshesFilename();

            objectController->changeMeshesFile(objectEntity, meshesFilename);
            setupObjectModelDataFrom(objectEntity);
        }
    }

    void ObjectPanelWidget::rigidBodyToggled(int rigidBodyState) {
        if (!disableObjectEvent) {
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();
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
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();
            std::optional<Light::LightType> lightType = changeLightTypeDialog.getLightType();

            objectController->changeLightType(objectEntity, lightType);
            setupObjectLightDataFrom(objectEntity);
        }
    }

    void ObjectPanelWidget::showChangeSoundDialog() {
        ChangeSoundDialog changeSoundDialog(this);
        changeSoundDialog.exec();

        if (changeSoundDialog.result() == QDialog::Accepted) {
            const ObjectEntity& objectEntity = *objectTableView->getMainSelectedObjectEntity();
            std::optional<Sound::SoundType> soundType = changeSoundDialog.getSoundType();
            std::string soundFilename = changeSoundDialog.getSoundFilename();

            objectController->changeSound(objectEntity, soundType, soundFilename);
            setupObjectSoundDataFrom(objectEntity);
        }
    }

}
