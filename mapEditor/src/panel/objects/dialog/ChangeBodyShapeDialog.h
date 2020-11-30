#ifndef URCHINENGINE_CHANGEBODYSHAPEDIALOG_H
#define URCHINENGINE_CHANGEBODYSHAPEDIALOG_H

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "UrchinMapHandler.h"
#include "UrchinPhysicsEngine.h"

namespace urchin {

    class ChangeBodyShapeDialog : public QDialog {
        Q_OBJECT

        public:
            ChangeBodyShapeDialog(QWidget*, bool);

            CollisionShape3D::ShapeType getShapeType() const;

        private:
            void setupBodyShapeTypeFields(QGridLayout*);

            void done(int) override;

            bool excludeCompoundShape;

            QLabel* bodyShapeTypeLabel;
            QComboBox* bodyShapeTypeComboBox;

            CollisionShape3D::ShapeType shapeType;
    };

}

#endif
