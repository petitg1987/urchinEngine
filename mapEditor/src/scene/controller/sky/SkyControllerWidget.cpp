#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QMessageBox>

#include "SkyControllerWidget.h"
#include "widget/style/ButtonStyleHelper.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin
{

    QString SkyControllerWidget::preferredSkyboxPath = QString();

    SkyControllerWidget::SkyControllerWidget() :
            skyController(nullptr),
            xnSkyboxFilenameText(nullptr),
            xpSkyboxFilenameText(nullptr),
            ynSkyboxFilenameText(nullptr),
            ypSkyboxFilenameText(nullptr),
            znSkyboxFilenameText(nullptr),
            zpSkyboxFilenameText(nullptr),
            offsetY(nullptr),
            disableSkyEvent(false)
    {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        setupSkyboxBox(mainLayout);
    }

    void SkyControllerWidget::load(SkyController *skyController)
    {
        this->skyController = skyController;

        auto &skybox = skyController->getSceneSky()->getSkybox();
        setupSkyDataFrom(skybox);
    }

    void SkyControllerWidget::unload()
    {
        skyController = nullptr;
    }

    void SkyControllerWidget::setupSkyboxBox(QVBoxLayout *mainLayout)
    {
        auto *skyboxGroupBox = new QGroupBox("Skybox");
        mainLayout->addWidget(skyboxGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(skyboxGroupBox);
        skyboxGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto *skyboxLayout = new QGridLayout(skyboxGroupBox);

        auto *xnSkyboxLayout = createFilenameInputText(skyboxLayout, 0, "X-:", &xnSkyboxFilenameText);
        auto *selectXnSkyboxFileButton = createSelectFileButton(xnSkyboxLayout);
        connect(selectXnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showXnSkyboxFilenameDialog()));
        auto *clearXnSkyboxFileButton = createClearFileButton(xnSkyboxLayout);
        connect(clearXnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearXnSkyboxFilename()));

        auto *xpSkyboxLayout = createFilenameInputText(skyboxLayout, 1, "X+:", &xpSkyboxFilenameText);
        auto *selectXpSkyboxFileButton = createSelectFileButton(xpSkyboxLayout);
        connect(selectXpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showXpSkyboxFilenameDialog()));
        auto *clearXpSkyboxFileButton = createClearFileButton(xpSkyboxLayout);
        connect(clearXpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearXpSkyboxFilename()));

        auto *ynSkyboxLayout = createFilenameInputText(skyboxLayout, 2, "Y-:", &ynSkyboxFilenameText);
        auto *selectYnSkyboxFileButton = createSelectFileButton(ynSkyboxLayout);
        connect(selectYnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showYnSkyboxFilenameDialog()));
        auto *clearYnSkyboxFileButton = createClearFileButton(ynSkyboxLayout);
        connect(clearYnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearYnSkyboxFilename()));

        auto *ypSkyboxLayout = createFilenameInputText(skyboxLayout, 3, "Y+:", &ypSkyboxFilenameText);
        auto *selectYpSkyboxFileButton = createSelectFileButton(ypSkyboxLayout);
        connect(selectYpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showYpSkyboxFilenameDialog()));
        auto *clearYpSkyboxFileButton = createClearFileButton(ypSkyboxLayout);
        connect(clearYpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearYpSkyboxFilename()));

        auto *znSkyboxLayout = createFilenameInputText(skyboxLayout, 4, "Z-:", &znSkyboxFilenameText);
        auto *selectZnSkyboxFileButton = createSelectFileButton(znSkyboxLayout);
        connect(selectZnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showZnSkyboxFilenameDialog()));
        auto *clearZnSkyboxFileButton = createClearFileButton(znSkyboxLayout);
        connect(clearZnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearZnSkyboxFilename()));

        auto *zpSkyboxLayout = createFilenameInputText(skyboxLayout, 5, "Z+:", &zpSkyboxFilenameText);
        auto *selectZpSkyboxFileButton = createSelectFileButton(zpSkyboxLayout);
        connect(selectZpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showZpSkyboxFilenameDialog()));
        auto *clearZpSkyboxFileButton = createClearFileButton(zpSkyboxLayout);
        connect(clearZpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearZpSkyboxFilename()));

        auto *offsetYLabel = new QLabel("Offset Y:");
        skyboxLayout->addWidget(offsetYLabel, 6, 0);

        offsetY = new QDoubleSpinBox();
        skyboxLayout->addWidget(offsetY, 6, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(offsetY);
        connect(offsetY, SIGNAL(valueChanged(double)), this, SLOT(skyChanged()));
    }

    QHBoxLayout *SkyControllerWidget::createFilenameInputText(QGridLayout *skyboxLayout, int row, const QString &text, QLineEdit **skyboxFilenameText)
    {
        auto *skyboxFilenameLabel= new QLabel(text);
        skyboxLayout->addWidget(skyboxFilenameLabel, row, 0);

        auto *skyboxFilenameLayout = new QHBoxLayout();
        skyboxLayout->addLayout(skyboxFilenameLayout, row, 1, Qt::AlignmentFlag::AlignLeft);
        (*skyboxFilenameText) = new QLineEdit();
        skyboxFilenameLayout->addWidget(*skyboxFilenameText);
        (*skyboxFilenameText)->setReadOnly(true);

        return skyboxFilenameLayout;
    }

    QPushButton *SkyControllerWidget::createSelectFileButton(QHBoxLayout *skyboxFilenameLayout)
    {
        auto *selectSkyboxFileButton = new QPushButton("...");
        skyboxFilenameLayout->addWidget(selectSkyboxFileButton);
        ButtonStyleHelper::applyNormalStyle(selectSkyboxFileButton);
        selectSkyboxFileButton->setFixedWidth(22);

        return selectSkyboxFileButton;
    }

    QPushButton *SkyControllerWidget::createClearFileButton(QHBoxLayout *skyboxFilenameLayout)
    {
        auto *clearSkyboxFileButton = new QPushButton("Clr");
        skyboxFilenameLayout->addWidget(clearSkyboxFileButton);
        ButtonStyleHelper::applyNormalStyle(clearSkyboxFileButton);
        clearSkyboxFileButton->setFixedWidth(22);

        return clearSkyboxFileButton;
    }

    void SkyControllerWidget::showXnSkyboxFilenameDialog()
    {
        showSkyboxFilenameDialog(xnSkyboxFilenameText);
    }

    void SkyControllerWidget::showXpSkyboxFilenameDialog()
    {
        showSkyboxFilenameDialog(xpSkyboxFilenameText);
    }

    void SkyControllerWidget::showYnSkyboxFilenameDialog()
    {
        showSkyboxFilenameDialog(ynSkyboxFilenameText);
    }

    void SkyControllerWidget::showYpSkyboxFilenameDialog()
    {
        showSkyboxFilenameDialog(ypSkyboxFilenameText);
    }

    void SkyControllerWidget::showZnSkyboxFilenameDialog()
    {
        showSkyboxFilenameDialog(znSkyboxFilenameText);
    }

    void SkyControllerWidget::showZpSkyboxFilenameDialog()
    {
        showSkyboxFilenameDialog(zpSkyboxFilenameText);
    }

    void SkyControllerWidget::showSkyboxFilenameDialog(QLineEdit *skyboxFilenameText)
    {
        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredSkyboxPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredSkyboxPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png)", nullptr, QFileDialog::DontUseNativeDialog);
        if(!filename.isNull())
        {
            std::string imageFilenamePath = filename.toUtf8().constData();
            std::string relativeTgaFilenamePath = FileHandler::getRelativePath(resourcesDirectory, imageFilenamePath);
            skyboxFilenameText->setText(QString::fromStdString(relativeTgaFilenamePath));

            std::string preferredPathString = FileHandler::getDirectoryFrom(imageFilenamePath);
            preferredSkyboxPath = QString::fromStdString(preferredPathString);

            try
            {
                skyChanged();
            }catch(std::exception &e)
            {
                QMessageBox::critical(this, "Error", e.what());
                clearSkyboxFilename(skyboxFilenameText);
            }
        }
    }

    void SkyControllerWidget::clearXnSkyboxFilename()
    {
        clearSkyboxFilename(xnSkyboxFilenameText);
    }

    void SkyControllerWidget::clearXpSkyboxFilename()
    {
        clearSkyboxFilename(xpSkyboxFilenameText);
    }

    void SkyControllerWidget::clearYnSkyboxFilename()
    {
        clearSkyboxFilename(ynSkyboxFilenameText);
    }

    void SkyControllerWidget::clearYpSkyboxFilename()
    {
        clearSkyboxFilename(ypSkyboxFilenameText);
    }

    void SkyControllerWidget::clearZnSkyboxFilename()
    {
        clearSkyboxFilename(znSkyboxFilenameText);
    }

    void SkyControllerWidget::clearZpSkyboxFilename()
    {
        clearSkyboxFilename(zpSkyboxFilenameText);
    }

    void SkyControllerWidget::clearSkyboxFilename(QLineEdit *skyboxFilenameText)
    {
        skyboxFilenameText->setText("");

        skyChanged();
    }

    void SkyControllerWidget::setupSkyDataFrom(const std::unique_ptr<Skybox> &skybox)
    {
        disableSkyEvent = true;
        xnSkyboxFilenameText->setText(skybox != nullptr ? QString::fromStdString(skybox->getFilenames()[0]) : "");
        xpSkyboxFilenameText->setText(skybox != nullptr ? QString::fromStdString(skybox->getFilenames()[1]) : "");
        ynSkyboxFilenameText->setText(skybox != nullptr ? QString::fromStdString(skybox->getFilenames()[2]) : "");
        ypSkyboxFilenameText->setText(skybox != nullptr ? QString::fromStdString(skybox->getFilenames()[3]) : "");
        znSkyboxFilenameText->setText(skybox != nullptr ? QString::fromStdString(skybox->getFilenames()[4]) : "");
        zpSkyboxFilenameText->setText(skybox != nullptr ? QString::fromStdString(skybox->getFilenames()[5]) : "");
        offsetY->setValue(skybox != nullptr ? skybox->getOffsetY() : 0.0f);
        disableSkyEvent = false;
    }

    void SkyControllerWidget::skyChanged()
    {
        if(!disableSkyEvent)
        {
            std::vector<std::string> skyboxFilenames;
            skyboxFilenames.emplace_back(xnSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(xpSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(ynSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(ypSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(znSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(zpSkyboxFilenameText->text().toStdString());

            skyController->updateSceneSky(skyboxFilenames, static_cast<float>(offsetY->value()));
        }
    }

}
