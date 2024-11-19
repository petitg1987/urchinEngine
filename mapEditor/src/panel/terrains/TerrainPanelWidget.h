#pragma once

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <UrchinCommon.h>

#include <controller/terrains/TerrainController.h>
#include <panel/terrains/TerrainTableView.h>

namespace urchin {

    class TerrainPanelWidget final : public QWidget, public Observer {
        Q_OBJECT

        public:
            TerrainPanelWidget();
            ~TerrainPanelWidget() override = default;

            TerrainTableView* getTerrainTableView() const;

            void load(TerrainController&);
            void unload();

        private:
            void setupGeneralPropertiesBox(QVBoxLayout*);
            void setupMeshBox(QVBoxLayout*);
            void setupMaterialBox(QVBoxLayout*);
            void setupGrassBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupTerrainDataFrom(const TerrainEntity*);

            TerrainController* terrainController;

            TerrainTableView* terrainTableView;
            QPushButton* addTerrainButton;
            QPushButton* removeTerrainButton;

            QGroupBox* generalPropertiesGroupBox;
            QGroupBox* meshGroupBox;
            QGroupBox* materialGroupBox;
            QGroupBox* grassGroupBox;

            bool disableTerrainEvent;

            //general properties
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* ambient;

            //mesh properties
            QDoubleSpinBox* xzScale;
            QDoubleSpinBox* yScale;

            //material properties
            QDoubleSpinBox* sRepeat;
            QDoubleSpinBox* tRepeat;
            QLineEdit* maskMapFilenameText;
            static QString preferredMaskMapPath;
            std::vector<QLineEdit*> materialFilenameTexts;
            static QString preferredMaterialPath;

            //grass properties
            QLineEdit* grassTextureFilenameText;
            static QString preferredGrassTexturePath;
            QLineEdit* grassMaskFilenameText;
            static QString preferredGrassMaskPath;
            QSpinBox* numGrassInTex;
            QDoubleSpinBox* grassQuantity;
            QDoubleSpinBox* grassHeight;
            QDoubleSpinBox* grassWidth;
            QDoubleSpinBox* windDirectionX;
            QDoubleSpinBox* windDirectionY;
            QDoubleSpinBox* windDirectionZ;
            QDoubleSpinBox* windStrength;

        private slots:
            void showAddTerrainDialog();
            void removeSelectedTerrain() const;

            void updateTerrainGeneralProperties() const;
            void updateTerrainMesh() const;
            void updateTerrainMaterial() const;
            void updateTerrainGrass() const;

            void showMaskFilenameDialog();
            void clearMaskFilename() const;
            void showMaterialFilenameDialog(const QString&);
            void clearMaterialFilename(const QString&) const;

            void showGrassTextureFilenameDialog();
            void clearGrassTextureFilename() const;
            void showGrassMaskFilenameDialog();
            void clearGrassMaskFilename() const;
    };

}
