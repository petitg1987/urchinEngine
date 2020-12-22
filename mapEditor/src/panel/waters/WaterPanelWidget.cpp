#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QMessageBox>

#include "WaterPanelWidget.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/SpinBoxStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"
#include "panel/waters/dialog/NewWaterDialog.h"

namespace urchin {
    QString WaterPanelWidget::preferredNormalTexturePath = QString();
    QString WaterPanelWidget::preferredDudvMapPath = QString();

    WaterPanelWidget::WaterPanelWidget() :
            waterController(nullptr),
            waterTableView(nullptr),
            addWaterButton(nullptr),
            removeWaterButton(nullptr),
            generalPropertiesGroupBox(nullptr),
            waterSurfacePropertiesGroupBox(nullptr),
            underWaterPropertiesGroupBox(nullptr),
            disableWaterEvent(false),
            positionX(nullptr), positionY(nullptr), positionZ(nullptr),
            xSize(nullptr), zSize(nullptr),
            waterColorR(nullptr), waterColorG(nullptr), waterColorB(nullptr),
            normalTextureFilenameText(nullptr), dudvMapFilenameText(nullptr),
            waveSpeed(nullptr), waveStrength(nullptr), sRepeat(nullptr), tRepeat(nullptr),
            density(nullptr), gradient(nullptr) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        waterTableView = new WaterTableView();
        mainLayout->addWidget(waterTableView);
        waterTableView->addObserver(this, WaterTableView::SELECTION_CHANGED);
        waterTableView->setFixedHeight(220);

        auto* buttonsLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonsLayout);
        buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        addWaterButton = new QPushButton("New Water");
        buttonsLayout->addWidget(addWaterButton);
        ButtonStyleHelper::applyNormalStyle(addWaterButton);
        connect(addWaterButton, SIGNAL(clicked()), this, SLOT(showAddWaterDialog()));

        removeWaterButton = new QPushButton("Remove Water");
        buttonsLayout->addWidget(removeWaterButton);
        ButtonStyleHelper::applyNormalStyle(removeWaterButton);
        removeWaterButton->setEnabled(false);
        connect(removeWaterButton, SIGNAL(clicked()), this, SLOT(removeSelectedWater()));

        setupGeneralPropertiesBox(mainLayout);
        setupWaterSurfaceProperties(mainLayout);
        setupUnderWaterProperties(mainLayout);
    }

    void WaterPanelWidget::setupGeneralPropertiesBox(QVBoxLayout* mainLayout) {
        generalPropertiesGroupBox = new QGroupBox("General Properties");
        mainLayout->addWidget(generalPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);
        generalPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        generalPropertiesGroupBox->hide();

        auto* generalPropertiesLayout = new QGridLayout(generalPropertiesGroupBox);

        auto *positionLabel= new QLabel("Position:");
        generalPropertiesLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(positionLayout, 0, 1, Qt::AlignmentFlag::AlignLeft);
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));

        auto *xSizeLabel= new QLabel("XZ size:");
        generalPropertiesLayout->addWidget(xSizeLabel, 1, 0);

        auto* xzSizeLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(xzSizeLayout, 1, 1, Qt::AlignmentFlag::AlignLeft);
        xSize = new QDoubleSpinBox();
        xzSizeLayout->addWidget(xSize);
        SpinBoxStyleHelper::applyDefaultStyleOn(xSize);
        xSize->setMinimum(0.0);
        xSize->setSingleStep(1.0);
        connect(xSize, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));
        zSize = new QDoubleSpinBox();
        xzSizeLayout->addWidget(zSize);
        SpinBoxStyleHelper::applyDefaultStyleOn(zSize);
        zSize->setMinimum(0.0);
        zSize->setSingleStep(1.0);
        connect(zSize, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));
    }

    void WaterPanelWidget::setupWaterSurfaceProperties(QVBoxLayout* mainLayout) {
        waterSurfacePropertiesGroupBox = new QGroupBox("Surface");
        mainLayout->addWidget(waterSurfacePropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(waterSurfacePropertiesGroupBox);
        waterSurfacePropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        waterSurfacePropertiesGroupBox->hide();

        auto* waterSurfacePropertiesLayout = new QGridLayout(waterSurfacePropertiesGroupBox);

        auto *waterColorLabel= new QLabel("Water color:");
        waterSurfacePropertiesLayout->addWidget(waterColorLabel, 0, 0);

        auto* waterColorLayout = new QHBoxLayout();
        waterSurfacePropertiesLayout->addLayout(waterColorLayout, 0, 1, Qt::AlignmentFlag::AlignLeft);
        waterColorR = new QDoubleSpinBox();
        waterColorLayout->addWidget(waterColorR);
        SpinBoxStyleHelper::applyDefaultStyleOn(waterColorR);
        waterColorR->setMinimum(0.0);
        waterColorR->setMaximum(1.0);
        connect(waterColorR, SIGNAL(valueChanged(double)), this, SLOT(updateSurfaceWaterProperties()));
        waterColorG = new QDoubleSpinBox();
        waterColorLayout->addWidget(waterColorG);
        SpinBoxStyleHelper::applyDefaultStyleOn(waterColorG);
        waterColorG->setMinimum(0.0);
        waterColorG->setMaximum(1.0);
        connect(waterColorG, SIGNAL(valueChanged(double)), this, SLOT(updateSurfaceWaterProperties()));
        waterColorB = new QDoubleSpinBox();
        waterColorLayout->addWidget(waterColorB);
        SpinBoxStyleHelper::applyDefaultStyleOn(waterColorB);
        waterColorB->setMinimum(0.0);
        waterColorB->setMaximum(1.0);
        connect(waterColorB, SIGNAL(valueChanged(double)), this, SLOT(updateSurfaceWaterProperties()));

        auto *normalTextureLabel= new QLabel("Normal:");
        waterSurfacePropertiesLayout->addWidget(normalTextureLabel, 2, 0);

        auto* normalTextureLayout = new QHBoxLayout();
        waterSurfacePropertiesLayout->addLayout(normalTextureLayout, 2, 1, Qt::AlignmentFlag::AlignLeft);
        normalTextureFilenameText = new QLineEdit();
        normalTextureLayout->addWidget(normalTextureFilenameText);
        normalTextureFilenameText->setReadOnly(true);

        auto* selectNormalTextureFileButton = new QPushButton("...");
        normalTextureLayout->addWidget(selectNormalTextureFileButton);
        ButtonStyleHelper::applyNormalStyle(selectNormalTextureFileButton);
        selectNormalTextureFileButton->setFixedWidth(22);
        connect(selectNormalTextureFileButton, SIGNAL(clicked()), this, SLOT(showNormalTextureFilenameDialog()));

        auto* clearNormalTextureFileButton = new QPushButton("Clr");
        normalTextureLayout->addWidget(clearNormalTextureFileButton);
        ButtonStyleHelper::applyNormalStyle(clearNormalTextureFileButton);
        clearNormalTextureFileButton->setFixedWidth(22);
        connect(clearNormalTextureFileButton, SIGNAL(clicked()), this, SLOT(clearNormalTextureFilename()));

        auto *dudvMapLabel= new QLabel("Du-dv map:");
        waterSurfacePropertiesLayout->addWidget(dudvMapLabel, 3, 0);

        auto* dudvMapLayout = new QHBoxLayout();
        waterSurfacePropertiesLayout->addLayout(dudvMapLayout, 3, 1, Qt::AlignmentFlag::AlignLeft);
        dudvMapFilenameText = new QLineEdit();
        dudvMapLayout->addWidget(dudvMapFilenameText);
        dudvMapFilenameText->setReadOnly(true);

        auto* selectDudvMapFileButton = new QPushButton("...");
        dudvMapLayout->addWidget(selectDudvMapFileButton);
        ButtonStyleHelper::applyNormalStyle(selectDudvMapFileButton);
        selectDudvMapFileButton->setFixedWidth(22);
        connect(selectDudvMapFileButton, SIGNAL(clicked()), this, SLOT(showDudvMapFilenameDialog()));

        auto* clearDudvMapFileButton = new QPushButton("Clr");
        dudvMapLayout->addWidget(clearDudvMapFileButton);
        ButtonStyleHelper::applyNormalStyle(clearDudvMapFileButton);
        clearDudvMapFileButton->setFixedWidth(22);
        connect(clearDudvMapFileButton, SIGNAL(clicked()), this, SLOT(clearDudvMapFilename()));

        auto *waveSpeedLabel= new QLabel("Wave speed:");
        waterSurfacePropertiesLayout->addWidget(waveSpeedLabel, 4, 0);

        waveSpeed = new QDoubleSpinBox();
        waterSurfacePropertiesLayout->addWidget(waveSpeed, 4, 1, 1, 3, Qt::AlignLeft);
        SpinBoxStyleHelper::applyDefaultStyleOn(waveSpeed);
        waveSpeed->setMinimum(0.0);
        waveSpeed->setSingleStep(0.01);
        connect(waveSpeed, SIGNAL(valueChanged(double)), this, SLOT(updateSurfaceWaterProperties()));

        auto *waveStrengthLabel= new QLabel("Wave strength:");
        waterSurfacePropertiesLayout->addWidget(waveStrengthLabel, 5, 0);

        waveStrength = new QDoubleSpinBox();
        waterSurfacePropertiesLayout->addWidget(waveStrength, 5, 1, 1, 3, Qt::AlignLeft);
        SpinBoxStyleHelper::applyDefaultStyleOn(waveStrength);
        waveStrength->setMinimum(0.0);
        waveStrength->setSingleStep(0.01);
        connect(waveStrength, SIGNAL(valueChanged(double)), this, SLOT(updateSurfaceWaterProperties()));

        auto *repeatLabel= new QLabel("Repeat:");
        waterSurfacePropertiesLayout->addWidget(repeatLabel, 6, 0);

        auto* repeatLayout = new QHBoxLayout();
        waterSurfacePropertiesLayout->addLayout(repeatLayout, 6, 1, Qt::AlignmentFlag::AlignLeft);
        sRepeat = new QDoubleSpinBox();
        repeatLayout->addWidget(sRepeat);
        SpinBoxStyleHelper::applyDefaultStyleOn(sRepeat);
        sRepeat->setMinimum(0.0);
        sRepeat->setSingleStep(1.0);
        connect(sRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateSurfaceWaterProperties()));

        tRepeat = new QDoubleSpinBox();
        repeatLayout->addWidget(tRepeat);
        SpinBoxStyleHelper::applyDefaultStyleOn(tRepeat);
        tRepeat->setMinimum(0.0);
        tRepeat->setSingleStep(1.0);
        connect(tRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateSurfaceWaterProperties()));
    }

    void WaterPanelWidget::setupUnderWaterProperties(QVBoxLayout* mainLayout) {
        underWaterPropertiesGroupBox = new QGroupBox("Under Water");
        mainLayout->addWidget(underWaterPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(underWaterPropertiesGroupBox);
        underWaterPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        underWaterPropertiesGroupBox->hide();

        auto* underWaterPropertiesLayout = new QGridLayout(underWaterPropertiesGroupBox);

        auto *densityLabel= new QLabel("Density:");
        underWaterPropertiesLayout->addWidget(densityLabel, 0, 0);

        density = new QDoubleSpinBox();
        underWaterPropertiesLayout->addWidget(density, 0, 1, 1, 3, Qt::AlignLeft);
        SpinBoxStyleHelper::applyDefaultStyleOn(density);
        density->setMinimum(0.0);
        connect(density, SIGNAL(valueChanged(double)), this, SLOT(updateUnderWaterProperties()));

        auto *gradientLabel= new QLabel("Gradient:");
        underWaterPropertiesLayout->addWidget(gradientLabel, 1, 0);

        gradient = new QDoubleSpinBox();
        underWaterPropertiesLayout->addWidget(gradient, 1, 1, 1, 3, Qt::AlignLeft);
        SpinBoxStyleHelper::applyDefaultStyleOn(gradient);
        gradient->setMinimum(0.0);
        connect(gradient, SIGNAL(valueChanged(double)), this, SLOT(updateUnderWaterProperties()));
    }

    WaterTableView* WaterPanelWidget::getWaterTableView() const {
        return waterTableView;
    }

    void WaterPanelWidget::load(WaterController* waterController) {
        this->waterController = waterController;

        std::list<const SceneWater*> sceneWaters = waterController->getSceneWaters();
        for (auto& sceneWater : sceneWaters) {
            waterTableView->addWater(sceneWater);
        }
    }

    void WaterPanelWidget::unload() {
        waterTableView->removeAllWaters();

        waterController = nullptr;
    }

    void WaterPanelWidget::notify(Observable* observable, int notificationType) {
        if (auto* waterTableView = dynamic_cast<WaterTableView*>(observable)) {
            if (notificationType == WaterTableView::SELECTION_CHANGED) {
                if (waterTableView->hasSceneWaterSelected()) {
                    const SceneWater* sceneWater = waterTableView->getSelectedSceneWater();
                    setupWaterDataFrom(sceneWater);

                    removeWaterButton->setEnabled(true);
                    generalPropertiesGroupBox->show();
                    waterSurfacePropertiesGroupBox->show();
                    underWaterPropertiesGroupBox->show();
                } else {
                    removeWaterButton->setEnabled(false);
                    generalPropertiesGroupBox->hide();
                    waterSurfacePropertiesGroupBox->hide();
                    underWaterPropertiesGroupBox->hide();
                }
            }
        }
    }

    void WaterPanelWidget::setupWaterDataFrom(const SceneWater* sceneWater) {
        disableWaterEvent = true;
        const Water* water = sceneWater->getWater();

        this->positionX->setValue(water->getCenterPosition().X);
        this->positionY->setValue(water->getCenterPosition().Y);
        this->positionZ->setValue(water->getCenterPosition().Z);

        this->xSize->setValue(water->getXSize());
        this->zSize->setValue(water->getZSize());

        this->waterColorR->setValue(water->getWaterColor().X);
        this->waterColorG->setValue(water->getWaterColor().Y);
        this->waterColorB->setValue(water->getWaterColor().Z);

        this->normalTextureFilenameText->setText(QString::fromStdString(water->getNormalFilename()));
        this->dudvMapFilenameText->setText(QString::fromStdString(water->getDudvFilename()));

        this->waveSpeed->setValue(water->getWaveSpeed());
        this->waveStrength->setValue(water->getWaveStrength());

        this->sRepeat->setValue(water->getSRepeat());
        this->tRepeat->setValue(water->getTRepeat());

        this->density->setValue(water->getDensity());
        this->gradient->setValue(water->getGradient());

        disableWaterEvent = false;
    }

    void WaterPanelWidget::showAddWaterDialog() {
        NewWaterDialog newSceneWaterDialog(this, waterController);
        newSceneWaterDialog.exec();

        if (newSceneWaterDialog.result() == QDialog::Accepted) {
            SceneWater* sceneWater = newSceneWaterDialog.getSceneWater();
            waterController->addSceneWater(sceneWater);

            waterTableView->addWater(sceneWater);
        }
    }

    void WaterPanelWidget::removeSelectedWater() {
        if (waterTableView->hasSceneWaterSelected()) {
            const SceneWater* sceneWater = waterTableView->getSelectedSceneWater();
            waterController->removeSceneWater(sceneWater);

            waterTableView->removeSelectedWater();
        }
    }

    void WaterPanelWidget::updateWaterProperties() {
        if (!disableWaterEvent) {
            const SceneWater* sceneWater = waterTableView->getSelectedSceneWater();

            Point3<float> position((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
            waterController->updateSceneWaterGeneral(sceneWater, position, (float)xSize->value(), (float)zSize->value());
        }
    }

    void WaterPanelWidget::updateSurfaceWaterProperties() {
        if (!disableWaterEvent) {
            const SceneWater* sceneWater = waterTableView->getSelectedSceneWater();

            Vector3<float> waterColor((float)waterColorR->value(), (float)waterColorG->value(), (float)waterColorB->value());
            std::string normalTextureFilename = normalTextureFilenameText->text().toStdString();
            std::string dudvMapFilename = dudvMapFilenameText->text().toStdString();
            waterController->updateSceneWaterSurface(sceneWater, waterColor, normalTextureFilename, dudvMapFilename, (float)waveSpeed->value(), (float)waveStrength->value(),
                                                     (float)sRepeat->value(), (float)tRepeat->value());
        }
    }

    void WaterPanelWidget::updateUnderWaterProperties() {
        if (!disableWaterEvent) {
            const SceneWater* sceneWater = waterTableView->getSelectedSceneWater();

            waterController->updateSceneWaterUnderWater(sceneWater, (float)density->value(), (float)gradient->value());
        }
    }

    void WaterPanelWidget::showNormalTextureFilenameDialog() {
        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredNormalTexturePath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredNormalTexturePath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string imageFilenamePath = filename.toUtf8().constData();
            std::string relativeTgaFilenamePath = FileUtil::getRelativePath(resourcesDirectory, imageFilenamePath);
            this->normalTextureFilenameText->setText(QString::fromStdString(relativeTgaFilenamePath));

            std::string preferredPathString = FileUtil::getDirectoryFrom(imageFilenamePath);
            preferredNormalTexturePath = QString::fromStdString(preferredPathString);

            try {
                updateSurfaceWaterProperties();
            } catch (std::exception& e) {
                QMessageBox::critical(this, "Error", e.what());
                clearNormalTextureFilename();
            }
        }
    }

    void WaterPanelWidget::clearNormalTextureFilename() {
        this->normalTextureFilenameText->setText("");

        updateSurfaceWaterProperties();
    }

    void WaterPanelWidget::showDudvMapFilenameDialog() {
        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredDudvMapPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredDudvMapPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string tgaFilenamePath = filename.toUtf8().constData();
            std::string relativeTgaFilenamePath = FileUtil::getRelativePath(resourcesDirectory, tgaFilenamePath);
            this->dudvMapFilenameText->setText(QString::fromStdString(relativeTgaFilenamePath));

            std::string preferredPathString = FileUtil::getDirectoryFrom(tgaFilenamePath);
            preferredDudvMapPath = QString::fromStdString(preferredPathString);

            try {
                updateSurfaceWaterProperties();
            } catch (std::exception& e) {
                QMessageBox::critical(this, "Error", e.what());
                clearDudvMapFilename();
            }
        }
    }

    void WaterPanelWidget::clearDudvMapFilename() {
        this->dudvMapFilenameText->setText("");

        updateSurfaceWaterProperties();
    }
}
