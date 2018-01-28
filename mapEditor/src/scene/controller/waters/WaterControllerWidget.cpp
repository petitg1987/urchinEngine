#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QMessageBox>

#include "WaterControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"
#include "scene/controller/waters/dialog/NewWaterDialog.h"

namespace urchin
{
    QString WaterControllerWidget::preferredNormalTexturePath = QString();

    WaterControllerWidget::WaterControllerWidget() :
            waterController(nullptr),
            disableWaterEvent(false)
    {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        waterTableView = new WaterTableView();
        mainLayout->addWidget(waterTableView);
        waterTableView->addObserver(this, WaterTableView::SELECTION_CHANGED);
        waterTableView->setFixedHeight(220);

        auto *buttonsLayout = new QHBoxLayout();
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
    }

    void WaterControllerWidget::setupGeneralPropertiesBox(QVBoxLayout *mainLayout)
    {
        generalPropertiesGroupBox = new QGroupBox("General Properties");
        mainLayout->addWidget(generalPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);
        generalPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        generalPropertiesGroupBox->hide();

        auto *generalPropertiesLayout = new QGridLayout(generalPropertiesGroupBox);

        QLabel *positionLabel= new QLabel("Position:");
        generalPropertiesLayout->addWidget(positionLabel, 0, 0);

        auto *positionLayout = new QHBoxLayout();
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

        QLabel *xSizeLabel= new QLabel("XZ size:");
        generalPropertiesLayout->addWidget(xSizeLabel, 1, 0);

        auto *xzSizeLayout = new QHBoxLayout();
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

        QLabel *waterColorLabel= new QLabel("Water color:");
        generalPropertiesLayout->addWidget(waterColorLabel, 2, 0);

        auto *waterColorLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(waterColorLayout, 2, 1, Qt::AlignmentFlag::AlignLeft);
        waterColorR = new QDoubleSpinBox();
        waterColorLayout->addWidget(waterColorR);
        SpinBoxStyleHelper::applyDefaultStyleOn(waterColorR);
        waterColorR->setMinimum(0.0);
        waterColorR->setMaximum(1.0);
        connect(waterColorR, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));
        waterColorG = new QDoubleSpinBox();
        waterColorLayout->addWidget(waterColorG);
        SpinBoxStyleHelper::applyDefaultStyleOn(waterColorG);
        waterColorG->setMinimum(0.0);
        waterColorG->setMaximum(1.0);
        connect(waterColorG, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));
        waterColorB = new QDoubleSpinBox();
        waterColorLayout->addWidget(waterColorB);
        SpinBoxStyleHelper::applyDefaultStyleOn(waterColorB);
        waterColorB->setMinimum(0.0);
        waterColorB->setMaximum(1.0);
        connect(waterColorB, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));

        QLabel *normalTextureLabel= new QLabel("Normal:");
        generalPropertiesLayout->addWidget(normalTextureLabel, 3, 0);

        auto *normalTextureLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(normalTextureLayout, 3, 1, Qt::AlignmentFlag::AlignLeft);
        normalTextureFilenameText = new QLineEdit();
        normalTextureLayout->addWidget(normalTextureFilenameText);
        normalTextureFilenameText->setReadOnly(true);

        QPushButton *selectNormalTextureFileButton = new QPushButton("...");
        normalTextureLayout->addWidget(selectNormalTextureFileButton);
        ButtonStyleHelper::applyNormalStyle(selectNormalTextureFileButton);
        selectNormalTextureFileButton->setFixedWidth(22);
        connect(selectNormalTextureFileButton, SIGNAL(clicked()), this, SLOT(showNormalTextureFilenameDialog()));

        QPushButton *clearNormalTextureFileButton = new QPushButton("Clr");
        normalTextureLayout->addWidget(clearNormalTextureFileButton);
        ButtonStyleHelper::applyNormalStyle(clearNormalTextureFileButton);
        clearNormalTextureFileButton->setFixedWidth(22);
        connect(clearNormalTextureFileButton, SIGNAL(clicked()), this, SLOT(clearNormalTextureFilename()));

        QLabel *repeatLabel= new QLabel("Repeat:");
        generalPropertiesLayout->addWidget(repeatLabel, 4, 0);

        auto *repeatLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(repeatLayout, 4, 1, Qt::AlignmentFlag::AlignLeft);
        sRepeat = new QDoubleSpinBox();
        repeatLayout->addWidget(sRepeat);
        SpinBoxStyleHelper::applyDefaultStyleOn(sRepeat);
        sRepeat->setMinimum(0.0);
        sRepeat->setSingleStep(1.0);
        connect(sRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));

        tRepeat = new QDoubleSpinBox();
        repeatLayout->addWidget(tRepeat);
        SpinBoxStyleHelper::applyDefaultStyleOn(tRepeat);
        tRepeat->setMinimum(0.0);
        tRepeat->setSingleStep(1.0);
        connect(tRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateWaterProperties()));
    }

    WaterTableView *WaterControllerWidget::getWaterTableView() const
    {
        return waterTableView;
    }

    void WaterControllerWidget::load(WaterController *waterController)
    {
        this->waterController = waterController;

        std::list<const SceneWater *> sceneWaters = waterController->getSceneWaters();
        for(std::list<const SceneWater *>::const_iterator it=sceneWaters.begin(); it!=sceneWaters.end(); ++it)
        {
            waterTableView->addWater((*it));
        }
    }

    void WaterControllerWidget::unload()
    {
        waterTableView->removeAllWaters();

        waterController = nullptr;
    }

    void WaterControllerWidget::notify(Observable *observable, int notificationType)
    {
        if(auto *waterTableView = dynamic_cast<WaterTableView *>(observable))
        {
            switch(notificationType)
            {
                case WaterTableView::SELECTION_CHANGED:
                    if(waterTableView->hasSceneWaterSelected())
                    {
                        const SceneWater *sceneWater = waterTableView->getSelectedSceneWater();
                        setupWaterDataFrom(sceneWater);

                        removeWaterButton->setEnabled(true);
                        generalPropertiesGroupBox->show();
                    }else
                    {
                        removeWaterButton->setEnabled(false);
                        generalPropertiesGroupBox->hide();
                    }
                    break;
                default:
                    ;
            }
        }
    }

    void WaterControllerWidget::setupWaterDataFrom(const SceneWater *sceneWater)
    {
        disableWaterEvent = true;
        const Water *water = sceneWater->getWater();

        this->positionX->setValue(water->getCenterPosition().X);
        this->positionY->setValue(water->getCenterPosition().Y);
        this->positionZ->setValue(water->getCenterPosition().Z);

        this->xSize->setValue(water->getXSize());
        this->zSize->setValue(water->getZSize());

        this->waterColorR->setValue(water->getWaterColor().X);
        this->waterColorG->setValue(water->getWaterColor().Y);
        this->waterColorB->setValue(water->getWaterColor().Z);

        this->normalTextureFilenameText->setText(QString::fromStdString(water->getNormalTexture()->getName()));

        this->sRepeat->setValue(water->getSRepeat());
        this->tRepeat->setValue(water->getTRepeat());

        disableWaterEvent = false;
    }

    void WaterControllerWidget::showAddWaterDialog()
    {
        NewWaterDialog newSceneWaterDialog(this, waterController);
        newSceneWaterDialog.exec();

        if(newSceneWaterDialog.result()==QDialog::Accepted)
        {
            SceneWater *sceneWater = newSceneWaterDialog.getSceneWater();
            waterController->addSceneWater(sceneWater);

            waterTableView->addWater(sceneWater);
        }
    }

    void WaterControllerWidget::removeSelectedWater()
    {
        if(waterTableView->hasSceneWaterSelected())
        {
            const SceneWater *sceneWater = waterTableView->getSelectedSceneWater();
            waterController->removeSceneWater(sceneWater);

            waterTableView->removeSelectedWater();
        }
    }

    void WaterControllerWidget::updateWaterProperties()
    {
        if(!disableWaterEvent)
        {
            const SceneWater *sceneWater = waterTableView->getSelectedSceneWater();

            Point3<float> position(positionX->value(), positionY->value(), positionZ->value());
            Vector3<float> waterColor(waterColorR->value(), waterColorG->value(), waterColorB->value());
            std::string normalTextureFilename = normalTextureFilenameText->text().toStdString();
            waterController->updateSceneWater(sceneWater, position, xSize->value(), zSize->value(), waterColor, normalTextureFilename, sRepeat->value(), tRepeat->value());
        }
    }

    void WaterControllerWidget::showNormalTextureFilenameDialog()
    {
        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredNormalTexturePath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredNormalTexturePath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png)", nullptr, QFileDialog::DontUseNativeDialog);
        if(!filename.isNull())
        {
            std::string tgaFilenamePath = filename.toUtf8().constData();
            std::string relativeTgaFilenamePath = FileHandler::getRelativePath(resourcesDirectory, tgaFilenamePath);
            this->normalTextureFilenameText->setText(QString::fromStdString(relativeTgaFilenamePath));

            std::string preferredPathString = FileHandler::getDirectoryFrom(tgaFilenamePath);
            preferredNormalTexturePath = QString::fromStdString(preferredPathString);

            try
            {
                updateWaterProperties();
            }catch(std::exception &e)
            {
                QMessageBox::critical(this, "Error", e.what());
                clearNormalTextureFilename();
            }
        }
    }

    void WaterControllerWidget::clearNormalTextureFilename()
    {
        this->normalTextureFilenameText->setText("");

        updateWaterProperties();
    }
}
