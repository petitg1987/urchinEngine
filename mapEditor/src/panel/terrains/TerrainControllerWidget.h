#ifndef URCHINENGINE_MAPEDITOR_TERRAINCONTROLLERWIDGET_H
#define URCHINENGINE_MAPEDITOR_TERRAINCONTROLLERWIDGET_H

#include <string>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include "UrchinCommon.h"

#include "controller/terrains/TerrainController.h"
#include "panel/terrains/TerrainTableView.h"

#define MAX_MATERIAL 4 //maximum 4 materials (RGBA)

namespace urchin
{

    class TerrainControllerWidget : public QWidget, public Observer
    {
        Q_OBJECT

        public:
            TerrainControllerWidget();
            ~TerrainControllerWidget() override = default;

            TerrainTableView *getTerrainTableView() const;

            void load(TerrainController *);
            void unload();

        private:
            void setupGeneralPropertiesBox(QVBoxLayout *);
            void setupMeshBox(QVBoxLayout *);
            void setupMaterialBox(QVBoxLayout *);
            void setupGrassBox(QVBoxLayout *);

            void notify(Observable *, int) override;
            void setupTerrainDataFrom(const SceneTerrain *);

            TerrainController *terrainController;

            TerrainTableView *terrainTableView;
            QPushButton *addTerrainButton;
            QPushButton *removeTerrainButton;

            QGroupBox *generalPropertiesGroupBox;
            QGroupBox *meshGroupBox;
            QGroupBox *materialGroupBox;
            QGroupBox *grassGroupBox;

            bool disableTerrainEvent;

            //general properties
            QDoubleSpinBox *positionX, *positionY, *positionZ;
            QDoubleSpinBox *ambient;

            //mesh properties
            QDoubleSpinBox *xzScale, *yScale;

            //material properties
            QDoubleSpinBox *sRepeat, *tRepeat;
            QLineEdit *maskMapFilenameText;
            static QString preferredMaskMapPath;
            std::vector<QLineEdit *> materialFilenameTexts;
            static QString preferredMaterialPath;

            //grass properties
            QLineEdit *grassTextureFilenameText;
            static QString preferredGrassTexturePath;
            QLineEdit *grassMaskFilenameText;
            static QString preferredGrassMaskPath;
            QSpinBox *numGrassInTex;
            QDoubleSpinBox *grassQuantity, *grassHeight, *grassLength;
            QDoubleSpinBox *windDirectionX, *windDirectionY, *windDirectionZ, *windStrength;

        private slots:
            void showAddTerrainDialog();
            void removeSelectedTerrain();

            void updateTerrainGeneralProperties();
            void updateTerrainMesh();
            void updateTerrainMaterial();
            void updateTerrainGrass();

            void showMaskFilenameDialog();
            void clearMaskFilename();
            void showMaterialFilenameDialog(const QString &);
            void clearMaterialFilename(const QString &);

            void showGrassTextureFilenameDialog();
            void clearGrassTextureFilename();
            void showGrassMaskFilenameDialog();
            void clearGrassMaskFilename();
    };

}

#endif
