#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include <UrchinCommon.h>
#include <widget/dialog/NewDialog.h>
#include <widget/style/LabelStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>

namespace urchin {

    NewDialog::NewDialog(QWidget* parent) :
        QDialog(parent),
        mapNameLabel(nullptr),
        mapNameText(nullptr),
        mapDirectoryLabel(nullptr),
        mapDirectoryText(nullptr),
        mapWorkingDirectoryLabel(nullptr),
        mapWorkingDirectoryText(nullptr),
        mapRelWorkingDirectoryText(nullptr) {
        this->setWindowTitle("New Map File");
        this->resize(530, 195);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupDirectoryFields(mainLayout);
        setupWorkingDirectoryFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 5, 0, 1, 3, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void NewDialog::setupNameFields(QGridLayout* mainLayout) {
        mapNameLabel = new QLabel("Name (*.uda):");
        mainLayout->addWidget(mapNameLabel, 0, 0);

        mapNameText = new QLineEdit();
        mainLayout->addWidget(mapNameText, 0, 1);
        mapNameText->setFixedWidth(360);
    }

    void NewDialog::setupDirectoryFields(QGridLayout* mainLayout) {
        mapDirectoryLabel = new QLabel("Directory:");
        mainLayout->addWidget(mapDirectoryLabel, 1, 0);

        mapDirectoryText = new QLineEdit();
        mainLayout->addWidget(mapDirectoryText, 1, 1);
        mapDirectoryText->setReadOnly(true);
        mapDirectoryText->setFixedWidth(360);

        auto* selectMapDirButton = new QPushButton("...");
        mainLayout->addWidget(selectMapDirButton, 1, 2);
        ButtonStyleHelper::applyNormalStyle(selectMapDirButton);
        selectMapDirButton->setFixedWidth(22);
        connect(selectMapDirButton, SIGNAL(clicked()), this, SLOT(showMapDirectoryDialog()));
    }

    void NewDialog::setupWorkingDirectoryFields(QGridLayout* mainLayout) {
        mapWorkingDirectoryLabel = new QLabel("Working Directory:");
        mainLayout->addWidget(mapWorkingDirectoryLabel, 2, 0);

        mapWorkingDirectoryText = new QLineEdit();
        mainLayout->addWidget(mapWorkingDirectoryText, 2, 1);
        mapWorkingDirectoryText->setReadOnly(true);
        mapWorkingDirectoryText->setFixedWidth(360);

        auto* selectMapWorkingDirButton = new QPushButton("...");
        mainLayout->addWidget(selectMapWorkingDirButton, 2, 2);
        ButtonStyleHelper::applyNormalStyle(selectMapWorkingDirButton);
        selectMapWorkingDirButton->setFixedWidth(22);
        connect(selectMapWorkingDirButton, SIGNAL(clicked()), this, SLOT(showMapWorkingDirectoryDialog()));

        //relative working directory
        auto* mapRelWorkingDirectoryLabel = new QLabel("Relative Working Dir.:");
        mainLayout->addWidget(mapRelWorkingDirectoryLabel, 3, 0);

        mapRelWorkingDirectoryText = new QLabel();
        mainLayout->addWidget(mapRelWorkingDirectoryText, 3, 1);
        mapRelWorkingDirectoryText->setFixedWidth(360);
    }

    std::string NewDialog::getFilename() const {
        return mapDirectory + mapFilename;
    }

    std::string NewDialog::getRelativeWorkingDirectory() const {
        return mapRelWorkingDirectory;
    }

    void NewDialog::updateMapFilename() {
        QString mapFilename = mapNameText->text();
        if (!mapFilename.isEmpty()) {
            this->mapFilename = mapFilename.toUtf8().constData();
            std::string fileExtension = FileUtil::getFileExtension(this->mapFilename);
            if (!StringUtil::insensitiveEquals(fileExtension, ".uda")) {
                this->mapFilename += ".uda";
            }
        }
    }

    void NewDialog::showMapDirectoryDialog() {
        QString mapDirectory = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "./", QFileDialog::DontUseNativeDialog|QFileDialog::ShowDirsOnly);
        if (!mapDirectory.isEmpty()) {
            this->mapDirectory = std::string(mapDirectory.toUtf8().constData()) + "/";

            mapDirectoryText->setText(mapDirectory);
            updateRelativeWorkingDirectory();
        }
    }

    void NewDialog::showMapWorkingDirectoryDialog() {
        QString mapWorkingDirectory = QFileDialog::getExistingDirectory(this, tr("Select working Directory"), "./", QFileDialog::DontUseNativeDialog|QFileDialog::ShowDirsOnly);
        if (!mapWorkingDirectory.isEmpty()) {
            this->mapWorkingDirectory = std::string(mapWorkingDirectory.toUtf8().constData()) + "/";

            mapWorkingDirectoryText->setText(mapWorkingDirectory);
            updateRelativeWorkingDirectory();
        }
    }

    void NewDialog::updateRelativeWorkingDirectory() {
        if (!mapDirectory.empty() && !mapWorkingDirectory.empty()) {
            mapRelWorkingDirectory = FileUtil::getRelativePath(mapDirectory, mapWorkingDirectory);
            mapRelWorkingDirectoryText->setText(QString::fromStdString(mapRelWorkingDirectory));
        }
    }

    void NewDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateMapFilename();
            LabelStyleHelper::applyNormalStyle(mapNameLabel);
            LabelStyleHelper::applyNormalStyle(mapDirectoryLabel);
            LabelStyleHelper::applyNormalStyle(mapWorkingDirectoryLabel);

            if (mapFilename.empty()) {
                LabelStyleHelper::applyErrorStyle(mapNameLabel, "Map name is mandatory");
                hasError = true;
            } else if (mapFilename.find_last_of("/\\") != std::string::npos) {
                LabelStyleHelper::applyErrorStyle(mapNameLabel, "Map name cannot contains slash character");
                hasError = true;
            }
            if (mapDirectory.empty()) {
                LabelStyleHelper::applyErrorStyle(mapDirectoryLabel, "Map directory is mandatory");
                hasError = true;
            }
            if (mapWorkingDirectory.empty()) {
                LabelStyleHelper::applyErrorStyle(mapWorkingDirectoryLabel, "Map working directory is mandatory");
                hasError = true;
            }

            if (!hasError) {
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

}
