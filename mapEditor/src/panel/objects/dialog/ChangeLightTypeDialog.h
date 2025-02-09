#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <Urchin3dEngine.h>

namespace urchin {

    class ChangeLightTypeDialog final : public QDialog {
        Q_OBJECT

        public:
            static constexpr char NONE_LABEL[] = "None";
            static constexpr char SUN_LIGHT_LABEL[] = "Sun";
            static constexpr char OMNIDIRECTIONAL_LIGHT_LABEL[] = "Omnidirectional";
            static constexpr char SPOT_LIGHT_LABEL[] = "Spot";

            explicit ChangeLightTypeDialog(QWidget*);

            std::optional<Light::LightType> getLightType() const;

        private:
            void setupLightTypeFields(QGridLayout*);

            void done(int) override;

            QComboBox* lightTypeComboBox;

            std::optional<Light::LightType> lightType;
    };

}
