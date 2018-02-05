#ifndef URCHINENGINE_MAPEDITOR_WATERCONTROLLERWIDGET_H
#define URCHINENGINE_MAPEDITOR_WATERCONTROLLERWIDGET_H

#include <string>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include "UrchinCommon.h"

#include "scene/controller/waters/WaterController.h"
#include "scene/controller/waters/WaterTableView.h"

namespace urchin
{

    class WaterControllerWidget : public QWidget, public Observer
    {
        Q_OBJECT

        public:
            WaterControllerWidget();
            ~WaterControllerWidget() override = default;

            WaterTableView *getWaterTableView() const;

            void load(WaterController *);
            void unload();

        private:
            void setupGeneralPropertiesBox(QVBoxLayout *);
            void setupUnderWaterProperties(QVBoxLayout *);

            void notify(Observable *, int) override;
            void setupWaterDataFrom(const SceneWater *);

            WaterController *waterController;

            WaterTableView *waterTableView;
            QPushButton *addWaterButton;
            QPushButton *removeWaterButton;

            QGroupBox *generalPropertiesGroupBox;
            QGroupBox *underWaterPropertiesGroupBox;

            bool disableWaterEvent;

            //general properties
            QDoubleSpinBox *positionX, *positionY, *positionZ;
            QDoubleSpinBox *xSize, *zSize;
            QDoubleSpinBox *waterColorR, *waterColorG, *waterColorB;
            QLineEdit *normalTextureFilenameText, *dudvMapFilenameText;
            static QString preferredNormalTexturePath, preferredDudvMapPath;
            QDoubleSpinBox *sRepeat, *tRepeat;

            //under water properties
            QDoubleSpinBox *density, *gradient;

        private slots:
            void showAddWaterDialog();
            void removeSelectedWater();

            void updateWaterProperties();
            void updateUnderWaterProperties();

            void showNormalTextureFilenameDialog();
            void clearNormalTextureFilename();

            void showDudvMapFilenameDialog();
            void clearDudvMapFilename();
    };

}

#endif
