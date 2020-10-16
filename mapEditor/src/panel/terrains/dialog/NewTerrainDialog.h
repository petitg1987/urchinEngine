#ifndef URCHINENGINE_NEWTERRAINDIALOG_H
#define URCHINENGINE_NEWTERRAINDIALOG_H

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include "UrchinMapHandler.h"
#include "controller/terrains/TerrainController.h"

namespace urchin {

    class NewTerrainDialog : public QDialog {
        Q_OBJECT

        public:
            NewTerrainDialog(QWidget *parent, const TerrainController *);

            SceneTerrain *getSceneTerrain() const;

        private:
            void setupNameFields(QGridLayout *);
            void setupHeightFilenameFields(QGridLayout *);

            void updateTerrainName();
            int buildSceneTerrain(int);

            void done(int) override;
            bool isSceneTerrainExist(const std::string &);

            const TerrainController *terrainController;

            QLabel *terrainNameLabel;
            QLineEdit *terrainNameText;
            QLabel *heightFilenameLabel;
            QLineEdit *heightFilenameText;

            std::string terrainName;
            std::string heightFilename;
            SceneTerrain *sceneTerrain;
            static QString preferredHeightPath;

        private slots:
            void showHeightFilenameDialog();
    };

}

#endif
