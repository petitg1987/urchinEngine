#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinAggregation.h>
#include <controller/terrains/TerrainController.h>

namespace urchin {

    class NewTerrainDialog : public QDialog {
        Q_OBJECT

        public:
            NewTerrainDialog(QWidget*, const TerrainController*);

            std::unique_ptr<TerrainEntity> moveTerrainEntity();

        private:
            void setupNameFields(QGridLayout*);
            void setupHeightFilenameFields(QGridLayout*);

            void updateTerrainName();
            int buildTerrainEntity(int);

            void done(int) override;
            bool isTerrainEntityExist(const std::string&);

            const TerrainController* terrainController;

            QLabel* terrainNameLabel;
            QLineEdit* terrainNameText;
            QLabel* heightFilenameLabel;
            QLineEdit* heightFilenameText;

            std::string terrainName;
            std::string heightFilename;
            std::unique_ptr<TerrainEntity> terrainEntity;
            static QString preferredHeightPath;

        private slots:
            void showHeightFilenameDialog();
    };

}
