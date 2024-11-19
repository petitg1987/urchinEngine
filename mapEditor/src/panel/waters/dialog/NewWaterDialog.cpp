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
            waterEntity(nullptr) {
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

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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

    int NewWaterDialog::buildWaterEntity(int result) {
        try {
            waterEntity = std::make_unique<WaterEntity>();
            waterEntity->setName(waterName);
            
            auto water = std::make_shared<Water>();

            waterEntity->setWater(water);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            return Rejected;
        }

        return result;
    }

    std::unique_ptr<WaterEntity> NewWaterDialog::moveWaterEntity() {
        return std::move(waterEntity);
    }

    void NewWaterDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            updateWaterName();
            LabelStyleHelper::applyNormalStyle(waterNameLabel);

            if (waterName.empty()) {
                LabelStyleHelper::applyErrorStyle(waterNameLabel, "Water name is mandatory");
                hasError = true;
            } else if (isWaterEntityExist(waterName)) {
                LabelStyleHelper::applyErrorStyle(waterNameLabel, "Water name is already used");
                hasError = true;
            }

            if (!hasError) {
                r = buildWaterEntity(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewWaterDialog::isWaterEntityExist(std::string_view name) const {
        std::list<const WaterEntity*> waterEntities = waterController->getWaterEntities();
        return std::ranges::any_of(waterEntities, [name](const auto& we){ return we->getName() == name; });
    }
}
