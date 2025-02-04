#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>

namespace urchin {

    class ChangeLightDialog final : public QDialog {
        Q_OBJECT

        public:
            static constexpr char SUN_LIGHT_LABEL[] = "Sun";
            static constexpr char OMNIDIRECTIONAL_LIGHT_LABEL[] = "Omnidirectional";
            static constexpr char SPOT_LIGHT_LABEL[] = "Spot";

            explicit ChangeLightDialog(QWidget*);

            Light::LightType getLightType() const;

        private:
            void setupLightTypeFields(QGridLayout*);

            void done(int) override;

            QComboBox* lightTypeComboBox;

            Light::LightType lightType;
    };

}
