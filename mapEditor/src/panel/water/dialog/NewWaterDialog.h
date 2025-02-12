#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinAggregation.h>
#include <controller/water/WaterController.h>

namespace urchin {

    class NewWaterDialog final : public QDialog {
        Q_OBJECT

        public:
            NewWaterDialog(QWidget*, const WaterController*);

            std::unique_ptr<WaterEntity> moveWaterEntity();

        private:
            void setupNameFields(QGridLayout*);

            void updateWaterName();
            int buildWaterEntity(int);

            void done(int) override;
            bool isWaterEntityExist(std::string_view) const;

            const WaterController* waterController;

            QLabel* waterNameLabel;
            QLineEdit* waterNameText;

            std::string waterName;
            std::unique_ptr<WaterEntity> waterEntity;
    };

}
