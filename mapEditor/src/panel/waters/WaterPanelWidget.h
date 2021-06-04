#pragma once

#include <string>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <UrchinCommon.h>

#include <controller/waters/WaterController.h>
#include <panel/waters/WaterTableView.h>

namespace urchin {

    class WaterPanelWidget : public QWidget, public Observer {
        Q_OBJECT

        public:
            WaterPanelWidget();
            ~WaterPanelWidget() override = default;

            WaterTableView* getWaterTableView() const;

            void load(WaterController*);
            void unload();

        private:
            void setupGeneralPropertiesBox(QVBoxLayout*);
            void setupWaterSurfaceProperties(QVBoxLayout*);
            void setupUnderWaterProperties(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupWaterDataFrom(const SceneWater*);

            WaterController* waterController;

            WaterTableView* waterTableView;
            QPushButton* addWaterButton;
            QPushButton* removeWaterButton;

            QGroupBox* generalPropertiesGroupBox;
            QGroupBox* waterSurfacePropertiesGroupBox;
            QGroupBox* underWaterPropertiesGroupBox;

            bool disableWaterEvent;

            //general properties
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* xSize;
            QDoubleSpinBox* zSize;

            //surface water properties
            QDoubleSpinBox* waterColorR;
            QDoubleSpinBox* waterColorG;
            QDoubleSpinBox* waterColorB;
            QLineEdit* normalTextureFilenameText;
            QLineEdit* dudvMapFilenameText;
            static QString preferredNormalTexturePath, preferredDudvMapPath;
            QDoubleSpinBox* waveSpeed;
            QDoubleSpinBox* waveStrength;
            QDoubleSpinBox* sRepeat;
            QDoubleSpinBox* tRepeat;

            //under water properties
            QDoubleSpinBox* density;
            QDoubleSpinBox* gradient;

        private slots:
            void showAddWaterDialog();
            void removeSelectedWater();

            void updateWaterProperties();
            void updateSurfaceWaterProperties();
            void updateUnderWaterProperties();

            void showNormalTextureFilenameDialog();
            void clearNormalTextureFilename();

            void showDudvMapFilenameDialog();
            void clearDudvMapFilename();
    };

}
