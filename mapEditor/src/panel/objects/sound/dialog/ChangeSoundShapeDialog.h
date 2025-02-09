#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>

#include <UrchinSoundEngine.h>

namespace urchin {

    class ChangeSoundShapeDialog final : public QDialog {
        Q_OBJECT

        public:
            explicit ChangeSoundShapeDialog(QWidget*);

            SoundShape::ShapeType getShapeType() const;

        private:
            void setupSoundShapeTypeFields(QGridLayout*);

            void done(int) override;

            QComboBox* soundShapeTypeComboBox;

            SoundShape::ShapeType shapeType;
    };

}
