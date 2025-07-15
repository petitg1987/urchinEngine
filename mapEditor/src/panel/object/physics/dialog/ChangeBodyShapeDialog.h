#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>

#include <UrchinPhysicsEngine.h>

namespace urchin {

    class ChangeBodyShapeDialog final : public QDialog {
        Q_OBJECT

        public:
            ChangeBodyShapeDialog(QWidget*, bool, CollisionShape3D::ShapeType);

            CollisionShape3D::ShapeType getShapeType() const;
            const DefaultShapeQuality& getDefaultShapeQuality() const;

        private:
            void setupBodyShapeTypeFields(QGridLayout*, CollisionShape3D::ShapeType);

            void done(int) override;

            bool excludeCompoundShape;

            QComboBox* bodyShapeTypeComboBox;
            QComboBox* defaultShapeQualityComboBox;

            CollisionShape3D::ShapeType shapeType;
            DefaultShapeQuality defaultShapeQuality;
    };

}
