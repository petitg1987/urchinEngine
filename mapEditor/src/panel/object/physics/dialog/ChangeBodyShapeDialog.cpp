#include <QtWidgets/QDialogButtonBox>

#include <panel/object/physics/dialog/ChangeBodyShapeDialog.h>
#include <panel/object/physics/bodyshape/BodyShapeWidget.h>

namespace urchin {

    ChangeBodyShapeDialog::ChangeBodyShapeDialog(QWidget* parent, bool excludeCompoundShape, CollisionShape3D::ShapeType defaultSelectedShapeType) :
            QDialog(parent),
            excludeCompoundShape(excludeCompoundShape),
            bodyShapeTypeComboBox(nullptr),
            shapeType(CollisionShape3D::ShapeType::SHAPE_MAX),
            defaultShapeQuality(DefaultShapeQuality::LOW) {
        this->setWindowTitle("Select Body Shape");
        this->resize(330, 120);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupBodyShapeTypeFields(mainLayout, defaultSelectedShapeType);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 2, Qt::AlignmentFlag::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeBodyShapeDialog::setupBodyShapeTypeFields(QGridLayout* mainLayout, CollisionShape3D::ShapeType defaultSelectedShapeType) {
        auto* bodyShapeTypeLabel = new QLabel("Shape Type:");
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
        for (int i = 0; i < bodyShapeTypeComboBox->count(); ++i) {
            if (bodyShapeTypeComboBox->itemData(i).toInt() == defaultSelectedShapeType) {
                bodyShapeTypeComboBox->setCurrentIndex(i);
                break;
            }
        }

        auto* defaultShapeQualityLabel = new QLabel("Default Shape Quality:");
        mainLayout->addWidget(defaultShapeQualityLabel, 1, 0);

        defaultShapeQualityComboBox = new QComboBox();
        mainLayout->addWidget(defaultShapeQualityComboBox, 1, 1);
        defaultShapeQualityComboBox->setFixedWidth(150);
        int itemIndex = 0;
        for (const DefaultShapeQuality& quality : DefaultShapeQuality::getAllQualities()) {
            defaultShapeQualityComboBox->addItem(QString::fromStdString(quality.getLabel()), QVariant(quality.getId()));

            std::string tooltip = std::format("Quality:<br> - Voxelization size: {}<br> - Convex hull max points: {}",
                quality.getVoxelizationSize(), quality.getConvexHullMaxPoints());
            defaultShapeQualityComboBox->setItemData(itemIndex++, QString::fromStdString(tooltip), Qt::ToolTipRole);
        }
        defaultShapeQualityComboBox->setCurrentIndex(0);
    }

    void ChangeBodyShapeDialog::done(int r) {
        if (Accepted == r) {
            QVariant variantShapeType = bodyShapeTypeComboBox->currentData();
            shapeType = static_cast<CollisionShape3D::ShapeType>(variantShapeType.toInt());

            QVariant variantDefaultShapeQuality = defaultShapeQualityComboBox->currentData();
            defaultShapeQuality = DefaultShapeQuality::getQualityById(variantDefaultShapeQuality.toInt());

            QDialog::done(r);
        } else {
            QDialog::done(r);
        }
    }

    CollisionShape3D::ShapeType ChangeBodyShapeDialog::getShapeType() const {
        return shapeType;
    }

    const DefaultShapeQuality& ChangeBodyShapeDialog::getDefaultShapeQuality() const {
        return defaultShapeQuality;
    }

}
