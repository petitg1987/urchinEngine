#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>

namespace urchin {

    class LightMaskNameDialog final : public QDialog {
        Q_OBJECT

        public:
            LightMaskNameDialog(QWidget*, const std::array<std::string, 8>&);

            std::array<std::string, 8> getLightMaskNames() const;

        private:
            std::array<QLineEdit*, 8> lightMaskNameTexts;
    };

}
