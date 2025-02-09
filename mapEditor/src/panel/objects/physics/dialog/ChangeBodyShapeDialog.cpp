#include <QtWidgets/QDialogButtonBox>

#include <panel/objects/physics/dialog/ChangeBodyShapeDialog.h>
#include <panel/objects/physics/bodyshape/BodyShapeWidget.h>

namespace urchin {

    ChangeBodyShapeDialog::ChangeBodyShapeDialog(QWidget* parent, bool excludeCompoundShape) :
            QDialog(parent),
            excludeCompoundShape(excludeCompoundShape),
            bodyShapeTypeLabel(nullptr),
            bodyShapeTypeComboBox(nullptr),
            shapeType(CollisionShape3D::ShapeType::SHAPE_MAX) {
        this->setWindowTitle("Select Body Shape");
        this->resize(260, 90);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupBodyShapeTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, 1, 2, Qt::AlignmentFlag::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeBodyShapeDialog::setupBodyShapeTypeFields(QGridLayout* mainLayout) {
        bodyShapeTypeLabel = new QLabel("Shape Type:");
        mainLayout->addWidget(bodyShapeTypeLabel, 0, 0);

        bodyShapeTypeComboBox = new QComboBox();
        mainLayout->addWidget(bodyShapeTypeComboBox, 0, 1);
        bodyShapeTypeComboBox->setFixedWidth(150);
        bodyShapeTypeComboBox->addItem(BodyShapeWidget::SPHERE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::SPHERE_SHAPE));
        bodyShapeTypeComboBox->addItem(BodyShapeWidget::BOX_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::BOX_SHAPE));
        bodyShapeTypeComboBox->addItem(BodyShapeWidget::CAPSULE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CAPSULE_SHAPE));
        bodyShapeTypeComboBox->addItem(BodyShapeWidget::CYLINDER_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CYLINDER_SHAPE));
        bodyShapeTypeComboBox->addItem(BodyShapeWidget::CONE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CONE_SHAPE));
        bodyShapeTypeComboBox->addItem(BodyShapeWidget::CONVEX_HULL_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE));
        if (!excludeCompoundShape) {
            bodyShapeTypeComboBox->addItem(BodyShapeWidget::COMPOUND_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::COMPOUND_SHAPE));
        }
    }

    void ChangeBodyShapeDialog::done(int r) {
        if (Accepted == r) {
            QVariant variant = bodyShapeTypeComboBox->currentData();
            shapeType = static_cast<CollisionShape3D::ShapeType>(variant.toInt());
            QDialog::done(r);
        } else {
            QDialog::done(r);
        }
    }

    CollisionShape3D::ShapeType ChangeBodyShapeDialog::getShapeType() const {
        return shapeType;
    }

}
