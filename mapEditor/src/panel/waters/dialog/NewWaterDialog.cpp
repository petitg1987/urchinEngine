#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include <panel/waters/dialog/NewWaterDialog.h>
#include <widget/style/LabelStyleHelper.h>

namespace urchin {
    NewWaterDialog::NewWaterDialog(QWidget* parent, const WaterController* waterController) :
            QDialog(parent),
            waterController(waterController),
            waterNameLabel(nullptr),
            waterNameText(nullptr),
            sceneWater(nullptr) {
        this->setWindowTitle("New Water");
        this->resize(530, 130);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void NewWaterDialog::setupNameFields(QGridLayout* mainLayout) {
        waterNameLabel = new QLabel("Water Name:");
        mainLayout->addWidget(waterNameLabel, 0, 0);

        waterNameText = new QLineEdit();
        mainLayout->addWidget(waterNameText, 0, 1);
        waterNameText->setFixedWidth(360);
    }

    void NewWaterDialog::updateWaterName() {
        QString waterName = waterNameText->text();
        if (!waterName.isEmpty()) {
            this->waterName = waterName.toUtf8().constData();
        }
    }

    int NewWaterDialog::buildSceneWater(int result) {
        try {
            sceneWater = std::make_unique<SceneWater>();
            sceneWater->setName(waterName);
            
            auto water = std::make_shared<Water>();

            sceneWater->setWater(water);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            return QDialog::Rejected;
        }

        return result;
    }


    std::unique_ptr<SceneWater> NewWaterDialog::moveSceneWater() {
        return std::move(sceneWater);
    }

    void NewWaterDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateWaterName();
            LabelStyleHelper::applyNormalStyle(waterNameLabel);

            if (waterName.empty()) {
                LabelStyleHelper::applyErrorStyle(waterNameLabel, "Water name is mandatory");
                hasError = true;
            } else if (isSceneWaterExist(waterName)) {
                LabelStyleHelper::applyErrorStyle(waterNameLabel, "Water name is already used");
                hasError = true;
            }

            if (!hasError) {
                r = buildSceneWater(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewWaterDialog::isSceneWaterExist(const std::string& name) {
        std::list<const SceneWater*> sceneWaters = waterController->getSceneWaters();
        return std::ranges::any_of(sceneWaters, [&name](const auto& sw){return sw->getName() == name;});
    }
}
