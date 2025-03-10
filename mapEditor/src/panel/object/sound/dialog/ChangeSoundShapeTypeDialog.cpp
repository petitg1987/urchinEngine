#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>

#include <panel/object/sound/dialog/ChangeSoundShapeTypeDialog.h>
#include <panel/object/sound/soundshape/SoundShapeWidget.h>

namespace urchin {

    ChangeSoundShapeTypeDialog::ChangeSoundShapeTypeDialog(QWidget* parent) :
            QDialog(parent),
            soundShapeTypeComboBox(nullptr),
            shapeType(SoundShape::ShapeType::SPHERE_SHAPE) {
        this->setWindowTitle("Select Trigger Shape");
        this->resize(280, 80);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupSoundShapeTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, 1, 2, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeSoundShapeTypeDialog::setupSoundShapeTypeFields(QGridLayout* mainLayout) {
        auto* soundShapeTypeLabel = new QLabel("Shape Type:");
        mainLayout->addWidget(soundShapeTypeLabel, 0, 0);

        soundShapeTypeComboBox = new QComboBox();
        mainLayout->addWidget(soundShapeTypeComboBox, 0, 1);
        soundShapeTypeComboBox->setFixedWidth(150);
        soundShapeTypeComboBox->addItem(SoundShapeWidget::SPHERE_SHAPE_LABEL, QVariant(SoundShape::ShapeType::SPHERE_SHAPE));
        soundShapeTypeComboBox->addItem(SoundShapeWidget::BOX_SHAPE_LABEL, QVariant(SoundShape::ShapeType::BOX_SHAPE));
    }

    void ChangeSoundShapeTypeDialog::done(int r) {
        if (Accepted == r) {
            QVariant variant = soundShapeTypeComboBox->currentData();
            shapeType = static_cast<SoundShape::ShapeType>(variant.toInt());
            QDialog::done(r);
        } else {
            QDialog::done(r);
        }
    }

    SoundShape::ShapeType ChangeSoundShapeTypeDialog::getShapeType() const {
        return shapeType;
    }

}
