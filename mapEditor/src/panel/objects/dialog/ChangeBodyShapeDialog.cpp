#include <stdexcept>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "ChangeBodyShapeDialog.h"
#include "panel/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

    ChangeBodyShapeDialog::ChangeBodyShapeDialog(QWidget *parent, bool excludeCompoundShape) :
        QDialog(parent),
        excludeCompoundShape(excludeCompoundShape),
        bodyShapeTypeLabel(nullptr),
        bodyShapeTypeComboBox(nullptr),
        shapeType(CollisionShape3D::ShapeType::SHAPE_MAX)
    {
        this->setWindowTitle("Select Body Shape");
        this->resize(245, 80);
        this->setFixedSize(this->width(),this->height());

        auto *mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupBodyShapeTypeFields(mainLayout);

        auto *buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, 1, 2, Qt::AlignmentFlag::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeBodyShapeDialog::setupBodyShapeTypeFields(QGridLayout *mainLayout)
    {
        bodyShapeTypeLabel = new QLabel("Shape Type:");
        mainLayout->addWidget(bodyShapeTypeLabel, 0, 0);

        bodyShapeTypeComboBox = new QComboBox();
        mainLayout->addWidget(bodyShapeTypeComboBox, 0, 1);
        bodyShapeTypeComboBox->setFixedWidth(150);
        bodyShapeTypeComboBox->addItem(SPHERE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::SPHERE_SHAPE));
        bodyShapeTypeComboBox->addItem(BOX_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::BOX_SHAPE));
        bodyShapeTypeComboBox->addItem(CAPSULE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CAPSULE_SHAPE));
        bodyShapeTypeComboBox->addItem(CYLINDER_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CYLINDER_SHAPE));
        bodyShapeTypeComboBox->addItem(CONE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CONE_SHAPE));
        bodyShapeTypeComboBox->addItem(CONVEX_HULL_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE));
        if(!excludeCompoundShape)
        {
            bodyShapeTypeComboBox->addItem(COMPOUND_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::COMPOUND_SHAPE));
        }
    }

    void ChangeBodyShapeDialog::done(int r)
    {
        if(QDialog::Accepted == r)
        {
            QVariant variant = bodyShapeTypeComboBox->currentData();
            shapeType = static_cast<CollisionShape3D::ShapeType>(variant.toInt());
            QDialog::done(r);
        }else
        {
            QDialog::done(r);
        }
    }

    CollisionShape3D::ShapeType ChangeBodyShapeDialog::getShapeType() const
    {
        return shapeType;
    }

}
