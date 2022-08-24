#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include <UrchinAggregation.h>
#include <controller/lights/LightController.h>

namespace urchin {

    class NewLightDialog final : public QDialog {
        Q_OBJECT

        public:
            static constexpr char SUN_LIGHT_LABEL[] = "Sun";
            static constexpr char OMNIDIRECTIONAL_LIGHT_LABEL[] = "Omnidirectional";

            NewLightDialog(QWidget*, const LightController*);

            std::unique_ptr<LightEntity> moveLightEntity();

        private:
            void setupNameFields(QGridLayout*);
            void setupLightTypeFields(QGridLayout*);

            void updateLightName();
            int buildLightEntity(int);

            void done(int) override;
            bool isLightEntityExist(std::string_view) const;

            const LightController* lightController;

            QLabel* lightNameLabel;
            QLineEdit* lightNameText;
            QLabel* lightTypeLabel;
            QComboBox* lightTypeComboBox;

            std::string lightName;
            std::unique_ptr<LightEntity> lightEntity;
    };

}
