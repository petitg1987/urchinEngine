#include <stdexcept>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "UrchinCommon.h"
#include "ChangeBodyShapeDialog.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	ChangeBodyShapeDialog::ChangeBodyShapeDialog(QWidget *parent, bool excludeCompoundShape) :
		QDialog(parent),
		excludeCompoundShape(excludeCompoundShape)
	{
		this->setWindowTitle("Select Body Shape");
		this->resize(290, 100);

		setupBodyShapeTypeFields();

		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 60, 245, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void ChangeBodyShapeDialog::setupBodyShapeTypeFields()
	{
		bodyShapeTypeLabel = new QLabel("Shape Type:", this);
		bodyShapeTypeLabel->setGeometry(QRect(10, 20, 90, 22));

		bodyShapeTypeComboBox = new QComboBox(this);
		bodyShapeTypeComboBox->setGeometry(QRect(100, 20, 175, 22));
		bodyShapeTypeComboBox->addItem(SPHERE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::SPHERE_SHAPE));
		bodyShapeTypeComboBox->addItem(BOX_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::BOX_SHAPE));
		bodyShapeTypeComboBox->addItem(CAPSULE_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CAPSULE_SHAPE));
		bodyShapeTypeComboBox->addItem(CYLINDER_SHAPE_LABEL, QVariant(CollisionShape3D::ShapeType::CYLINDER_SHAPE));
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
