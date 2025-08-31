#pragma once

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <UrchinCommon.h>

#include "controller/terrain/TerrainController.h"
#include "panel/terrain/TerrainTableView.h"
#include "widget/BitsetComboBox.h"

namespace urchin {

    class TerrainPanelWidget final : public QWidget, public Observer {
        Q_OBJECT

        public:
            TerrainPanelWidget();
            ~TerrainPanelWidget() override = default;

            TerrainTableView* getTerrainTableView() const;

            void load(SceneController&);
            void unload();

        private:
            void setupGeneralPropertiesBox(QVBoxLayout*);
            void setupMeshBox(QVBoxLayout*);
            void setupMaterialBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupTerrainDataFrom(const TerrainEntity*);

            TerrainController* terrainController;

            TerrainTableView* terrainTableView;
            QPushButton* addTerrainButton;
            QPushButton* removeTerrainButton;

            QGroupBox* generalPropertiesGroupBox;
            QGroupBox* meshGroupBox;
            QGroupBox* materialGroupBox;

            bool disableTerrainEvent;

            //general properties
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* ambient;
            BitsetComboBox* lightMask;

            //mesh properties
            QDoubleSpinBox* xzScale;
            QDoubleSpinBox* yScale;
            QCheckBox* flatMode;

            //material properties
            QDoubleSpinBox* sRepeat;
            QDoubleSpinBox* tRepeat;
            QLineEdit* maskMapFilenameText;
            static QString preferredMaskMapPath;
            std::vector<QLineEdit*> materialFilenameTexts;
            static QString preferredMaterialPath;

        private slots:
            void showAddTerrainDialog();
            void removeSelectedTerrain() const;

            void updateTerrainGeneralProperties() const;
            void updateTerrainMesh() const;
            void updateTerrainMaterial() const;

            void showMaskFilenameDialog();
            void clearMaskFilename() const;
            void showMaterialFilenameDialog(const QString&);
            void clearMaterialFilename(const QString&) const;
    };

}
