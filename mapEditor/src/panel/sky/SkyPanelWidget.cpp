#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QMessageBox>
#include <UrchinAggregation.h>

#include <panel/sky/SkyPanelWidget.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>

namespace urchin {

    QString SkyPanelWidget::preferredSkyboxPath = QString();

    SkyPanelWidget::SkyPanelWidget() :
            skyController(nullptr),
            xnSkyboxFilenameText(nullptr),
            xpSkyboxFilenameText(nullptr),
            ynSkyboxFilenameText(nullptr),
            ypSkyboxFilenameText(nullptr),
            znSkyboxFilenameText(nullptr),
            zpSkyboxFilenameText(nullptr),
            offsetY(nullptr),
            disableSkyEvent(false) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        setupSkyboxBox(mainLayout);
    }

    void SkyPanelWidget::load(SkyController& skyController) {
        this->skyController = &skyController;

        auto* skybox = this->skyController->getSkyEntity().getSkybox();
        setupSkyDataFrom(skybox);
    }

    void SkyPanelWidget::unload() {
        skyController = nullptr;
    }

    void SkyPanelWidget::setupSkyboxBox(QVBoxLayout* mainLayout) {
        auto* skyboxGroupBox = new QGroupBox("Skybox");
        mainLayout->addWidget(skyboxGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(skyboxGroupBox);
        skyboxGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* skyboxLayout = new QGridLayout(skyboxGroupBox);

        auto* xnSkyboxLayout = createFilenameInputText(skyboxLayout, 0, "X-:", &xnSkyboxFilenameText);
        const auto* selectXnSkyboxFileButton = createSelectFileButton(xnSkyboxLayout);
        connect(selectXnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showXnSkyboxFilenameDialog()));
        const auto* clearXnSkyboxFileButton = createClearFileButton(xnSkyboxLayout);
        connect(clearXnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearXnSkyboxFilename()));

        auto* xpSkyboxLayout = createFilenameInputText(skyboxLayout, 1, "X+:", &xpSkyboxFilenameText);
        const auto* selectXpSkyboxFileButton = createSelectFileButton(xpSkyboxLayout);
        connect(selectXpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showXpSkyboxFilenameDialog()));
        const auto* clearXpSkyboxFileButton = createClearFileButton(xpSkyboxLayout);
        connect(clearXpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearXpSkyboxFilename()));

        auto* ynSkyboxLayout = createFilenameInputText(skyboxLayout, 2, "Y-:", &ynSkyboxFilenameText);
        const auto* selectYnSkyboxFileButton = createSelectFileButton(ynSkyboxLayout);
        connect(selectYnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showYnSkyboxFilenameDialog()));
        const auto* clearYnSkyboxFileButton = createClearFileButton(ynSkyboxLayout);
        connect(clearYnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearYnSkyboxFilename()));

        auto* ypSkyboxLayout = createFilenameInputText(skyboxLayout, 3, "Y+:", &ypSkyboxFilenameText);
        const auto* selectYpSkyboxFileButton = createSelectFileButton(ypSkyboxLayout);
        connect(selectYpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showYpSkyboxFilenameDialog()));
        const auto* clearYpSkyboxFileButton = createClearFileButton(ypSkyboxLayout);
        connect(clearYpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearYpSkyboxFilename()));

        auto* znSkyboxLayout = createFilenameInputText(skyboxLayout, 4, "Z-:", &znSkyboxFilenameText);
        const auto* selectZnSkyboxFileButton = createSelectFileButton(znSkyboxLayout);
        connect(selectZnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showZnSkyboxFilenameDialog()));
        const auto* clearZnSkyboxFileButton = createClearFileButton(znSkyboxLayout);
        connect(clearZnSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearZnSkyboxFilename()));

        auto* zpSkyboxLayout = createFilenameInputText(skyboxLayout, 5, "Z+:", &zpSkyboxFilenameText);
        const auto* selectZpSkyboxFileButton = createSelectFileButton(zpSkyboxLayout);
        connect(selectZpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(showZpSkyboxFilenameDialog()));
        const auto* clearZpSkyboxFileButton = createClearFileButton(zpSkyboxLayout);
        connect(clearZpSkyboxFileButton, SIGNAL(clicked()), this, SLOT(clearZpSkyboxFilename()));

        auto* offsetYLabel = new QLabel("Offset Y:");
        skyboxLayout->addWidget(offsetYLabel, 6, 0);

        offsetY = new QDoubleSpinBox();
        skyboxLayout->addWidget(offsetY, 6, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(offsetY);
        connect(offsetY, SIGNAL(valueChanged(double)), this, SLOT(skyChanged()));
    }

    QHBoxLayout* SkyPanelWidget::createFilenameInputText(QGridLayout* skyboxLayout, int row, const QString& text, QLineEdit **skyboxFilenameText) const {
        auto* skyboxFilenameLabel= new QLabel(text);
        skyboxLayout->addWidget(skyboxFilenameLabel, row, 0);

        auto* skyboxFilenameLayout = new QHBoxLayout();
        skyboxLayout->addLayout(skyboxFilenameLayout, row, 1, Qt::AlignmentFlag::AlignLeft);
        (*skyboxFilenameText) = new QLineEdit();
        skyboxFilenameLayout->addWidget(*skyboxFilenameText);
        (*skyboxFilenameText)->setReadOnly(true);

        return skyboxFilenameLayout;
    }

    QPushButton* SkyPanelWidget::createSelectFileButton(QHBoxLayout* skyboxFilenameLayout) const {
        auto* selectSkyboxFileButton = new QPushButton("...");
        skyboxFilenameLayout->addWidget(selectSkyboxFileButton);
        ButtonStyleHelper::applyNormalStyle(selectSkyboxFileButton);
        selectSkyboxFileButton->setFixedWidth(22);

        return selectSkyboxFileButton;
    }

    QPushButton* SkyPanelWidget::createClearFileButton(QHBoxLayout* skyboxFilenameLayout) const {
        auto* clearSkyboxFileButton = new QPushButton("Clr");
        skyboxFilenameLayout->addWidget(clearSkyboxFileButton);
        ButtonStyleHelper::applyNormalStyle(clearSkyboxFileButton);
        clearSkyboxFileButton->setFixedWidth(22);

        return clearSkyboxFileButton;
    }

    void SkyPanelWidget::showXnSkyboxFilenameDialog() {
        showSkyboxFilenameDialog(xnSkyboxFilenameText);
    }

    void SkyPanelWidget::showXpSkyboxFilenameDialog() {
        showSkyboxFilenameDialog(xpSkyboxFilenameText);
    }

    void SkyPanelWidget::showYnSkyboxFilenameDialog() {
        showSkyboxFilenameDialog(ynSkyboxFilenameText);
    }

    void SkyPanelWidget::showYpSkyboxFilenameDialog() {
        showSkyboxFilenameDialog(ypSkyboxFilenameText);
    }

    void SkyPanelWidget::showZnSkyboxFilenameDialog() {
        showSkyboxFilenameDialog(znSkyboxFilenameText);
    }

    void SkyPanelWidget::showZpSkyboxFilenameDialog() {
        showSkyboxFilenameDialog(zpSkyboxFilenameText);
    }

    void SkyPanelWidget::showSkyboxFilenameDialog(QLineEdit* skyboxFilenameText) {
        std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
        QString directory = preferredSkyboxPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredSkyboxPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png *.qoi)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string imageFilename = filename.toUtf8().constData();
            std::string relativeFilename = PathUtil::computeRelativePath(resourcesDirectory, imageFilename);
            skyboxFilenameText->setText(QString::fromStdString(relativeFilename));

            std::string preferredPathString = FileUtil::getDirectory(imageFilename);
            preferredSkyboxPath = QString::fromStdString(preferredPathString);

            try {
                skyChanged();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", e.what());
                clearSkyboxFilename(skyboxFilenameText);
            }
        }
    }

    void SkyPanelWidget::clearXnSkyboxFilename() {
        clearSkyboxFilename(xnSkyboxFilenameText);
    }

    void SkyPanelWidget::clearXpSkyboxFilename() {
        clearSkyboxFilename(xpSkyboxFilenameText);
    }

    void SkyPanelWidget::clearYnSkyboxFilename() {
        clearSkyboxFilename(ynSkyboxFilenameText);
    }

    void SkyPanelWidget::clearYpSkyboxFilename() {
        clearSkyboxFilename(ypSkyboxFilenameText);
    }

    void SkyPanelWidget::clearZnSkyboxFilename() {
        clearSkyboxFilename(znSkyboxFilenameText);
    }

    void SkyPanelWidget::clearZpSkyboxFilename() {
        clearSkyboxFilename(zpSkyboxFilenameText);
    }

    void SkyPanelWidget::clearSkyboxFilename(QLineEdit* skyboxFilenameText) {
        skyboxFilenameText->setText("");

        skyChanged();
    }

    void SkyPanelWidget::setupSkyDataFrom(const Skybox* skybox) {
        disableSkyEvent = true;
        std::size_t fileIndex = 0;
        for (QLineEdit* skyboxFilenameText : {xnSkyboxFilenameText, xpSkyboxFilenameText, ynSkyboxFilenameText, ypSkyboxFilenameText, znSkyboxFilenameText, zpSkyboxFilenameText}) {
            if (skybox != nullptr) {
                std::string relativeFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), skybox->getFilenames()[fileIndex]);
                skyboxFilenameText->setText(QString::fromStdString(relativeFilename));
            } else {
                skyboxFilenameText->setText("");
            }
            fileIndex++;
        }
        offsetY->setValue(skybox != nullptr ? skybox->getOffsetY() : 0.0f);
        disableSkyEvent = false;
    }

    void SkyPanelWidget::skyChanged() {
        if (!disableSkyEvent) {
            std::vector<std::string> skyboxFilenames;
            skyboxFilenames.emplace_back(xnSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(xpSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(ynSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(ypSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(znSkyboxFilenameText->text().toStdString());
            skyboxFilenames.emplace_back(zpSkyboxFilenameText->text().toStdString());

            skyController->updateSkyEntity(skyboxFilenames, (float)offsetY->value());
        }
    }

}
